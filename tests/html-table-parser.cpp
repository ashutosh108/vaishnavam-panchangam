#include "html-parser.h"
#include "html-table-parser.h"

#include <algorithm>
#include <cctype>
#include "tinyfsm.hpp"

namespace html {

std::string Table::get(std::size_t row, std::size_t column)
{
    return data.at(row).at(column);
}

std::size_t Table::find_first_free_col_at_row(std::size_t row) {
    auto & row_data = get_row(row);
    if (row_data.empty()) return 0;
    auto last_possible_col = row_data.rbegin()->first; // key of last element in row is the last candidate for linear search
    for (std::size_t col=0; col <= last_possible_col; ++col) {
        if (row_data.find(col) == row_data.end()) {
            return col;
        }
    }
    return last_possible_col+1;
}

void Table::append_cell(std::size_t row, std::string s, RowSpan rowspan_, ColSpan colspan_)
{
    std::size_t col = find_first_free_col_at_row(row);

    for (std::size_t row_counter=0; row_counter < rowspan_.row_count; ++row_counter) {
        for (std::size_t col_counter=0; col_counter < colspan_.col_count; ++col_counter) {
            set(row + row_counter, col+col_counter, s);
        }
    }
}

std::size_t Table::get_row_length(std::size_t row)
{
    return data.at(row).size();
}

std::size_t Table::row_count()
{
    return data.size();
}

Table::Row & Table::get_row(std::size_t row)
{
    if (row >= data.size()) {
        data.resize(row+1);
    }
    return data.at(row);
}

std::string & Table::set(std::size_t row, std::size_t col, std::string s)
{
    auto & row_data = get_row(row);
    return row_data[col] = s;
}

std::string trim(std::string_view s) {
    auto first_non_ws = std::find_if(s.begin(), s.end(), [](char c){ return !std::isspace(c); });
    auto last_non_ws = std::find_if(s.rbegin(), s.rend(), [](char c){ return !std::isspace(c); }).base();
    return first_non_ws >= last_non_ws ? std::string{} : std::string(first_non_ws, last_non_ws);
}

// Events
struct TableOpen : tinyfsm::Event {};
struct TableClose : tinyfsm::Event {};
struct TdOpen : tinyfsm::Event {
    Table::RowSpan row_span{1};
    Table::ColSpan col_span{1};
    std::string_view text_after;
    TdOpen(Table::RowSpan row_span_, Table::ColSpan col_span_, std::string_view text_after_)
        : row_span(row_span_), col_span(col_span_), text_after(text_after_) {}
};
struct TdClose : tinyfsm::Event {};
struct TrOpen : tinyfsm::Event {};
struct TrClose : tinyfsm::Event {};
struct OtherTag : tinyfsm::Event {
    std::string_view text_after;
    OtherTag(std::string_view text_after_)
        : text_after(text_after_) {}
};
struct EndStream : tinyfsm::Event {};

// Finite State Machine
class WaitTdTag;
class ParserMachine : public tinyfsm::Fsm<ParserMachine> {
public:
    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &) { }

    static void start() {
        ParserMachine::row = 0;
        ParserMachine::got_table = false;
        ParserMachine::got_tr_or_td = false;
        ParserMachine::t = {};
        tinyfsm::Fsm<ParserMachine>::start();
    }

    virtual void react(TableOpen const &) {}
    virtual void react(TableClose const &) {}
    virtual void react(TrOpen const &) {}
    virtual void react(TrClose const &) {}
    virtual void react(TdOpen const &) {}
    virtual void react(TdClose const &) {}
    virtual void react(OtherTag const &) {}
    virtual void react(EndStream const &) {
        transit<WaitTdTag>();
    }
    virtual ~ParserMachine()=default;
    virtual void entry(void) {}
    virtual void exit(void);

    inline static std::size_t row = 0;
    inline static bool got_table = false;
    inline static bool got_tr_or_td = false;
    inline static Table t{};
    inline static std::string td_text{};
    inline static Table::RowSpan row_span{};
    inline static Table::ColSpan col_span{};
};
void ParserMachine::exit() {}

// States
class WaitTableTag : public ParserMachine {
    virtual void react(const TableOpen & /*event*/) override;
private:
    using ParserMachine::react;
};

void WaitTableTag::react(const TableOpen &)
{
    got_table = true;
    transit<WaitTdTag>();
}

class InsideTdTag;
class WaitTdTag : public ParserMachine {
    void react(const TrOpen &) override {
        if (got_tr_or_td) ++row;
        got_tr_or_td = true;
    }
    void react(const TdOpen & event) override {
        got_tr_or_td = true;
        row_span = event.row_span;
        col_span = event.col_span;
        td_text = trim(event.text_after);
        transit<InsideTdTag>();
    }
private:
    using ParserMachine::react;
};

class InsideTdTag : public ParserMachine {
    void react(const TrOpen &) override {
        transit<WaitTdTag>([]() { ++row; });
    }
    void react(const TrClose &) override {
        transit<WaitTdTag>();
    }
    void react(const TdClose &) override {
        transit<WaitTdTag>();
    }
    void react(const TdOpen & event) override {
        transit<class InsideTdTag>([&] {
            row_span = event.row_span;
            col_span = event.col_span;
            td_text = trim(event.text_after);
        });
    }
    void react(const OtherTag & event) override {
        //append space unless string already ends on space
        if (!td_text.empty() && !std::isspace(*td_text.rbegin())) {
            td_text += " ";
        }
        td_text += trim(event.text_after);
    }
    void exit() override {
        t.append_cell(row, trim(td_text), row_span, col_span);
    }
private:
    using ParserMachine::react;
};

} // namespace html

// have to declare this in the global namespace (probably due to tinyfsm bug)
FSM_INITIAL_STATE(html::ParserMachine, html::WaitTableTag)

namespace html {

std::optional<Table> TableParser::next_table()
{
    ParserMachine::start();
    while (auto token = token_stream.next_token()) {
        if (token->tag_name == "table") {
            ParserMachine::dispatch(TableOpen{});
        } else if (token->tag_name == "/table") {
            ParserMachine::dispatch(TableClose{});
        } else if (token->tag_name == "tr") {
            ParserMachine::dispatch(TrOpen{});
        } else if (token->tag_name == "/tr") {
            ParserMachine::dispatch(TrClose{});
        } else if (token->tag_name == "td") {
            Table::RowSpan row_span{token->get_attr_ul_or_default("rowspan", 1)};
            Table::ColSpan col_span{token->get_attr_ul_or_default("colspan", 1)};
            ParserMachine::dispatch(TdOpen(row_span, col_span, token->text_after));
        } else if (token->tag_name == "/td") {
            ParserMachine::dispatch(TdClose{});
        } else {
            ParserMachine::dispatch(OtherTag{token->text_after});
        }
    }
    ParserMachine::dispatch(EndStream{});

    if (!ParserMachine::got_table) {
        return {};
    }
    return {ParserMachine::t};
}

} // namespace html

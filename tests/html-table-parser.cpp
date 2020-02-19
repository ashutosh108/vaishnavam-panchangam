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

void Table::append_cell(std::size_t row, std::string && s, RowSpan rowspan_, ColSpan colspan_)
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

// Finite State Machine
class ParserMachine : public tinyfsm::Fsm<ParserMachine> {
public:
    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &) { }

    virtual void react(TableOpen const &) {}
    virtual void react(TableClose const &) {}
    virtual void react(TrOpen const &) {}
    virtual void react(TrClose const &) {}
    virtual void react(TdOpen const &) {}
    virtual void react(TdClose const &) {}
    virtual ~ParserMachine()=default;
    virtual void entry(void) {}
    virtual void exit(void) {}
    inline static std::size_t row = 0;
    inline static bool got_table = false;
    inline static bool got_tr_or_td = false;
    inline static Table t{};
};

// States
class WaitTdTag;
class WaitTableTag : public ParserMachine {
    void react(const TableOpen & /*event*/) override {
        got_table = true;
        transit<WaitTdTag>();
    }
};

class WaitTdTag : public ParserMachine {
    void react(const TrOpen & /*event*/) override {
        if (got_tr_or_td) ++row;
        got_tr_or_td = true;
    }
    void react(const TdOpen & event) override {
        got_tr_or_td = true;
        t.append_cell(row, trim(event.text_after), event.row_span, event.col_span);
    }
};

}

// have to declare this in the global namespace (probably due to tinyfsm bug)
FSM_INITIAL_STATE(html::ParserMachine, html::WaitTableTag)

namespace html {

std::optional<Table> TableParser::next_table()
{
    ParserMachine::row = 0;
    ParserMachine::got_table = false;
    ParserMachine::got_tr_or_td = false;
    ParserMachine::t = {};
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
        }
    }

    if (!ParserMachine::got_table) {
        return {};
    }
    return {ParserMachine::t};
}

} // namespace html

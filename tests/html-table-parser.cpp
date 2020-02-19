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

std::optional<Table> TableParser::next_table()
{
    enum class State { WaitTableTag, WaitTdTag };
    State state{State::WaitTableTag};
    std::size_t row{0};
    bool got_table = false;
    bool got_tr_or_td = false;
    Table t;
    while (auto token = token_stream.next_token()) {
        switch (state) {
        case State::WaitTableTag:
            if (token->tag_name == "table") {
                got_table = true;
                state = State::WaitTdTag;
            }
            break;
        case State::WaitTdTag:
            if (token->tag_name == "tr") {
                // first <tr> means we get 0-th row. Next <tr>s increment row count.
                if (got_tr_or_td) {
                    ++row;
                }
                got_tr_or_td = true;
            } else if (token->tag_name == "td") {
                got_tr_or_td = true;
                std::size_t colspan = token->get_attr_ul_or_default("colspan", 1);
                std::size_t rowspan = token->get_attr_ul_or_default("rowspan", 1);
                t.append_cell(row, trim(token->text_after), Table::RowSpan{rowspan}, Table::ColSpan{colspan});
            }
            break;
        }
    }

    if (!got_table) {
        return {};
    }
    return {t};
}

} // namespace html

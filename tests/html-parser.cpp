#include "html-parser.h"

#include <cassert>
#include <cctype>
#include <charconv>
#include <iostream>
#include <regex>

std::optional<html::Token> html::TokenStream::next_token()
{
    enum class State {
        SkipUntilTagStart, SkipUntilTagName, GetTagName, GetAttributes, GetTrail, End
    };
    State state{State::SkipUntilTagStart};
    std::string::const_iterator tag_name_start = iter;
    std::string::size_type tag_name_size{0};
    std::string::const_iterator attributes_start = iter;
    std::string::size_type attributes_size{0};
    std::string::const_iterator trail_start = iter;
    std::string::size_type trail_size{0};

    for (; iter != html.end() && state != State::End; ++iter) {
        auto c = *iter;
        switch (state) {
        case State::SkipUntilTagStart:
            if (c == '<') {
                state = State::SkipUntilTagName;
            }
            break;
        case State::SkipUntilTagName:
            if (std::isalpha(c) || c == '/') {
                tag_name_start = iter;
                state = State::GetTagName;
            }
            break;
        case State::GetTagName:
            if (std::isalpha(c) || c == '/') {
            } else {
                tag_name_size = static_cast<std::string::size_type>(iter - tag_name_start);
                attributes_start = iter;
                trail_start = iter+1;
                state = (c == '>' ? State::GetTrail : State::GetAttributes);
            }
            break;
        case State::GetAttributes:
            if (c != '>') {
            } else {
                attributes_size = static_cast<std::string::size_type>(iter - attributes_start);
                trail_start = iter+1;
                state = State::GetTrail;
            }
            break;
        case State::GetTrail:
            if (c != '<') {
            } else {
                trail_size = static_cast<std::string::size_type>(iter - trail_start);
                --iter; // leave '<' char to be consumed during next call to next_token()
                state = State::End;
            }
            break;
        case State::End:
            assert(false);
        }
    }

    // if we exited the loop because end of string, make sure we set proper length for trail
    if (state == State::GetTrail) {
        trail_size = static_cast<std::string::size_type>(iter - trail_start);
    }

    std::string_view tag_name(&(*tag_name_start), tag_name_size);
    std::string_view attributes(&(*attributes_start), attributes_size);
    std::string_view trail(&(*trail_start), trail_size);

    if (tag_name.empty()) {
        return std::nullopt;
    }

    return Token{tag_name, attributes, trail};
}

std::string html::Table::get(std::size_t row, std::size_t column)
{
    return data.at(row).at(column);
}

void html::Table::append_cell(std::size_t row_, std::string && s, rowspan rowspan_, colspan colspan_)
{
    for (std::size_t row=row_; row < row_ + rowspan_.row_count; ++row) {
        if (data.size() < row+1) {
            data.resize(row+1);
        }
        for (std::size_t col_counter=0; col_counter < colspan_.col_count; ++col_counter) {
            data[row].push_back(s);
        }
    }
}

std::size_t html::Table::get_row_length(std::size_t row)
{
    return data[row].size();
}

std::size_t html::Table::row_count()
{
    return data.size();
}

std::vector<std::string> &html::Table::get_row(std::size_t row)
{
    return data.at(row);
}

std::optional<html::Table> html::TableParser::next_table()
{
    enum class State { WaitTableTag, WaitTdTag };
    State state{State::WaitTableTag};
    std::size_t row{0};
    bool got_table = false;
    bool got_tr = false;
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
                if (got_tr) {
                    ++row;
                }
                got_tr = true;
            } else if (token->tag_name == "td") {
                std::size_t colspan = 1;
                auto colspan_str = token->get_attr("colspan");
                if (colspan_str) {
                    colspan = std::stoul(std::string{*colspan_str});
                }

                std::size_t rowspan = 1;
                auto rowspan_str = token->get_attr("rowspan");
                if (rowspan_str) {
                    rowspan = std::stoul(std::string{*rowspan_str});
                }

                // To account for cells pre-filled with rowspan from previous rows,
                // adjust col number.
                t.append_cell(row, std::string{token->text_after}, Table::rowspan{rowspan}, Table::colspan{colspan});
            }
            break;
        }
    }

    if (!got_table) {
        return {};
    }
    return {t};
}

std::optional<std::string> html::Token::get_attr(std::string attr)
{
    std::string regex_str{std::string{R"~(\b)~"} + attr + R"~(\s*=\s*["']?(\d+)["']?\b)~"};
    std::regex regex{regex_str};
    std::smatch match;
    std::string attr_string{tag_attributes};
    if (std::regex_search(attr_string, match, regex)) {
        if (match.size() >= 2) {
            return match[1].str();
        }
    }
    return std::nullopt;
}

unsigned long html::Token::get_attr_ul_or_default(std::string attr, unsigned long default_val)
{
    auto val = get_attr(attr);
    if (val) {
        const char * begin = val->data();
        const char * end = val->data() + val->size();
        unsigned long int_val;
        auto [p, errc] = std::from_chars(begin, end, int_val);
        if (errc == std::errc()) {
            return int_val;
        }
    }
    return default_val;
}

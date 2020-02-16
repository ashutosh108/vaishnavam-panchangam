#ifndef VP_TESTS_HTML_PARSER_H
#define VP_TESTS_HTML_PARSER_H

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* This is a non-validating HTML parser created only to parse HTML <table>s in test data for auto-generated tests.
 * - Optimized for HTML text to fit in memory
 * - Only works for non-negative int values in attributes
 * - Tested for simple cases with colspan and rowspan
 *
 * Usage:
 *  TableParser p{"<table>...</table>");
 *  auto t = p.next_table();
 *  if (t) {
 *    // use t.get(row, col) for valid row and col values
 *  }
 */

namespace html {

struct Token {
    std::string_view tag_name;
    std::string_view tag_attributes;
    std::string_view text_after;
    std::optional<std::string> get_attr(std::string attr);
    unsigned long get_attr_ul_or_default(std::string attr, unsigned long default_val);
};

class TokenStream {
    std::string html;
    std::string::const_iterator iter;
public:
    TokenStream(std::string html_) : html(html_), iter(html.begin()) {}
    std::optional<Token> next_token();
};

class Table {
public:
    std::string get(std::size_t row, std::size_t column);

    struct RowSpan {
        std::size_t row_count;
    };

    struct ColSpan {
        std::size_t col_count;
    };
    using Row = std::map<std::size_t, std::string>;

    void append_cell(std::size_t row, std::string && s, RowSpan rowspan_=RowSpan{1}, ColSpan colspan_=ColSpan{1});
    std::size_t get_row_length(std::size_t row);
    std::size_t row_count();
    Row  & get_row(std::size_t row);
    std::string & set(std::size_t row, std::size_t col, std::string s);
private:
    std::vector<Row> data;
    std::size_t find_first_free_col_at_row(std::size_t row);
};

class TableParser {
    TokenStream token_stream;
public:
    TableParser(std::string && html) : token_stream{html} {}
    std::optional<Table> next_table();
};

}

#endif

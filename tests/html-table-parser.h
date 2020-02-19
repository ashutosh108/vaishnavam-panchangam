#ifndef VP_TESTS_HTML_TABLE_PARSER_H
#define VP_TESTS_HTML_TABLE_PARSER_H

#include "html-parser.h"

#include <map>
#include <string>
#include <vector>

namespace html {

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

} // namespace html

#endif

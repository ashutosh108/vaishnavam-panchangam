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
    std::optional<std::string> get_attr(const std::string & attr);
    unsigned long get_attr_ul_or_default(const std::string & attr, unsigned long default_val);
};

class TokenStream {
    std::string html;
    std::string::const_iterator iter;
public:
    TokenStream(std::string html_) : html(html_), iter(html.begin()) {}
    std::optional<Token> next_token();
};

}

#endif

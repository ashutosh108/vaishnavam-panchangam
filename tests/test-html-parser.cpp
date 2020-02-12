#include "catch.hpp"

#include "html-parser.h"

TEST_CASE("HtmlTokenStream can stream tokens") {
    html::TokenStream p("<html>");
    auto token = p.next_token();
    REQUIRE(token.has_value());
    REQUIRE("html" == token->tag_name);
}

TEST_CASE("HtmlTokenStream returns empty token at the end") {
    html::TokenStream p("<html>");
    auto token = p.next_token();
    REQUIRE(token.has_value());
    REQUIRE("html" == token->tag_name);

    auto token2 = p.next_token();
    REQUIRE(!token2.has_value());
}

TEST_CASE("HtmlTokenStream can stream three tokens") {
    html::TokenStream p("<tag attr=val>text<tag attr=val>text<tag attr=val>text");

    int count = 0;
    while (auto token = p.next_token()) {
        REQUIRE(token->tag_name == "tag");
        REQUIRE(token->tag_attributes == " attr=val");
        REQUIRE(token->text_after == "text");
        ++count;
    }
    REQUIRE(count == 3);
}

TEST_CASE("HtmlTokenStream can handle closing tags") {
    html::TokenStream p{"<html></html>"};
    int count = 0;
    while (auto token = p.next_token()) {
        if (count == 0)  {
            REQUIRE(token->tag_name == "html");
        } else {
            REQUIRE(token->tag_name == "/html");
        }
        ++count;
    }
}

TEST_CASE("HtmlTable can set and get elements") {
    html::Table t;
    t.append_cell(0, "qwe");
    t.append_cell(1, "asd");
    REQUIRE("qwe" == t.get(0, 0));
    REQUIRE("asd" == t.get(1, 0));
}

TEST_CASE("HtmlTable can set elements with rowspan and colspan") {
    html::Table t;
    t.append_cell(0, "qwe", html::Table::rowspan{4});
    REQUIRE("qwe" == t.get(0, 0));
    REQUIRE("qwe" == t.get(1, 0));
    REQUIRE("qwe" == t.get(2, 0));
    REQUIRE("qwe" == t.get(3, 0));

    t.append_cell(0, "asd", html::Table::rowspan{1}, html::Table::colspan{4});
    REQUIRE("asd" == t.get(0, 1));
    REQUIRE("asd" == t.get(0, 2));
    REQUIRE("asd" == t.get(0, 3));
    REQUIRE("asd" == t.get(0, 4));
}

TEST_CASE("HtmlTableParser can parse simple html table") {
    html::TableParser p{"<table><tr><td>0 0</td><td>0 1</td></tr><tr><td>1 0</td><td>1 1</td></tr></table>"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0" == t->get(0, 0));
    REQUIRE("0 1" == t->get(0, 1));
    REQUIRE("1 0" == t->get(1, 0));
    REQUIRE("1 1" == t->get(1, 1));
}

TEST_CASE("Token get_attr_ul_or_default works") {
    html::TokenStream p{"<tag attr=\"123\" attr2=\"456\" >"};
    auto t = p.next_token();
    REQUIRE(t.has_value());

    REQUIRE(123UL == t->get_attr_ul_or_default("attr", 3UL));
    REQUIRE(456UL == t->get_attr_ul_or_default("attr2", 3UL));
}

TEST_CASE("HtmlTableParser can keep count of column numbers in simple html table with rowspan") {
    html::TableParser p{R"~(<table>
                        <tr><td rowspan="2">0 0 & 1 0</td><td>0 1</td></tr>
                        <tr><td colspan=2>1 1 & 1 2</td></tr></table>)~"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0 & 1 0" == t->get(0, 0));
    REQUIRE("0 1" == t->get(0, 1));
    REQUIRE("0 0 & 1 0" == t->get(1, 0));
    REQUIRE("1 1 & 1 2" == t->get(1, 1));
    REQUIRE("1 1 & 1 2" == t->get(1, 2));
}

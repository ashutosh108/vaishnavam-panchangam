#include "catch.hpp"

#include "html-table-parser.h"

TEST_CASE("HtmlTable can set and get elements") {
    html::Table t;
    t.append_cell(0, "qwe");
    t.append_cell(1, "asd");
    REQUIRE("qwe" == t.get(0, 0));
    REQUIRE("asd" == t.get(1, 0));
}

TEST_CASE("HtmlTable can set elements with rowspan and colspan") {
    html::Table t;
    t.append_cell(0, "qwe", html::Table::RowSpan{4});
    REQUIRE("qwe" == t.get(0, 0));
    REQUIRE("qwe" == t.get(1, 0));
    REQUIRE("qwe" == t.get(2, 0));
    REQUIRE("qwe" == t.get(3, 0));

    t.append_cell(0, "asd", html::Table::RowSpan{1}, html::Table::ColSpan{4});
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

TEST_CASE("html::TableParser understands when <tr> was missing for the firest line, but is present for the next one") {
    html::TableParser p{R"~(<table><td>0 0<tr><td>1 0)~"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0" == t->get(0, 0));
    REQUIRE("1 0" == t->get(1, 0));
}

TEST_CASE("html::TableParser can handle rowspans when long col comes before the short one") {
    html::TableParser p{R"~(<table><td>0 0<td rowspan="2">0..1 1<td rowspan="3">0..2 2</td>
                        <tr><td>1 0</td>
                        <tr><td>2 0<td>2 1</table>
                        )~"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0" == t->get(0, 0));
    REQUIRE("1 0" == t->get(1, 0));
    REQUIRE("2 0" == t->get(2, 0));
    REQUIRE("0..1 1" == t->get(0, 1));
    REQUIRE("0..1 1" == t->get(1, 1));
    REQUIRE("2 1" == t->get(2, 1));
    REQUIRE("0..2 2" == t->get(0, 2));
    REQUIRE("0..2 2" == t->get(1, 2));
    REQUIRE("0..2 2" == t->get(2, 2));
}

TEST_CASE("html::TableParser trims spaces") {
    html::TableParser p{R"~(<table><td>  0 0    )~"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0" == t->get(0, 0));
}


TEST_CASE("html::TableParser strips <br>s") {
    html::TableParser p{R"~(<table><td>0 <br><br> 0 )~"};
    auto t = p.next_table();
    REQUIRE(t.has_value());
    REQUIRE("0 0" == t->get(0, 0));
}

#include "catch-formatters.h"

#include "html-table-writer.h"

namespace {
std::string table_to_string(const vp::Table & table) {
    std::stringstream stream;
    stream << vp::Html_Table_Writer(table);
    return stream.str();
}
}

TEST_CASE("Html_Table_Writer generates something for a small table") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");

    std::string s = table_to_string(table);

    using Catch::Matchers::StartsWith;
    using Catch::Matchers::Contains;
    REQUIRE_THAT(s, StartsWith("<table"));
    REQUIRE_THAT(s, Contains("<tr"));
    REQUIRE_THAT(s, Contains("<td"));
    REQUIRE_THAT(s, Contains("cell1,1"));
    REQUIRE_THAT(s, Contains("cell2,2"));
    REQUIRE_THAT(s, Contains("</table>"));
}

TEST_CASE("Html_Table_Writer keeps classes for td") {
    vp::Table table;
    table.add_cell("cell1,1", "class1");

    using Catch::Matchers::Contains;
    REQUIRE_THAT(table_to_string(table), Contains("class=\"class1\""));
}

TEST_CASE("Html_Table_Writer writes proper rowspans") {
    vp::Table table;
    table.add_cell("cellx,1");
    table.start_new_row();
    table.add_cell("cellx,1");
    table.merge_cells_into_rowspans();

    std::string str = table_to_string(table);
    using Catch::Matchers::Contains;
    REQUIRE_THAT(str, Contains("rowspan=\"2\""));
}

TEST_CASE("Html_Table_Writer writes proper colspans") {
    vp::Table table;
    table.add_cell("cellx,1");
    table.add_cell("cellx,1");
    table.merge_cells_into_colspans();

    std::string str = table_to_string(table);
    using Catch::Matchers::Contains;
    REQUIRE_THAT(str, Contains("colspan=\"2\""));
    REQUIRE_THAT(str, !Contains("colspan=\"0\""));
}

TEST_CASE("Html_Table_Writer writes every column width in the first non-col-spanning cell of each column") {
    vp::Table table;
    table.add_cell("cell1,1-2");
    table.add_cell("cell1,1-2");
    table.add_cell("cell1,3");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");
    table.add_cell("cell2,3");

    table.set_column_widths({10.0,40.0,50.0});
    table.merge_cells_into_colspans();

    auto s = table_to_string(table);
    using Catch::Matchers::Matches;
    REQUIRE_THAT(s, Matches(R"~((.|\n)*<td\s+[^>]*width="10%"[^>]*>cell2,1(.|\n)*)~"));
    REQUIRE_THAT(s, Matches(R"~((.|\n)*<td\s+[^>]*width="40%"[^>]*>cell2,2(.|\n)*)~"));
    REQUIRE_THAT(s, Matches(R"~((.|\n)*<td\s+[^>]*width="50%"[^>]*>cell1,3(.|\n)*)~"));
}

TEST_CASE("Html_Table_Writer writes cells' 'title' attribute with proper escape") {
    vp::Table table;
    table.add_cell("text").set_title(R"(qwe&'"<>)");
    auto s = table_to_string(table);

    using Catch::Matchers::Contains;
    REQUIRE_THAT(s, Contains(R"(title="qwe&amp;&#039;&quot;&lt;&gt;")"));
}

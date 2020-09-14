#include "catch-formatters.h"

#include "html-table-writer.h"

TEST_CASE("Html_Table_Writer generates something for a small table") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");

    std::stringstream s;
    s << vp::Html_Table_Writer(table);

    using Catch::Matchers::StartsWith;
    using Catch::Matchers::Contains;
    REQUIRE_THAT(s.str(), StartsWith("<table"));
    REQUIRE_THAT(s.str(), Contains("<tr"));
    REQUIRE_THAT(s.str(), Contains("<td"));
    REQUIRE_THAT(s.str(), Contains("cell1,1"));
    REQUIRE_THAT(s.str(), Contains("cell2,2"));
    REQUIRE_THAT(s.str(), Contains("</table>"));
}

TEST_CASE("Html_Table_Writer keeps classes for td") {
    vp::Table table;
    table.add_cell("cell1,1", "class1");

    std::stringstream s;
    s << vp::Html_Table_Writer(table);

    using Catch::Matchers::Contains;
    REQUIRE_THAT(s.str(), Contains("class=\"class1\""));
}

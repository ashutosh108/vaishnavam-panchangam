#include "catch-formatters.h"

#include "table-calendar-generator.h"
#include "text-interface.h"

#include <sstream>

static auto some_vratas() {
    using namespace date;
    const auto arbitrary_date = 2020_y/January/1;
    return vp::text_ui::calc_all(arbitrary_date);
}

TEST_CASE("Table_Caledar_Generator returns reasonable table") {
    const auto vratas = some_vratas();

    const auto table = vp::Table_Calendar_Generator::generate(vratas);
    REQUIRE(table.width() == 6);
    REQUIRE(table.at(0, 3).text == "6 января");
    REQUIRE(table.at(0, 4).text == "7 января");
    REQUIRE(table.at(0, 5).text == "8 января");

    REQUIRE(table.height() >= 20);

    REQUIRE(table.at(1, 0).text == "+5:30");
}

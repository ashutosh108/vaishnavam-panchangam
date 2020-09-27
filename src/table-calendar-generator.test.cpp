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
    REQUIRE(table.at(0, 3).text == "January 6");
    REQUIRE(table.at(0, 4).text == "January 7");
    REQUIRE(table.at(0, 5).text == "January 8");

    REQUIRE(table.height() >= 20);

    REQUIRE(table.at(1, 0).text == "+5:30");
    REQUIRE(table.at(1, 1).text == "India");
    REQUIRE(table.at(1, 2).text == "Udupi");
    REQUIRE(table.at(1, 3).text == "Śuddhā Ekādaśī");
    REQUIRE(table.at(1, 4).text == ">10:06");
    REQUIRE(table.at(1, 5).text == "");
}

TEST_CASE("Table_Caledar_Generator returns reasonable table adds ' (DST)' for 'summer' times") {
    using namespace date;
    const auto vratas = vp::text_ui::calc_all(2020_y/July/1);

    const auto table = vp::Table_Calendar_Generator::generate(vratas);
    int num_with_dst = 0;
    int num_without_dst = 0;
    for (int row=0; row < table.height(); ++row) {
        auto text = table.at(row, 0).text;
        if (text.find("DST") != std::string::npos) {
            ++num_with_dst;
        } else {
            ++num_without_dst;
        }
    }
    REQUIRE(num_with_dst > 0);
    REQUIRE(num_with_dst > 0);
}

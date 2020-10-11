#include "catch-formatters.h"

#include "table-calendar-generator.h"
#include "text-interface.h"

#include <sstream>

namespace {
auto some_vratas(date::year_month_day date = date::year{2020}/1/1) {
    return vp::text_ui::calc_all(date);
}

auto some_table(date::year_month_day date = date::year{2020}/1/1) {
    return vp::Table_Calendar_Generator::generate(some_vratas(date));
}
}

TEST_CASE("Table_Calendar_Generator returns reasonable table") {
    const auto table = some_table();
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

TEST_CASE("Table_Calendar_Generator returns reasonable table adds ' (DST)' for 'summer' times") {
    using namespace date;
    const auto table = some_table(2020_y/July/1);
    int num_with_dst = 0;
    int num_without_dst = 0;
    for (std::size_t row=0; row < table.height(); ++row) {
        auto text = table.at(row, 0).text;
        if (text.find("DST") != std::string::npos) {
            ++num_with_dst;
        } else {
            ++num_without_dst;
        }
    }
    REQUIRE(num_with_dst > 0);
    REQUIRE(num_without_dst > 0);
}

TEST_CASE("Table_Calendar_Generator generates 'shrIH'/'Om tatsat' with dates as the top and bottom rows") {
    const auto table = some_table();

    size_t rows = table.height();

    REQUIRE(rows >= 3);

    REQUIRE(table.at(0, 3).text == "January 6");
    REQUIRE(table.at(rows-1, 3).text == "January 6");
    REQUIRE(table.at(0, 4).text == "January 7");
    REQUIRE(table.at(rows-1, 4).text == "January 7");
    REQUIRE(table.at(0, 5).text == "January 8");
    REQUIRE(table.at(rows-1, 5).text == "January 8");

    using Catch::Matchers::Contains;
    REQUIRE_THAT(table.at(0, 0).text, Contains("श्रीः"));
    REQUIRE_THAT(table.at(rows-1, 0).text, Contains("ॐ तत्सत्"));
}

TEST_CASE("generated table has 'mainpart' as a class for top/bottom date headers") {
    const auto table = some_table();
    using Catch::Matchers::Contains;

    REQUIRE_THAT(table.at(0, 3).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(0, 4).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(0, 5).classes, Contains("mainpart"));

    size_t rows = table.height();
    REQUIRE(rows >= 3);

    REQUIRE_THAT(table.at(rows-1, 3).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(rows-1, 4).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(rows-1, 5).classes, Contains("mainpart"));
}

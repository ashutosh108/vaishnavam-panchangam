#include "catch.hpp"

#include <sstream>

#include "date-fixed.h"

using namespace date;

TEST_CASE("Date keeps year, month and day") {
    date::year_month_day d{date::year{2019}, date::month{3}, date::day{19}};
    REQUIRE(d.year() == 2019_y);
    REQUIRE(d.month() == March);
    REQUIRE(d.day() == date::day{19});
}

TEST_CASE("Printing date") {
    std::stringstream s;
    s << date::year_month_day{2019_y/March/19};
    REQUIRE(s.str() == "2019-03-19");
}

TEST_CASE("Can compare Date for equality") {
    date::year_month_day d1{2019_y/March/19};
    date::year_month_day d2{2019_y/March/19};
    date::year_month_day d3{2019_y/March/20};
    REQUIRE(d1 == d2);
    REQUIRE(d1 != d3);
}

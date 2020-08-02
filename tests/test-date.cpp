#include "date-fixed.h"

#include "catch-formatters.h"

using namespace date;

TEST_CASE("Date keeps year, month and day") {
    date::year_month_day d{date::year{2019}, date::month{3}, date::day{19}};
    REQUIRE(d.year() == 2019_y);
    REQUIRE(d.month() == March);
    REQUIRE(d.day() == date::day{19});
}

TEST_CASE("Printing date") {
    REQUIRE(fmt::to_string(date::year_month_day{2019_y/March/19}) == "2019-03-19");
}

TEST_CASE("Can compare Date for equality") {
    date::year_month_day d1{2019_y/March/19};
    date::year_month_day d2{2019_y/March/19};
    date::year_month_day d3{2019_y/March/20};
    REQUIRE(d1 == d2);
    REQUIRE(d1 != d3);
}

TEST_CASE("Can print year_month_day using fmt::to_string()") {
    const date::year_month_day d{20_y/March/6};
    auto s = fmt::to_string(d);
    REQUIRE(s == "0020-03-06");
}

TEST_CASE("Can print year_month_day using fmt::format()") {
    const date::year_month_day d{20_y/March/6};
    REQUIRE(fmt::format("{}", d) == "0020-03-06");
}

TEST_CASE("can print local_time with fmt") {
    using namespace std::chrono_literals;
    auto t = date::local_time<std::chrono::seconds>(date::local_days{20_y/June/7} + 5h);
    REQUIRE(fmt::to_string(t) == "0020-06-07 05:00:00.000000");
}

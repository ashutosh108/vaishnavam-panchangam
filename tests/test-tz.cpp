#include "tz-fixed.h"

#include <catch.hpp>

TEST_CASE("can format date::zoned_time with fmt") {
    using namespace date;
    using namespace std::chrono_literals;
    const auto l = date::local_time<std::chrono::seconds>(date::local_days{2020_y/June/7} + 1h + 2min + 3s);
    const auto timezone = date::locate_zone("Europe/Kiev");
    const auto t = date::make_zoned(timezone, l);
    REQUIRE(fmt::to_string(t) == "2020-06-07 01:02:03.000000 EEST");
}

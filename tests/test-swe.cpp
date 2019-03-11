#include "catch.hpp"

#include "swe.h"

TEST_CASE("Swe default constructor") {
    Swe s;
    REQUIRE(true);
}

TEST_CASE("get sunrise") {
    auto sunrise = Swe{}.get_sunrise(Swe_Time{2458552.5}, 50.45, 30.523333);
    REQUIRE(sunrise.year() == 2019);
    REQUIRE(sunrise.month() == 3);
    REQUIRE(sunrise.day() == 10);
    REQUIRE(sunrise.hours() == Approx(4.4816697389));
}

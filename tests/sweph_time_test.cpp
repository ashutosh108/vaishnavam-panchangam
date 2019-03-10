#include "catch.hpp"

#include <sstream>

#include "sweph_time.h"

TEST_CASE( "Can create Sweph_Time from double" ) {
    const double arbitrary_double = 124.0;
    Sweph_Time t{arbitrary_double};
}

TEST_CASE ("Pretty print Sweph_Time") {
    std::stringstream s;
    s << Sweph_Time{2458552.686736239120364};
    REQUIRE(s.str() == "2019-03-10 04:28:54.011060 UTC");
}

TEST_CASE ("as_julian_days") {
    Sweph_Time t{123.50};
    REQUIRE(t.as_julian_days() == 123.50);
}

TEST_CASE ("construct from Y, m, d") {
    std::stringstream s;
    s << Sweph_Time{2019, 3, 10};
    REQUIRE(s.str() == "2019-03-10 00:00:00.000000 UTC");
}

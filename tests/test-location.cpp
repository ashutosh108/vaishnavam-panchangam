#include "location.h"

#include "catch.hpp"

using namespace vp;

TEST_CASE("Location") {
    REQUIRE(Location{45.0_N, 123.0_E}.latitude == 45.0);
    REQUIRE(Location{45.0_N, 123.0_E}.longitude == 123.0);
}

TEST_CASE("Location degrees, minutes, seconds constructor") {
    Location l{10'30'36_N, 20'45'36_E, "Custom location"};
    REQUIRE(l.latitude == Approx(10.51)); // 10 + 30/60 + 36/3600
    REQUIRE(l.longitude == Approx(20.76)); // 20 + 45/60 + 36/3600
}

#include "catch.hpp"

#include "location.h"

TEST_CASE("Location") {
    REQUIRE(Location{123, 45}.latitude == 123.0);
    REQUIRE(Location{123, 45}.longitude == 45.0);
}

TEST_CASE("Location degrees, minutes, seconds constructor") {
    Location l{10, 30, 36, 20, 45, 36};
    REQUIRE(l.latitude == Approx(10.51)); // 10 + 30/60 + 36/3600
    REQUIRE(l.longitude == Approx(20.76)); // 20 + 45/60 + 36/3600
}

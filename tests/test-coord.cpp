#include "catch.hpp"

#include "coord.h"

TEST_CASE("Coord") {
    REQUIRE(Coord{123, 45}.latitude == 123.0);
    REQUIRE(Coord{123, 45}.longitude == 45.0);
}

TEST_CASE("Coord degrees, minutes, seconds constructor") {
    Coord c{10, 30, 36, 20, 45, 36};
    REQUIRE(c.latitude == Approx(10.51)); // 10 + 30/60 + 36/3600
    REQUIRE(c.longitude == Approx(20.76)); // 20 + 45/60 + 36/3600
}

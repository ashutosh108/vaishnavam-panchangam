#include "location.h"

#include "catch-formatters.h"
#include "fmt-format-fixed.h"

using namespace vp;

TEST_CASE("Location") {
    REQUIRE(Location{45.0_N, 123.0_E}.latitude.latitude == 45.0);
    REQUIRE(Location{45.0_N, 123.0_E}.longitude.longitude == 123.0);
}

void test_location_print(Latitude lat, Longitude lng, std::string_view expected) {
    REQUIRE(fmt::format(FMT_STRING("{}"), Location{lat, lng}) == expected);
}

TEST_CASE("Location prints coordinates as expected, with four decimal places") {
    test_location_print(12.9999_N, 23.9999_E,  "12.9999, 23.9999");
    test_location_print(12.99999_N, 23.99999_E,  "13.0000, 24.0000");
    test_location_print(90.0000_S, 180.0000_W, "-90.0000, -180.0000");
    test_location_print(0.0001_N, 0.0001_W,     "0.0001, -0.0001");
}

TEST_CASE("Latitude implements all six comparison operators") {
    Latitude lat1{10.0}, lat2{20.0};

    REQUIRE(lat1 < lat2);
    REQUIRE(!(lat2 < lat1));
    REQUIRE(!(lat1 < lat1));

    REQUIRE(lat1 <= lat2);
    REQUIRE(!(lat2 <= lat1));
    REQUIRE(lat1 <= lat1);

    REQUIRE(lat2 > lat1);
    REQUIRE(!(lat1 > lat2));
    REQUIRE(!(lat1 > lat1));

    REQUIRE(lat2 >= lat1);
    REQUIRE(!(lat1 >= lat2));
    REQUIRE(lat1 >= lat1);

    REQUIRE(!(lat1 == lat2));
    REQUIRE(lat1 == lat1);

    REQUIRE(lat1 != lat2);
    REQUIRE(!(lat1 != lat1));
}

TEST_CASE("Longitude implements all six comparison operators") {
    Longitude lng1{10.0}, lng2{20.0};

    REQUIRE(lng1 < lng2);
    REQUIRE(!(lng2 < lng1));
    REQUIRE(!(lng1 < lng1));

    REQUIRE(lng1 <= lng2);
    REQUIRE(!(lng2 <= lng1));
    REQUIRE(lng1 <= lng1);

    REQUIRE(lng2 > lng1);
    REQUIRE(!(lng1 > lng2));
    REQUIRE(!(lng1 > lng1));

    REQUIRE(lng2 >= lng1);
    REQUIRE(!(lng1 >= lng2));
    REQUIRE(lng1 >= lng1);

    REQUIRE(!(lng1 == lng2));
    REQUIRE(lng1 == lng1);

    REQUIRE(lng1 != lng2);
    REQUIRE(!(lng1 != lng1));
}

TEST_CASE("Coord implements all six comparison operators") {
    Coord c1{Latitude{10.0}, Longitude{20.0}}, c2{Latitude{10.0}, Longitude{30.0}};

    REQUIRE(c1 < c2);
    REQUIRE(!(c2 < c1));
    REQUIRE(!(c1 < c1));

    REQUIRE(c1 <= c2);
    REQUIRE(!(c2 <= c1));
    REQUIRE(c1 <= c1);

    REQUIRE(c2 > c1);
    REQUIRE(!(c1 > c2));
    REQUIRE(!(c1 > c1));

    REQUIRE(c2 >= c1);
    REQUIRE(!(c1 >= c2));
    REQUIRE(c1 >= c1);

    REQUIRE(!(c1 == c2));
    REQUIRE(c1 == c1);

    REQUIRE(c1 != c2);
    REQUIRE(!(c1 != c1));
}

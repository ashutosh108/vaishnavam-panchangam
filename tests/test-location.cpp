#include "location.h"

#include "catch.hpp"
#include "fmt-format-fixed.h"

using namespace vp;

TEST_CASE("Location") {
    REQUIRE(Location{45.0_N, 123.0_E}.latitude.latitude == 45.0);
    REQUIRE(Location{45.0_N, 123.0_E}.longitude.longitude == 123.0);
}

TEST_CASE("Location degrees, minutes, seconds constructor") {
    Location l{10'30'36_N, 20'45'36_E, "Custom location"};
    REQUIRE(l.latitude.latitude == Approx(10.51)); // 10 + 30/60 + 36/3600
    REQUIRE(l.longitude.longitude == Approx(20.76)); // 20 + 45/60 + 36/3600
}

void test_location_print(Latitude lat, Longitude lng, std::string_view expected) {
    REQUIRE(fmt::to_string(Location{lat, lng}) == expected);
}

TEST_CASE("Location prints coordinates nicely, with unicode etc") {
    test_location_print(12'34'56_N, 23'45'56_E,  "12°34′56″N, 23°45′56″E");
    test_location_print(90'00'00_S, 180'00'00_W, "90°00′00″S, 180°00′00″W");
    test_location_print(89'59'59_S, 179'59'59_W, "89°59′59″S, 179°59′59″W");
    test_location_print(0'00'01_N, 0'00'01_W,     "0°00′01″N, 0°00′01″W");
}

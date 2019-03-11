#include "catch.hpp"
#include <sstream>

#include "swe.h"

TEST_CASE("Swe default constructor") {
    Swe s;
    REQUIRE(true);
}

TEST_CASE("get sunrise") {
    auto sunrise = Swe{}.get_sunrise(Swe_Time{2019, 3, 10}, 50.45, 30.523333);
    REQUIRE(sunrise.has_value());
    REQUIRE(sunrise->year() == 2019);
    REQUIRE(sunrise->month() == 3);
    REQUIRE(sunrise->day() == 10);
    REQUIRE(sunrise->hours() == Approx(4.4816697389));
}

TEST_CASE("get sun longitude") {
    double sun_longitude = Swe{}.get_sun_longitude(Swe_Time{2019, 3, 10});
    REQUIRE(sun_longitude == Approx(349.1222311334));
}

TEST_CASE("get moon longitude") {
    double moon_longitude = Swe{}.get_moon_longitude(Swe_Time{2019, 3, 10});
    REQUIRE(moon_longitude == Approx(26.2874840949));
}

TEST_CASE("get tithi") {
    Swe_Time t2{2019, 3, 21, 1.716666}; // around 1:43am (UTC time), peak of purnima
    double tithi = Swe{}.get_tithi(t2);
    REQUIRE(tithi == Approx(15.0001492371));
}

// skip since find_tithi_start() is not implemented yet
TEST_CASE("find tithi start", "[.]") {
    Swe_Time start{2019, 3, 10};
    Swe_Time tithi_start = Swe{}.find_tithi_start(start, 11.0);
    Swe_Time expected{2019, 3, 17, 15.35};
    REQUIRE(tithi_start.as_julian_days() == Approx(expected.as_julian_days()).epsilon(0.00000001));
}

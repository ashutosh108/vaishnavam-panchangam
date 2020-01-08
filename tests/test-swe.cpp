#include "swe.h"

#include "catch.hpp"
#include "date-fixed.h"
#include <sstream>

using namespace date;
using namespace vp;

[[maybe_unused]] constexpr Location arbitrary_coord{50.0, 60.0, "UTC"};

TEST_CASE("Swe default constructor") {
    [[maybe_unused]] Swe s{arbitrary_coord};
    REQUIRE(true);
}

TEST_CASE("get sunrise") {
    Location c{50.45, 30.523333};
    auto sunrise = Swe{c}.get_sunrise(Swe_Time{2019_y, March, 10_d});
    REQUIRE(sunrise.has_value());
    REQUIRE(sunrise->as_date() == 2019_y/March/10);
    REQUIRE(sunrise->hours().count() == Approx(4.4816697389));
}

TEST_CASE("get_sunset") {
    Location c{50.45, 30.523333};
    auto sunset = Swe{c}.get_sunset(Swe_Time{2019_y, March, 10_d});
    REQUIRE(sunset.has_value());
    REQUIRE(*sunset == Swe_Time{2019_y, March, 10_d, 15, 48, 33.812600});
}

TEST_CASE("get sun longitude") {
    double sun_longitude = Swe{arbitrary_coord}.get_sun_longitude(Swe_Time{2019_y, March, 10_d});
    REQUIRE(sun_longitude == Approx(349.1222311334));
}

TEST_CASE("get moon longitude") {
    double moon_longitude = Swe{arbitrary_coord}.get_moon_longitude(Swe_Time{2019_y, March, 10_d});
    REQUIRE(moon_longitude == Approx(26.2874840949));
}

TEST_CASE("get tithi") {
    Swe_Time t2{2019_y, March, 21_d, double_hours{1.716666}}; // around 1:43am (UTC time), peak of purnima
    auto tithi = Swe{arbitrary_coord}.get_tithi(t2);
    REQUIRE(tithi.tithi == Approx(15.0001492371));
}

// skip since find_tithi_start() is not implemented yet
//TEST_CASE("find tithi start", "[.]") {
//    Swe_Time start{2019, 3, 10};
//    Swe_Time tithi_start = Swe{}.find_tithi_start(start, 11.0);
//    Swe_Time expected{2019, 3, 17, 15.35};
//    REQUIRE(tithi_start.as_julian_days() == Approx(expected.as_julian_days()).epsilon(0.00000001));
//}

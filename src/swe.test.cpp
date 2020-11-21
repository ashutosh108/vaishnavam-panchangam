#include "swe.h"

#include "catch-formatters.h"
#include <chrono>
#include "date-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

static const Location arbitrary_coord{50.0_N, 60.0_E};

TEST_CASE("Swe default constructor") {
    [[maybe_unused]] Swe s{arbitrary_coord};
    REQUIRE(true);
}

TEST_CASE("find_sunrise()") {
    Location c{50.45_N, 30.523333_E};
    auto sunrise = Swe{c}.find_sunrise(JulDays_UT{2019_y/March/10});
    REQUIRE(sunrise.has_value());
    REQUIRE(sunrise->year_month_day() == 2019_y/March/10);
    REQUIRE(sunrise->hours().count() == Approx(4.4816697389).margin(60./86400)); // 1 minute margin
}

TEST_CASE("find_sunrise_v() is callable") {
    Location c{50.45_N, 30.523333_E};
    auto sunrise = Swe{c}.find_sunrise_v(JulDays_UT{2019_y/March/10});
    REQUIRE(sunrise.year_month_day() == 2019_y/March/10);
    REQUIRE(sunrise.hours().count() == Approx(4.4816697389).margin(60./86400)); // 1 minute margin
}

TEST_CASE("find_sunrise_v() throws when no (imminent) sunrise found") {
    Location c{68'58'00_N, 33'05'00_E, "Murmansk"}; // fixed here to avoid test changes when we update coords
    REQUIRE_THROWS(Swe{c}.find_sunrise_v(JulDays_UT{2019_y/December/1}));
}

TEST_CASE("find_sunset") {
    Location c{50.45_N, 30.523333_E};
    auto sunset = Swe{c}.find_sunset(JulDays_UT{2019_y/March/10});
    REQUIRE(sunset.has_value());
    REQUIRE(sunset->round_to_minute_down() == date::sys_days(2019_y/March/10) + 15h + 48min /*+ 33.812600s*/);
}

TEST_CASE("find_sunset_v") {
    Location c{50.45_N, 30.523333_E};
    auto sunset = Swe{c}.find_sunset_v(JulDays_UT{2019_y/March/10});
    REQUIRE(sunset.round_to_minute_down() == date::sys_days(2019_y/March/10) + 15h + 48min /*+ 33.812600s*/);
}

TEST_CASE("find_sunset_v throws when no (imminent) sunset found") {
    Location c{68'58'00_N, 33'05'00_E, "Murmansk"}; // fixed here to avoid test changes when we update coords
    REQUIRE_THROWS(Swe{c}.find_sunset_v(JulDays_UT{2019_y/December/1}));
}


TEST_CASE("get sun longitude") {
    double sun_longitude = Swe{arbitrary_coord}.get_sun_longitude(JulDays_UT{2019_y/March/10});
    REQUIRE(sun_longitude == Approx(349.1222311334));
}

TEST_CASE("get moon longitude") {
    double moon_longitude = Swe{arbitrary_coord}.get_moon_longitude(JulDays_UT{2019_y/March/10});
    REQUIRE(moon_longitude == Approx(26.2874840949));
}

TEST_CASE("get tithi") {
    JulDays_UT t2{2019_y/March/21, double_hours{1.716666}}; // around 1:43am (UTC time), peak of purnima
    auto tithi = Swe{arbitrary_coord}.get_tithi(t2);
    REQUIRE(tithi.tithi == Approx(15.0001492371));
}

// skip since find_tithi_start() is not implemented yet
//TEST_CASE("find tithi start", "[.]") {
//    JulDays_UT start{2019, 3, 10};
//    JulDays_UT tithi_start = Swe{}.find_tithi_start(start, 11.0);
//    JulDays_UT expected{2019, 3, 17, 15.35};
//    REQUIRE(tithi_start.as_julian_days() == Approx(expected.as_julian_days()).epsilon(0.00000001));
//}

TEST_CASE("swe gives different sunrises for edge and center of sun disc") {
    JulDays_UT after{2019_y/March/10};
    Location loc{23.0_N, 45.0_E};
    auto sunrise_center = [&]() {
        return vp::Swe{loc}.find_sunrise_v(after);
    }();
    auto sunrise_edge = [&]() {
        return vp::Swe{loc, vp::CalcFlags::SunriseByDiscEdge}.find_sunrise_v(after);
    }();
    REQUIRE(sunrise_edge < sunrise_center);
    auto diff = date::floor<std::chrono::seconds>(sunrise_center - sunrise_edge);
    REQUIRE(diff < 10min);
    REQUIRE(diff >= 1min);
}

TEST_CASE("sunrise with refraction off comes 3-5 minutes before actual observed sunrise with refraction on") {
    const JulDays_UT after{2020_y/January/1};
    const auto sunrise_with_refraction_off = [&]() {
        return vp::Swe{arbitrary_coord, vp::CalcFlags::RefractionOff}.find_sunrise_v(after);
    }();
    const auto sunrise_with_refraction_on = [&]() {
        return vp::Swe{arbitrary_coord, vp::CalcFlags::RefractionOn}.find_sunrise_v(after);
    }();
    auto diff = sunrise_with_refraction_off - sunrise_with_refraction_on;
    REQUIRE(diff >= 3min);
    REQUIRE(diff < 5min);
}

TEST_CASE("get_moon_longitude_sidereal works for known case (Rohini end on 2020-11-03 is 2:30am next day in India)") {
    Location udupi{28'39'00_N,  77'13'00_E};
    constexpr auto timezone_offset = 5h+30min; // IST
    // bracket is [t-30sec..t+30sec]
    JulDays_UT t1{2020_y/November/4, 2h + 30min - timezone_offset - 30s};
    JulDays_UT t2{t1 + 1min};
    // 2020-11-03
    // Sunrise: ~06:26
    // Rohini (4-of-27) until 50-08 26-30
    const auto actual1 = vp::Swe{udupi}.get_moon_longitude_sidereal(t1);
    const auto actual2 = vp::Swe{udupi}.get_moon_longitude_sidereal(t2);

    const vp::Nirayana_Longitude expected{4.0 * (360.0 / 27.0)};

    CAPTURE(t1);
    CAPTURE(t2);
    CAPTURE(actual1.longitude);
    CAPTURE(actual2.longitude);
    CAPTURE(expected.longitude);
    CHECK(actual1.longitude <= expected.longitude);
    REQUIRE(actual2.longitude >= expected.longitude);
}

TEST_CASE("surya_nirayana_longitude() works for known date of Mesha sankranti") {
    // mesha sankranti (corresponding to 0 degrees surya nirayana longitude) generally occurs on April 13th or 14th,
    // so expect it to be > 13th UTC midnight but < 15th UTC midnight
    JulDays_UT earliest{2020_y/April/13};
    JulDays_UT latest{2020_y/April/15};
    Swe swe{arbitrary_coord};

    Nirayana_Longitude l1 = swe.surya_nirayana_longitude(earliest);
    Nirayana_Longitude l2 = swe.surya_nirayana_longitude(latest);

    CAPTURE(l1.longitude);
    CAPTURE(l2.longitude);
    REQUIRE(l1.longitude > 357.0);
    REQUIRE(l2.longitude < 3.0);
}

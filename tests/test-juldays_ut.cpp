#include "juldays_ut.h"

#include "disable-warning.h"

#include "catch.hpp"
#include <chrono>
#include <sstream>
#include "tz-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

TEST_CASE( "Can create JulDays_UT from double_days" ) {
    const double_days arbitrary_double_days{124.0};
    JulDays_UT t{arbitrary_double_days};
    REQUIRE(true);
}

TEST_CASE ("Pretty print JulDays_UT") {
    std::stringstream s;
    s << JulDays_UT{double_days{2458552.686736239120364}};
    REQUIRE(s.str() == "2019-03-10 04:28:54.011060 UTC");
}

TEST_CASE ("raw_julian_days_ut") {
    JulDays_UT t{double_days{123.50}};
    REQUIRE(t.raw_julian_days_ut() == double_days{123.50});
}

TEST_CASE ("construct from Y, m, d") {
    std::stringstream s;
    s << JulDays_UT{2019_y/March/10};
    REQUIRE(s.str() == "2019-03-10 00:00:00.000000 UTC");
}

TEST_CASE ("construct from Y, m, d, hours") {
    std::stringstream s;
    s << JulDays_UT{2019_y/March/10, double_hours{5.25}};
    REQUIRE(s.str() == "2019-03-10 05:15:00.000000 UTC");
}

TEST_CASE("hours matter") {
    JulDays_UT t1{2019_y/March/10, double_hours{0.0}};
    JulDays_UT t2{2019_y/March/10, double_hours{6.0}};
    double_days diff = t2 - t1;
    REQUIRE(diff == double_days{0.25});
}

TEST_CASE("can compare") {
    JulDays_UT t1{2019_y/March/10_d};
    JulDays_UT t2{double_days{2458552.5}};
    REQUIRE(t1 == t2);
}

TEST_CASE("create from Y M D h m s", "[hms]") {
    JulDays_UT t1{2019_y/March/17, double_hours{4.2267416752777778}};
    JulDays_UT t2{2019_y/March/17, 4h + 13min + 36.270031s};
    REQUIRE(t1 == t2);
}

TEST_CASE("Can create from Date") {
    date::year_month_day d{2019_y/March/9};
    JulDays_UT t{d};
    REQUIRE(t == JulDays_UT{2019_y/March/9});
}

TEST_CASE("Can add double to JulDays_UT") {
    // make sure that first argument to + can be const
    JulDays_UT const t1{2019_y/March/17};
    REQUIRE(t1+double_days{1.0} == JulDays_UT{2019_y/March/18});
}

TEST_CASE("+= returns correct value") {
    REQUIRE((JulDays_UT{2019_y/March/17}+=double_days{1.0}) == JulDays_UT{2019_y/March/18});
}

TEST_CASE("+= modifies JulDays_UT") {
    JulDays_UT t{2019_y/March/17};
    t += double_days{1.0};
    REQUIRE(t == JulDays_UT{2019_y/March/18});
}

TEST_CASE("Substracting double from JulDays_UT means substracting days") {
    JulDays_UT const t1{2019_y/March/17};
    REQUIRE(t1-double_days{1.0} == JulDays_UT{2019_y/March/16});
}

TEST_CASE("-= returns correct value") {
    REQUIRE((JulDays_UT{2019_y/March/17}-=double_days{1.0}) == JulDays_UT{2019_y/March/16});
}

TEST_CASE("-= modifies JulDays_UT") {
    JulDays_UT t{2019_y/March/17};
    t -= double_days{1.0};
    REQUIRE(t == JulDays_UT{2019_y/March/16});
}

TEST_CASE("Can compare <") {
    REQUIRE(JulDays_UT{2019_y/March/17} < JulDays_UT{2019_y/March/18});
}

TEST_CASE("Can compare >") {
    REQUIRE(JulDays_UT{2019_y/March/18} > JulDays_UT{2019_y/March/17});
}

TEST_CASE("Can substract JulDays_UT from JulDays_UT") {
    REQUIRE((JulDays_UT{2019_y/April/16} - JulDays_UT{2019_y/April/10}) == double_days{6.0});
}

TEST_CASE("Pretty-print zoned time") {
    std::stringstream s;
    s << Swe_Zoned_Time{"Europe/Kiev", 2019_y/March/10, double_hours{4.5}};
    REQUIRE(s.str() == "2019-03-10 06:30:00.000000 EET");
}

#include "swe_time.h"

#include "catch.hpp"
#include <sstream>
#include "tz-fixed.h"

using namespace date;
using namespace vp;

TEST_CASE( "Can create Swe_Time from double_days" ) {
    const double_days arbitrary_double_days{124.0};
    Swe_Time t{arbitrary_double_days};
    REQUIRE(true);
}

TEST_CASE ("Pretty print Swe_Time") {
    std::stringstream s;
    s << Swe_Time{double_days{2458552.686736239120364}};
    REQUIRE(s.str() == "2019-03-10 04:28:54.011060 UTC");
}

TEST_CASE ("as_julian_days") {
    Swe_Time t{double_days{123.50}};
    REQUIRE(t.as_julian_days() == double_days{123.50});
}

TEST_CASE ("construct from Y, m, d") {
    std::stringstream s;
    s << Swe_Time{2019_y, March, 10_d};
    REQUIRE(s.str() == "2019-03-10 00:00:00.000000 UTC");
}

TEST_CASE ("construct from Y, m, d, hours") {
    std::stringstream s;
    s << Swe_Time{2019_y, March, 10_d, double_hours{5.25}};
    REQUIRE(s.str() == "2019-03-10 05:15:00.000000 UTC");
}

TEST_CASE("hours matter") {
    Swe_Time t1{2019_y, March, 10_d, double_hours{0.0}};
    Swe_Time t2{2019_y, March, 10_d, double_hours{6.0}};
    double_days diff = t2.as_julian_days() - t1.as_julian_days();
    REQUIRE(diff == double_days{0.25});
}

TEST_CASE("can compare") {
    Swe_Time t1{2019_y, March, 10_d};
    Swe_Time t2{double_days{2458552.5}};
    REQUIRE(t1 == t2);
}

TEST_CASE("create from Y M D h m s", "[hms]") {
    Swe_Time t1{2019_y, March, 17_d, double_hours{4.2267416752777778}};
    Swe_Time t2{2019_y, March, 17_d, 4, 13, 36.270031};
    REQUIRE(t1 == t2);
}

TEST_CASE("Can create from Date") {
    date::year_month_day d{2019_y/March/9};
    Swe_Time t{d};
    REQUIRE(t == Swe_Time{2019_y, March, 9_d});
}

TEST_CASE("Can add double to Swe_Time") {
    // make sure that first argument to + can be const
    Swe_Time const t1{2019_y, March, 17_d};
    REQUIRE(t1+double_days{1.0} == Swe_Time{2019_y, March, 18_d});
}

TEST_CASE("+= returns correct value") {
    REQUIRE((Swe_Time{2019_y, March, 17_d}+=double_days{1.0}) == Swe_Time{2019_y, March, 18_d});
}

TEST_CASE("+= modifies Swe_Time") {
    Swe_Time t{2019_y, March, 17_d};
    t += double_days{1.0};
    REQUIRE(t == Swe_Time{2019_y, March, 18_d});
}

TEST_CASE("Substracting double from Swe_Time means substracting days") {
    Swe_Time const t1{2019_y, March, 17_d};
    REQUIRE(t1-double_days{1.0} == Swe_Time{2019_y, March, 16_d});
}

TEST_CASE("-= returns correct value") {
    REQUIRE((Swe_Time{2019_y, March, 17_d}-=double_days{1.0}) == Swe_Time{2019_y, March, 16_d});
}

TEST_CASE("-= modifies Swe_Time") {
    Swe_Time t{2019_y, March, 17_d};
    t -= double_days{1.0};
    REQUIRE(t == Swe_Time{2019_y, March, 16_d});
}

TEST_CASE("Can compare <") {
    REQUIRE(Swe_Time{2019_y, March, 17_d} < Swe_Time{2019_y, March, 18_d});
}

TEST_CASE("Can compare >") {
    REQUIRE(Swe_Time{2019_y, March, 18_d} > Swe_Time{2019_y, March, 17_d});
}

TEST_CASE("Can substract Swe_Time from Swe_Time") {
    REQUIRE((Swe_Time{2019_y, April, 16_d} - Swe_Time{2019_y, April, 10_d}) == double_days{6.0});
}

TEST_CASE("Pretty-print zoned time") {
    std::stringstream s;
    s << Swe_Zoned_Time{"Europe/Kiev", 2019_y/March/10, double_hours{4.5}};
    REQUIRE(s.str() == "2019-03-10 06:30:00.000000 EET");
}

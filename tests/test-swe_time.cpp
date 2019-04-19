#include "catch.hpp"

#include <sstream>

#include "date.h"
#include "swe_time.h"

TEST_CASE( "Can create Swe_Time from double" ) {
    const double arbitrary_double = 124.0;
    Swe_Time t{arbitrary_double};
    REQUIRE(true);
}

TEST_CASE ("Pretty print Swe_Time") {
    std::stringstream s;
    s << Swe_Time{2458552.686736239120364};
    REQUIRE(s.str() == "2019-03-10 04:28:54.011060 UTC");
}

TEST_CASE ("as_julian_days") {
    Swe_Time t{123.50};
    REQUIRE(t.as_julian_days() == 123.50);
}

TEST_CASE ("construct from Y, m, d") {
    std::stringstream s;
    s << Swe_Time{2019, 3, 10};
    REQUIRE(s.str() == "2019-03-10 00:00:00.000000 UTC");
}

TEST_CASE ("construct from Y, m, d, hours") {
    std::stringstream s;
    s << Swe_Time{2019, 3, 10, 5.25};
    REQUIRE(s.str() == "2019-03-10 05:15:00.000000 UTC");
}

TEST_CASE("hours matter") {
    Swe_Time t1{2019, 3, 10, 0.0};
    Swe_Time t2{2019, 3, 10, 6.0};
    double diff = t2.as_julian_days() - t1.as_julian_days();
    REQUIRE(diff == 0.25);
}

TEST_CASE("can compare") {
    Swe_Time t1{2019, 3, 10};
    Swe_Time t2{2458552.5};
    REQUIRE(t1 == t2);
}

TEST_CASE("create from Y M D h m s", "[hms]") {
    Swe_Time t1{2019, 3, 17, 4.2267416752777778};
    Swe_Time t2{2019, 3, 17, 4, 13, 36.270031};
    REQUIRE(t1 == t2);
}

TEST_CASE("Can create from Date") {
    Date d{2019, 3, 9};
    Swe_Time t{d};
    REQUIRE(t == Swe_Time{2019, 3, 9});
}

TEST_CASE("Can add double to Swe_Time") {
    // make sure that first argument to + can be const
    Swe_Time const t1{2019, 3, 17};
    REQUIRE(t1+1.0 == Swe_Time{2019, 3, 18});
}

TEST_CASE("+= returns correct value") {
    REQUIRE((Swe_Time{2019, 3, 17}+=1.0) == Swe_Time{2019, 3, 18});
}

TEST_CASE("+= modifies Swe_Time") {
    Swe_Time t{2019, 3, 17};
    t += 1.0;
    REQUIRE(t == Swe_Time{2019, 3, 18});
}

TEST_CASE("Substracting double from Swe_Time means substracting days") {
    Swe_Time const t1{2019, 3, 17};
    REQUIRE(t1-1.0 == Swe_Time{2019, 3, 16});
}

TEST_CASE("-= returns correct value") {
    REQUIRE((Swe_Time{2019, 3, 17}-=1.0) == Swe_Time{2019, 3, 16});
}

TEST_CASE("-= modifies Swe_Time") {
    Swe_Time t{2019, 3, 17};
    t -= 1.0;
    REQUIRE(t == Swe_Time{2019, 3, 16});
}

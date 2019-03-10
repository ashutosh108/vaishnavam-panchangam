#include "catch.hpp"

#include <sstream>

#include "sweph_time.h"

TEST_CASE( "Can create Sweph_Time from double" ) {
    const double arbitrary_double = 124.0;
    Sweph_Time t{arbitrary_double};
}

TEST_CASE ("Pretty print Sweph_Time") {
    std::stringstream s;
    s << Sweph_Time{2458552.686736239120364};
    REQUIRE(s.str() == "2019-03-10 04:28:54.0111");
}

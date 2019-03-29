#include "catch.hpp"

#include <sstream>

#include "coord.h"
#include "vrata.h"
#include "vrata_detail.h"

TEST_CASE("Print result is not empty") {
    std::stringstream s;
    Vrata v{Date{2019, 3, 1}};
    Coord c{50.0, 50.0};
    Vrata_Detail vd{v, c};
    s << vd;
    REQUIRE(s.str() != "");
}

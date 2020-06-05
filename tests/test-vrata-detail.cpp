#include "date-fixed.h"
#include "location.h"
#include "vrata.h"
#include "vrata_detail.h"

#include "catch.hpp"
#include <sstream>


using namespace date;
using namespace vp;

TEST_CASE("Print result is not empty") {
    std::stringstream s;
    Vrata v{2019_y/March/1, dummy_ativrddhatvam};
    Location c{50.0, 50.0};
    Vrata_Detail vd{v, c};
    s << vd;
    REQUIRE(s.str() != "");
}

TEST_CASE("we get nearest next ekadashi start for petropavlovsk 2019-03-18") {
    Vrata v{2019_y/March/18, dummy_ativrddhatvam};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    JulDays_UT ekadashi_earliest{2019_y/March/16};
    JulDays_UT ekadashi_latest{2019_y/March/17};
    REQUIRE(vd.ekadashi_start > ekadashi_earliest);
    REQUIRE(vd.ekadashi_start < ekadashi_latest);
}

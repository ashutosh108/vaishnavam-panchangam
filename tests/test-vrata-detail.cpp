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
    Vrata v{2019_y/March/1};
    Location c{50.0, 50.0};
    Vrata_Detail vd{v, c};
    s << vd;
    REQUIRE(s.str() != "");
}

TEST_CASE("get_approx_local_midnight() adjusts to the right side") {
    Vrata v{2019_y/March/18};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    auto local_midnight = vd.get_approx_local_midnight();
    JulDays_UT local_midnight_in_utc_earliest{2019_y, March, 17_d};
    JulDays_UT local_midnight_in_utc_latest{2019_y, March, 18_d};
    REQUIRE(local_midnight > local_midnight_in_utc_earliest);
    REQUIRE(local_midnight < local_midnight_in_utc_latest);
}

TEST_CASE("we get nearest next ekadashi start for petropavlovsk 2019-03-18") {
    Vrata v{2019_y/March/18};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    JulDays_UT ekadashi_earliest{2019_y, March, 16_d};
    JulDays_UT ekadashi_latest{2019_y, March, 17_d};
    REQUIRE(*vd.ekadashi_start > ekadashi_earliest);
    REQUIRE(*vd.ekadashi_start < ekadashi_latest);
}

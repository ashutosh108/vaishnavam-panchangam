#include "catch.hpp"

#include <sstream>

#include "date/date.h"
#include "location.h"
#include "vrata.h"
#include "vrata_detail.h"

using namespace date;

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
    Swe_Time local_midnight_in_utc_earliest{2019_y, March, 17_d};
    Swe_Time local_midnight_in_utc_latest{2019_y, March, 18_d};
    REQUIRE(local_midnight > local_midnight_in_utc_earliest);
    REQUIRE(local_midnight < local_midnight_in_utc_latest);
}

TEST_CASE("we get nearest next ekadashi start for petropavlovsk 2019-03-18") {
    Vrata v{2019_y/March/18};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    Swe_Time ekadashi_earliest{2019_y, March, 16_d};
    Swe_Time ekadashi_latest{2019_y, March, 17_d};
    REQUIRE(*vd.ekadashi_start > ekadashi_earliest);
    REQUIRE(*vd.ekadashi_start < ekadashi_latest);
}

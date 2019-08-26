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

TEST_CASE("get_approx_local_midnight() adjusts to the right side") {
    Vrata v{Date{2019, 3, 18}};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    auto local_midnight = vd.get_approx_local_midnight();
    Swe_Time local_midnight_in_utc_earliest{2019, 3, 17};
    Swe_Time local_midnight_in_utc_latest{2019, 3, 18};
    REQUIRE(local_midnight > local_midnight_in_utc_earliest);
    REQUIRE(local_midnight < local_midnight_in_utc_latest);
}

TEST_CASE("we get nearest next ekadashi start for petropavlovsk 2019-03-18") {
    Vrata v{Date{2019, 3, 18}};
    Vrata_Detail vd{v, petropavlovskkamchatskiy_coord};
    Swe_Time ekadashi_earliest{2019, 3, 16};
    Swe_Time ekadashi_latest{2019, 3, 17};
    REQUIRE(*vd.ekadashi_start > ekadashi_earliest);
    REQUIRE(*vd.ekadashi_start < ekadashi_latest);
}

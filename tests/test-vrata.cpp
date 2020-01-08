#include "vrata.h"

#include "catch.hpp"
#include "date-fixed.h"
#include <sstream>

using namespace date;
using namespace vp;

TEST_CASE("Can compare Vrata") {
    Vrata v1{2019_y/March/19};
    Vrata v2{2019_y/March/19};
    Vrata v3{2019_y/March/20};
    REQUIRE(v1 == v2);
    REQUIRE(v2 == v1);
    REQUIRE(v1 != v3);
    REQUIRE(v3 != v1);
}

TEST_CASE("Can print vrata") {
    std::stringstream s;
    s << Vrata{2019_y/March/19};
    REQUIRE_FALSE(s.str().empty());
}

TEST_CASE("Can create with \"before this time\" paran") {
    Vrata v{Vrata_Type::Ekadashi,
            2019_y/March/19,
            Paran{Paran::Type::Standard, std::nullopt, Swe_Time{2019_y, March, 19_d, 11, 13, 0}}};
}

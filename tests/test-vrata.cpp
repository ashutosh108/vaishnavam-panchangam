#include "catch.hpp"

#include <sstream>

#include "date.h"
#include "vrata.h"

TEST_CASE("Can compare Vrata") {
    Vrata v1{Date{2019, 3, 19}};
    Vrata v2{Date{2019, 3, 19}};
    Vrata v3{Date{2019, 3, 20}};
    REQUIRE(v1 == v2);
    REQUIRE(v2 == v1);
    REQUIRE(v1 != v3);
    REQUIRE(v3 != v1);
}

TEST_CASE("Can print vrata") {
    std::stringstream s;
    s << Vrata{Date{2019, 3, 19}};
    REQUIRE(s.str() == "Ekadashi{2019-03-19}");
}

TEST_CASE("Can create with \"before this time\" paran") {
    Vrata v{Vrata_Type::Ekadashi,
            Date{2019, 3, 19},
            Paran{std::nullopt, Swe_Time{2019, 3, 19, 11, 13, 0}}};
}

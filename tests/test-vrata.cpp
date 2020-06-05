#include "vrata.h"

#include "catch.hpp"
#include "date-fixed.h"
#include <sstream>

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

TEST_CASE("Can compare Vrata") {
    Vrata v1{2019_y/March/19, dummy_ativrddhatvam};
    Vrata v2{2019_y/March/19, dummy_ativrddhatvam};
    Vrata v3{2019_y/March/20, dummy_ativrddhatvam};
    REQUIRE(v1 == v2);
    REQUIRE(v2 == v1);
    REQUIRE(v1 != v3);
    REQUIRE(v3 != v1);
}

TEST_CASE("Can print vrata") {
    std::stringstream s;
    s << Vrata{2019_y/March/19, dummy_ativrddhatvam};
    REQUIRE_FALSE(s.str().empty());
}

TEST_CASE("Can create with \"before this time\" paran") {
    Vrata v{Vrata_Type::Ekadashi,
            2019_y/March/19,
            Paran{Paran::Type::Standard, std::nullopt, JulDays_UT{2019_y/March/19, 11h + 13min}},
            dummy_ativrddhatvam};
}

TEST_CASE("ekadashi_name_rus_is_valid") {
    REQUIRE(ekadashi_name_rus_is_valid("Шат̣-тилā"));
    REQUIRE(ekadashi_name_rus_is_valid("Шат̣тилā"));
}

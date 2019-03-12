#include "catch.hpp"

#include "swe_time.h"
#include "calc.h"

TEST_CASE("find_next_ekadashi_sunrise") {
    Swe_Time start{2019, 3, 9};
    auto sunrise = Calc{}.find_next_ekadashi_sunrise(start, 50.45, 30.523333);
    Swe_Time expected{2019, 3, 17, 4, 13, 36.270031};
    REQUIRE(sunrise.has_value());
    REQUIRE(*sunrise == expected);
}

// disabled until we learn to skip dashami-viddha ekadashi(?)
TEST_CASE("Vijaya Ekadashi Kiev 2019", "[.]") {
    Swe_Time start{2019, 2, 28};
    auto vrata = Calc{}.find_next_vrata(start, 50.45, 30.523333);
    Vrata expected_vrata{Paksha::Krishna, Vrata_Type::Shuddha_Ekadashi, Swe_Time{2019, 3, 2, 4.05}};
    REQUIRE(vrata.has_value());
    REQUIRE(vrata->paksha == expected_vrata.paksha);
    REQUIRE(vrata->type == expected_vrata.type);
    REQUIRE(vrata->sunrise == expected_vrata.sunrise);
}

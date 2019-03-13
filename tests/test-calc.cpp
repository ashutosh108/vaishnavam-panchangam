#include <sstream>

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
TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    Swe_Time start{2019, 2, 28};
    auto vrata = Calc{}.find_next_vrata(start, 50.45, 30.523333);
    REQUIRE(vrata.has_value());
    REQUIRE(vrata->tithi_sunrise.get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Shuddha_Ekadashi);
    REQUIRE(vrata->sunrise.year() == 2019);
    REQUIRE(vrata->sunrise.month() == 3);
    REQUIRE(vrata->sunrise.day() == 2);
}

TEST_CASE("get_arunodaya") {
    auto arunodaya = Calc{}.get_arunodaya(Swe_Time{2019, 3, 2, 4, 45, 58.052015}, 50.45, 30.523333);
    REQUIRE(arunodaya.has_value());
    REQUIRE(*arunodaya == Swe_Time{2019, 3, 2, 3, 0, 17.512880});
}

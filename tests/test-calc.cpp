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

TEST_CASE("Tithi pretty-printing: Shukla Dvadashi") {
    std::stringstream s;
    s << Tithi{11.3};
    REQUIRE(s.str() == "Shukla Dvadashi(0.3)");
}

TEST_CASE("Tithi pretty-printing: Krishna Ekadashi") {
    std::stringstream s;
    s << Tithi{15+10.4};
    REQUIRE(s.str() == "Krishna Ekadashi(0.4)");
}

TEST_CASE("Tithi pretty-printing: Purnima") {
    std::stringstream s;
    s << Tithi{14.7};
    REQUIRE(s.str() == "Purnima(0.7)");
}

TEST_CASE("Tithi pretty-printing: Amavasya") {
    std::stringstream s;
    s << Tithi{15+14.8};
    REQUIRE(s.str() == "Amavasya(0.8)");
}

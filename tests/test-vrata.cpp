#include "vrata.h"

#include "catch-formatters.h"
#include "date-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;
using Catch::Matchers::Contains;

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
    REQUIRE(!fmt::to_string(Vrata{2019_y/March/19}).empty());
}

TEST_CASE("Can create with \"before this time\" paran") {
    Vrata v{Vrata_Type::Ekadashi,
            2019_y/March/19,
            Paran{Paran::Type::Standard, std::nullopt, JulDays_UT{2019_y/March/19, 11h + 13min}}};
}

TEST_CASE("ekadashi_name_rus_is_valid") {
    REQUIRE(ekadashi_name_rus_is_valid("Шат̣-тилā"));
    REQUIRE(ekadashi_name_rus_is_valid("Шат̣тилā"));
}

TEST_CASE("atirikta ekAdashI is written as 'with Atiriktā Ekādaśī'") {
    REQUIRE_THAT(fmt::to_string(Vrata_Type::With_Atirikta_Ekadashi), Contains("with Atiriktā Ekādaśī"));
}

TEST_CASE("atirikta dvAdashI is written from capital 'A'") {
    REQUIRE_THAT(fmt::to_string(Vrata_Type::With_Atirikta_Dvadashi), Contains("with Atiriktā Dvādaśī"));
}

TEST_CASE("Śuddhā Ekādaśī is written as initial-caps") {
    REQUIRE_THAT(fmt::to_string(Vrata_Type::Ekadashi), Contains("Śuddhā Ekādaśī"));
}

#include "vrata.h"

#include "catch-formatters.h"
#include "date-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;
using Catch::Matchers::Contains;

TEST_CASE("Can compare Vrata") {
    Vrata v1{2019_y/March/19, Chandra_Masa::Phalguna, Paksha::Shukla};
    Vrata v2{2019_y/March/19, Chandra_Masa::Phalguna, Paksha::Shukla};
    Vrata v3{2019_y/March/20, Chandra_Masa::Phalguna, Paksha::Shukla};
    REQUIRE(v1 == v2);
    REQUIRE(v2 == v1);
    REQUIRE(v1 != v3);
    REQUIRE(v3 != v1);
}

TEST_CASE("Can print vrata") {
    REQUIRE(!fmt::to_string(Vrata{2019_y/March/19, Chandra_Masa::Phalguna, Paksha::Shukla}).empty());
}

TEST_CASE("Can create with \"before this time\" paran") {
    Vrata v{Vrata_Type::Ekadashi,
            2019_y/March/19,
            Chandra_Masa::Phalguna,
            Paksha::Shukla,
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

TEST_CASE("Ekādaśī is written as initial-caps") {
    REQUIRE_THAT(fmt::to_string(Vrata_Type::Ekadashi), Contains("Ekādaśī"));
}

TEST_CASE("MaybeVrata and VratasForDate types (or aliases) do exist") {
    [[maybe_unused]] vp::MaybeVrata vrata;
    [[maybe_unused]] vp::VratasForDate vratas;
}

TEST_CASE("is_atirikta() works for all types") {
    REQUIRE(!vp::is_atirikta(vp::Vrata_Type::Ekadashi));
    REQUIRE(vp::is_atirikta(vp::Vrata_Type::With_Atirikta_Ekadashi));
    REQUIRE(vp::is_atirikta(vp::Vrata_Type::With_Atirikta_Dvadashi));
    REQUIRE(vp::is_atirikta(vp::Vrata_Type::With_Shravana_Dvadashi_Next_Day));
}

TEST_CASE("Vrata_Type formatting") {
    REQUIRE(fmt::to_string(vp::Vrata_Type::Ekadashi) == "Ekādaśī");
    REQUIRE(fmt::to_string(vp::Vrata_Type::With_Atirikta_Ekadashi) == "Ekādaśī with Atiriktā Ekādaśī (two days fast)");
    REQUIRE(fmt::to_string(vp::Vrata_Type::With_Atirikta_Dvadashi) == "Ekādaśī with Atiriktā Dvādaśī (two days fast)");
    REQUIRE(fmt::to_string(vp::Vrata_Type::With_Shravana_Dvadashi_Next_Day) == "Ekādaśī with next-day Śravaṇa-dvādaśī (two days fast)");
}

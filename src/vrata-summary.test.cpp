#include "catch-formatters.h"

#include "vrata-summary.h"

TEST_CASE("summary contains location name") {
    auto vrata = vp::Vrata::SampleVrata();
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("Sample location"));
}

TEST_CASE("summary contains vrata type, date and paran (including optional paran limit)") {
    using namespace date;
    using namespace std::chrono_literals;
    auto vrata = vp::Vrata::SampleVrata();
    vrata.paran.paran_start = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 7h};
    vrata.paran.paran_end = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 8h};
    vrata.paran.paran_limit = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 9h};
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("Ekādaśī"));
    REQUIRE_THAT(actual, Contains("2000-01-01"));
    REQUIRE_THAT(actual, Contains("07:00"));
    REQUIRE_THAT(actual, Contains("08:00"));
    REQUIRE_THAT(actual, Contains("&lt;09:00"));
    REQUIRE_THAT(actual, !Contains("Harivāsara"));
}

TEST_CASE("summary for Atiriktā contains both dates") {
    auto vrata = vp::Vrata::SampleVrata();
    vrata.type = vp::Vrata_Type::With_Atirikta_Ekadashi;
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("2000-01-01"));
    REQUIRE_THAT(actual, !Contains("2000-01-01 0")); // must not print date as YYYY-mm-dd HH:MM:SS.SSSSSS
    REQUIRE_THAT(actual, Contains("2000-01-02"));
    REQUIRE_THAT(actual, !Contains("2000-01-02 0")); // must not print date as YYYY-mm-dd HH:MM:SS.SSSSSS
}

TEST_CASE("summary contains ekādaśī name, māsa name and pakṣa") {
    auto vrata = vp::Vrata::SampleVrata();
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("Mārgaśīrṣa"));
    REQUIRE_THAT(actual, Contains("kṛṣṇa pakṣa"));
    REQUIRE_THAT(actual, Contains("Saphalā"));
}

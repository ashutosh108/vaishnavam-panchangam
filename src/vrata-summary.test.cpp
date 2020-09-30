#include "catch-formatters.h"

#include "vrata-summary.h"

TEST_CASE("summary contains location name") {
    auto vrata = vp::Vrata::SampleVrata();
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("Sample location"));
}

TEST_CASE("summary contains vrata type, date and paran") {
    using namespace date;
    using namespace std::chrono_literals;
    auto vrata = vp::Vrata::SampleVrata();
    vrata.paran.paran_start = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 7h};
    vrata.paran.paran_end = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 8h};
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("Śuddhā Ekādaśī"));
    REQUIRE_THAT(actual, Contains("2000-01-01"));
    REQUIRE_THAT(actual, Contains("07:00"));
    REQUIRE_THAT(actual, Contains("08:00"));
}

TEST_CASE("summary for Atiriktā contains both dates") {
    auto vrata = vp::Vrata::SampleVrata();
    vrata.type = vp::Vrata_Type::With_Atirikta_Ekadashi;
    auto actual = fmt::format("{}", vp::Vrata_Summary{&vrata});
    using Catch::Matchers::Contains;
    REQUIRE_THAT(actual, Contains("2000-01-01"));
    REQUIRE_THAT(actual, Contains("2000-01-02"));
}

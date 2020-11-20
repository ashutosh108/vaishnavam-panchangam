#include "nakshatra.h"

#include "swe.h"

#include "catch-formatters.h"

TEST_CASE("positive_delta_between_nakshatras works for zero-crossing pair as well") {
    REQUIRE(vp::positive_delta_between_nakshatras(vp::Nakshatra{26.0}, vp::Nakshatra{1.0}) == 2.0);
    REQUIRE(vp::positive_delta_between_nakshatras(vp::Nakshatra{1.0}, vp::Nakshatra{26.0}) == 25.0);
}

TEST_CASE("positive_delta_between_nakshatras works for zero-crossing pairs when using raw longitude") {
    vp::Nakshatra n1{vp::Longitude_sidereal{360 * 24.0/27.0}};
    vp::Nakshatra n2{vp::Longitude_sidereal{360 * 3.0/27.0}};
    REQUIRE(vp::positive_delta_between_nakshatras(n1, n2) == 6.0);
}

TEST_CASE("creating nakshatra with value <0 or >=27 throws") {
    vp::Nakshatra n{0.0};
    REQUIRE_THROWS(n = vp::Nakshatra{-0.1});
    REQUIRE_THROWS(n = vp::Nakshatra{27.0});
}

TEST_CASE("creating nakshatra with longitude <0 or >=360 throws") {
    vp::Nakshatra n{0.0};
    REQUIRE_THROWS(n = vp::Nakshatra{vp::Longitude_sidereal{-1.0}});
    REQUIRE_THROWS(n = vp::Nakshatra{vp::Longitude_sidereal{360.0}});
}

TEST_CASE("minimal_delta_between_nakshatras works for zero-crossing pair") {
    REQUIRE(vp::minimal_delta_between_nakshatras(vp::Nakshatra{26.0}, vp::Nakshatra{1.0}) == 2.0);
    REQUIRE(vp::minimal_delta_between_nakshatras(vp::Nakshatra{1.0}, vp::Nakshatra{26.0}) == -2.0);
}

TEST_CASE("floor and ceil work") {
    REQUIRE(vp::Nakshatra{1.1}.floor() == vp::Nakshatra{1.0});
    REQUIRE(vp::Nakshatra{26.9}.ceil() == vp::Nakshatra{0.0});
}

TEST_CASE("Nakshatra formatting works") {
    REQUIRE(fmt::to_string(vp::Nakshatra{1.1}) == "Bharani(.100)");
    REQUIRE(fmt::to_string(vp::Nakshatra{26.99999}) == "Ashvini(.000)");
}

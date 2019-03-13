#include "catch.hpp"

#include "tithi.h"

TEST_CASE("Paksha") {
    REQUIRE(Tithi{11.3}.get_paksha() == Paksha::Shukla);
    REQUIRE(Tithi{15+11.3}.get_paksha() == Paksha::Krishna);
}


TEST_CASE("throws on values below zero") {
    REQUIRE_THROWS_AS(Tithi{-1}, std::range_error);
}

TEST_CASE("throws on values of 30 or above") {
    REQUIRE_THROWS_AS(Tithi{30}, std::range_error);
}

TEST_CASE("is Ekadashi") {
    REQUIRE(Tithi{10}.is_ekadashi());
    REQUIRE(Tithi{10.5}.is_ekadashi());
    REQUIRE_FALSE(Tithi{11}.is_ekadashi());
}

TEST_CASE("is Dashami") {
    REQUIRE_FALSE(Tithi{8.99}.is_dashami());
    REQUIRE(Tithi{9}.is_dashami());
    REQUIRE(Tithi{9.99}.is_dashami());
    REQUIRE_FALSE(Tithi{10}.is_dashami());
}

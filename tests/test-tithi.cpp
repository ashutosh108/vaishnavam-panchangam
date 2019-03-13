#include <sstream>

#include "catch.hpp"

#include "tithi.h"

TEST_CASE("Paksha") {
    REQUIRE(Tithi{11.3}.get_paksha() == Paksha::Shukla);
    REQUIRE(Tithi{15+11.3}.get_paksha() == Paksha::Krishna);
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

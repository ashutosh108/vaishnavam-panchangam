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

TEST_CASE("can construct and compare Tithi using symbolic names") {
    REQUIRE(Tithi{Tithi::Dashami} == Tithi{9});
    REQUIRE(Tithi{Tithi::Dashami} != Tithi{9.1});
    REQUIRE(Tithi{Tithi::Ekadashi} == Tithi{10});
    REQUIRE(Tithi{Tithi::Dvadashi} == Tithi{11});
    REQUIRE(Tithi{Tithi::Trayodashi} == Tithi{12});

    REQUIRE(Tithi::Dashami_End == Approx(Tithi::Ekadashi));
    REQUIRE(Tithi::Ekadashi_End == Approx(Tithi::Dvadashi));
    REQUIRE(Tithi::Dvadashi_End == Approx(Tithi::Trayodashi));
}

TEST_CASE("less than and greater than works for Tithi") {
    REQUIRE(Tithi{Tithi::Ekadashi} < Tithi{Tithi::Dvadashi});
    REQUIRE(Tithi{Tithi::Ekadashi} <= Tithi{Tithi::Dvadashi});
    REQUIRE(Tithi{Tithi::Ekadashi} <= Tithi{Tithi::Ekadashi});
    REQUIRE_FALSE(Tithi{Tithi::Dvadashi} <= Tithi{Tithi::Ekadashi});
    REQUIRE(Tithi{Tithi::Ekadashi} > Tithi{Tithi::Dashami});
    REQUIRE(Tithi{Tithi::Ekadashi} >= Tithi{Tithi::Dashami});
    REQUIRE(Tithi{Tithi::Ekadashi} >= Tithi{Tithi::Ekadashi});
    REQUIRE_FALSE(Tithi{Tithi::Ekadashi} >= Tithi{Tithi::Dvadashi});
}

TEST_CASE("delta_until_tithi") {
    REQUIRE(Tithi{Tithi::Ekadashi}.positive_delta_until_tithi(Tithi{Tithi::Dvadashi}) == 1.0);
    REQUIRE(Tithi{Tithi::Dvadashi}.positive_delta_until_tithi(Tithi{Tithi::Ekadashi}) == 29.0);
    REQUIRE(Tithi{Tithi::Ekadashi}.delta_to_nearest_tithi(Tithi{Tithi::Dvadashi}) == 1.0);
    REQUIRE(Tithi{Tithi::Dvadashi}.delta_to_nearest_tithi(Tithi{Tithi::Ekadashi}) == Approx(-1.0));
}

TEST_CASE("Can substract Tithis") {
    REQUIRE(Tithi{Tithi::Dvadashi} - Tithi{Tithi::Dashami} == 2.0);
}

TEST_CASE("Can add double to Tithi") {
    REQUIRE(Tithi{Tithi::Dashami} + 2.0 == Tithi{Tithi::Dvadashi});
}

TEST_CASE("Can substract double from Tithi") {
    REQUIRE(Tithi{Tithi::Dvadashi} - 2.0 == Tithi{Tithi::Dashami});
}

TEST_CASE("Tithi += double should work") {
    Tithi t{Tithi::Ekadashi};
    t += 1.0;
    REQUIRE(t == Tithi{Tithi::Dvadashi});
}

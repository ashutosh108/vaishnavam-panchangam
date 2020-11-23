#include "tithi.h"

#include "catch-formatters.h"
#include "fmt-format-fixed.h"

using namespace vp;

TEST_CASE("Paksha") {
    REQUIRE(Tithi{11.3}.get_paksha() == Paksha::Shukla);
    REQUIRE(Tithi{15+11.3}.get_paksha() == Paksha::Krishna);
}

TEST_CASE("Tithi pretty-printing: Shukla Dvadashi") {
    REQUIRE(fmt::to_string(Tithi{11.3}) == "Shukla Dvadashi(0.3)");
}

TEST_CASE("Tithi pretty-printing: Krishna Ekadashi") {
    REQUIRE(fmt::to_string(Tithi{15+10.4}) == "Krishna Ekadashi(0.4)");
}

TEST_CASE("Tithi pretty-printing: Purnima") {
    REQUIRE(fmt::to_string(Tithi{14.7}) == "Purnima(0.7)");
}

TEST_CASE("Tithi pretty-printing: Amavasya") {
    REQUIRE(fmt::to_string(Tithi{15+14.8}) == "Amavasya(0.8)");
}

TEST_CASE("throws on values below zero") {
    REQUIRE_THROWS_AS(Tithi{-1}, std::range_error);
}

TEST_CASE("throws on values of 30 or above") {
    REQUIRE_THROWS_AS(Tithi{30}, std::range_error);
}

TEST_CASE("is Dvadashi") {
    REQUIRE(Tithi{11}.is_dvadashi());
    REQUIRE(Tithi{11.5}.is_dvadashi());
    REQUIRE_FALSE(Tithi{12}.is_dvadashi());
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

TEST_CASE("is_krishna_pratipat()") {
    REQUIRE(Tithi{15.0}.is_krishna_pratipat());
    REQUIRE(Tithi{15.5}.is_krishna_pratipat());
    REQUIRE_FALSE(Tithi{16.0}.is_krishna_pratipat());
    REQUIRE_FALSE(Tithi{0.0}.is_krishna_pratipat());
    REQUIRE_FALSE(Tithi{0.5}.is_krishna_pratipat());
}

TEST_CASE("can construct and compare Tithi using symbolic names") {
    REQUIRE(Tithi::Dashami() == Tithi{9});
    REQUIRE(Tithi::Dashami() != Tithi{9.1});
    REQUIRE(Tithi::Ekadashi() == Tithi{10});
    REQUIRE(Tithi::Dvadashi() == Tithi{11});
    REQUIRE(Tithi::Trayodashi() == Tithi{12});

    REQUIRE(Tithi::Dashami_End() == Tithi::Ekadashi());
    REQUIRE(Tithi::Ekadashi_End() == Tithi::Dvadashi());
    REQUIRE(Tithi::Dvadashi_End() == Tithi::Trayodashi());
}

TEST_CASE("less than and greater than works for Tithi") {
    REQUIRE(Tithi::Ekadashi() < Tithi::Dvadashi());
    REQUIRE(Tithi::Ekadashi() <= Tithi::Dvadashi());
    REQUIRE(Tithi::Ekadashi() <= Tithi::Ekadashi());
    REQUIRE_FALSE(Tithi::Dvadashi() <= Tithi::Ekadashi());
    REQUIRE(Tithi::Ekadashi() > Tithi::Dashami());
    REQUIRE(Tithi::Ekadashi() >= Tithi::Dashami());
    REQUIRE(Tithi::Ekadashi() >= Tithi::Ekadashi());
    REQUIRE_FALSE(Tithi::Ekadashi() >= Tithi::Dvadashi());
}

TEST_CASE("delta_until_tithi") {
    REQUIRE(Tithi::Ekadashi().positive_delta_until_tithi(Tithi::Dvadashi()) == 1.0);
    REQUIRE(Tithi::Dvadashi().positive_delta_until_tithi(Tithi::Ekadashi()) == 29.0);
    REQUIRE(Tithi::Ekadashi().delta_to_nearest_tithi(Tithi::Dvadashi()) == 1.0);
    REQUIRE(Tithi::Dvadashi().delta_to_nearest_tithi(Tithi::Ekadashi()) == Approx(-1.0));
}

TEST_CASE("Can substract Tithis") {
    REQUIRE(Tithi::Dvadashi() - Tithi::Dashami() == 2.0);
}

TEST_CASE("Can add double to Tithi") {
    REQUIRE(Tithi::Dashami() + 2.0 == Tithi::Dvadashi());
}

TEST_CASE("Can substract double from Tithi") {
    REQUIRE(Tithi::Dvadashi() - 2.0 == Tithi::Dashami());
}

TEST_CASE("Tithi += double should work") {
    Tithi t{Tithi::Ekadashi()};
    t += 1.0;
    REQUIRE(t == Tithi::Dvadashi());
}

TEST_CASE("Tithi::floor(), ceil, + double work") {
    REQUIRE(Tithi{0.5}.floor() == Tithi{0.0});
    REQUIRE(Tithi{1.0}.floor() == Tithi{1.0});
    REQUIRE(Tithi{29.9}.floor() == Tithi{29.0});

    REQUIRE(Tithi{0.5}.ceil() == Tithi{1.0});
    REQUIRE(Tithi{0.0}.ceil() == Tithi{0.0});
    REQUIRE(Tithi{29.9}.ceil() == Tithi{0.0});

    REQUIRE(Tithi{15.0} + 1.0 == Tithi{16.0});
    REQUIRE(Tithi{29.5} + 1.0 == Tithi{0.5});
}

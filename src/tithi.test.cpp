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

TEST_CASE("is_shukla_pratipat()") {
    REQUIRE_FALSE(Tithi{15.0}.is_shukla_pratipat());
    REQUIRE_FALSE(Tithi{15.5}.is_shukla_pratipat());
    REQUIRE(Tithi{0.0}.is_shukla_pratipat());
    REQUIRE(Tithi{0.5}.is_shukla_pratipat());
    REQUIRE_FALSE(Tithi{1.0}.is_shukla_pratipat());
}

TEST_CASE("can construct and compare Tithi using symbolic names") {
    REQUIRE(Tithi::Shukla_Pratipat() == Tithi{0.0});
    REQUIRE(Tithi::Dashami() == Tithi{9});
    REQUIRE(Tithi::Dashami() != Tithi{9.1});
    REQUIRE(Tithi::Ekadashi() == Tithi{10});
    REQUIRE(Tithi::Dvadashi() == Tithi{11});
    REQUIRE(Tithi::Trayodashi() == Tithi{12});
    REQUIRE(Tithi::Chaturdashi() == Tithi{13});
    REQUIRE(Tithi::Purnima() == Tithi{14});
    REQUIRE(Tithi::Krishna_Pratipat() == Tithi{15.0});
    REQUIRE(Tithi::Krishna_Saptami() == Tithi{15.0 + 6.0});
    REQUIRE(Tithi::Krishna_Ashtami() == Tithi{15.0 + 7.0});
    REQUIRE(Tithi::Krishna_Navami() == Tithi{15.0 + 8.0});

    REQUIRE(Tithi::Dashami_End() == Tithi::Ekadashi());
    REQUIRE(Tithi::Ekadashi_End() == Tithi::Dvadashi());
    REQUIRE(Tithi::Dvadashi_End() == Tithi::Trayodashi());
    REQUIRE(Tithi::Trayodashi_End() == Tithi::Chaturdashi());
    REQUIRE(Tithi::Chaturdashi_End() == Tithi::Purnima());
    REQUIRE(Tithi::Purnima_End() == Tithi::Krishna_Pratipat());
    REQUIRE(Tithi::Krishna_Saptami_End() == Tithi::Krishna_Ashtami());
    REQUIRE(Tithi::Krishna_Ashtami_End() == Tithi::Krishna_Navami());

    REQUIRE(Tithi::Amavasya_End() == Tithi::Shukla_Pratipat());
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

TEST_CASE("Tithi enum class") {
    SECTION("can compare") {
        vp::DiscreteTithi t1{vp::DiscreteTithi::Amavasya()};
        vp::DiscreteTithi t2{vp::DiscreteTithi::Shukla_Pratipat()};
        vp::DiscreteTithi t3{vp::DiscreteTithi::Amavasya()};
        REQUIRE(t1 != t2);
        REQUIRE(t1 == t3);
    }
    SECTION("can format OK") {
        CHECK("Śukla Pratipat" == fmt::to_string(vp::DiscreteTithi::Shukla_Pratipat()));
        CHECK("Śukla Dvitīyā" == fmt::to_string(vp::DiscreteTithi::Shukla_Dvitiya()));
        CHECK("Śukla Tṛtīyā" == fmt::to_string(vp::DiscreteTithi::Shukla_Tritiya()));
        CHECK("Śukla Caturthī" == fmt::to_string(vp::DiscreteTithi::Shukla_Chaturthi()));
        CHECK("Śukla Pañcamī" == fmt::to_string(vp::DiscreteTithi::Shukla_Panchami()));
        CHECK("Śukla Ṣaṣṭhī" == fmt::to_string(vp::DiscreteTithi::Shukla_Shashthi()));
        CHECK("Śukla Saptamī" == fmt::to_string(vp::DiscreteTithi::Shukla_Saptami()));
        CHECK("Śukla Aśtamī" == fmt::to_string(vp::DiscreteTithi::Shukla_Ashtami()));
        CHECK("Śukla Navamī" == fmt::to_string(vp::DiscreteTithi::Shukla_Navami()));
        CHECK("Śukla Daśamī" == fmt::to_string(vp::DiscreteTithi::Shukla_Dashami()));
        CHECK("Śukla Ekādaśī" == fmt::to_string(vp::DiscreteTithi::Shukla_Ekadashi()));
        CHECK("Śukla Dvādaśī" == fmt::to_string(vp::DiscreteTithi::Shukla_Dvadashi()));
        CHECK("Śukla Trayodaśī" == fmt::to_string(vp::DiscreteTithi::Shukla_Trayodashi()));
        CHECK("Śukla Caturdaśī" == fmt::to_string(vp::DiscreteTithi::Shukla_Chaturdashi()));
        CHECK("Pūrṇimā" == fmt::to_string(vp::DiscreteTithi::Purnima()));
        CHECK("Kṛṣṇa Pratipat" == fmt::to_string(vp::DiscreteTithi::Krishna_Pratipat()));
        CHECK("Kṛṣṇa Dvitīyā" == fmt::to_string(vp::DiscreteTithi::Krishna_Dvitiya()));
        CHECK("Kṛṣṇa Tṛtīyā" == fmt::to_string(vp::DiscreteTithi::Krishna_Tritiya()));
        CHECK("Kṛṣṇa Caturthī" == fmt::to_string(vp::DiscreteTithi::Krishna_Chaturthi()));
        CHECK("Kṛṣṇa Pañcamī" == fmt::to_string(vp::DiscreteTithi::Krishna_Panchami()));
        CHECK("Kṛṣṇa Ṣaṣṭhī" == fmt::to_string(vp::DiscreteTithi::Krishna_Shashthi()));
        CHECK("Kṛṣṇa Saptamī" == fmt::to_string(vp::DiscreteTithi::Krishna_Saptami()));
        CHECK("Kṛṣṇa Aśtamī" == fmt::to_string(vp::DiscreteTithi::Krishna_Ashtami()));
        CHECK("Kṛṣṇa Navamī" == fmt::to_string(vp::DiscreteTithi::Krishna_Navami()));
        CHECK("Kṛṣṇa Daśamī" == fmt::to_string(vp::DiscreteTithi::Krishna_Dashami()));
        CHECK("Kṛṣṇa Ekādaśī" == fmt::to_string(vp::DiscreteTithi::Krishna_Ekadashi()));
        CHECK("Kṛṣṇa Dvādaśī" == fmt::to_string(vp::DiscreteTithi::Krishna_Dvadashi()));
        CHECK("Kṛṣṇa Trayodaśī" == fmt::to_string(vp::DiscreteTithi::Krishna_Trayodashi()));
        CHECK("Kṛṣṇa Caturdaśī" == fmt::to_string(vp::DiscreteTithi::Krishna_Chaturdashi()));
        CHECK("Amāvāsyā" == fmt::to_string(vp::DiscreteTithi::Amavasya()));
    }
    SECTION("initialization from Tithi class works as expected") {
        REQUIRE(vp::DiscreteTithi{vp::Tithi::Dashami()} == vp::DiscreteTithi::Shukla_Dashami());
        REQUIRE(vp::DiscreteTithi{vp::Tithi::Dashami() + 0.99} == vp::DiscreteTithi::Shukla_Dashami());
        REQUIRE(vp::DiscreteTithi{vp::Tithi::Dashami() + 1.0} == vp::DiscreteTithi::Shukla_Ekadashi());
        REQUIRE(vp::DiscreteTithi{vp::Tithi::Ekadashi()} == vp::DiscreteTithi::Shukla_Ekadashi());
        REQUIRE(vp::DiscreteTithi{vp::Tithi::Ekadashi() + 0.99} == vp::DiscreteTithi::Shukla_Ekadashi());
    }
}

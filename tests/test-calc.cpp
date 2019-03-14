#include <chrono>
#include <sstream>

#include "catch.hpp"

#include "calc.h"
#include "date.h"
#include "swe.h"
#include "swe_time.h"

TEST_CASE("find_next_ekadashi_sunrise") {
    Swe_Time start{2019, 3, 9};
    auto sunrise = Calc{}.find_next_ekadashi_sunrise(start, Coord{50.45, 30.523333});
    Swe_Time expected{2019, 3, 17, 4, 13, 36.270031};
    REQUIRE(sunrise.has_value());
    REQUIRE(*sunrise == expected);
}

TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    Date base_date{2019, 2, 28};
    auto vrata = Calc{}.find_next_vrata(base_date, Coord{50.45, 30.523333});
    REQUIRE(vrata.has_value());
    REQUIRE(Swe{}.get_tithi(Swe_Time{vrata->date}).get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == Date{2019, 3, 2});
}

TEST_CASE("get_arunodaya") {
    auto arunodaya = Calc{}.get_arunodaya(Swe_Time{2019, 3, 2, 4, 45, 58.052015}, Coord{50.45, 30.523333});
    REQUIRE(arunodaya.has_value());
    REQUIRE(*arunodaya == Swe_Time{2019, 3, 2, 3, 0, 17.512880});
}

void test_ekadashi(Date base_date, Coord coord, Date vrata_date) {
    auto vrata = Calc{}.find_next_vrata(base_date, coord);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == vrata_date);
}

Vrata vrata(Calc &c, Date base_date, Coord coord) {
    return c.find_next_vrata(base_date, coord).value();
}

TEST_CASE("Ekadashi 2019-02-28") {
    Date base_date{2019, 2, 28};
    Calc c;
    REQUIRE(vrata(c, base_date, udupi_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, gokarna_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, newdelhi_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, manali_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kalkuta_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, aktau_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, perm_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, ufa_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, ekaterinburg_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, surgut_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, chelyabinsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, ekaterinburg_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, bishkek_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, almaata_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, tekeli_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, omsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, novosibirsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, barnaul_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, tomsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kophangan_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, denpasar_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, mirnyy_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, habarovsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, vladivostok_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, petropavlovskkamchatskiy_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, erevan_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, tbilisi_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, samara_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, volgograd_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, ulyanovsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, pyatigorsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, stavropol_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, semikarakorsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, krasnodar_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, simferopol_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, donetsk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, staryyoskol_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, voronezh_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, tambov_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kazan_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kirov_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, ryazan_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, moskva_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, spb_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, murmansk_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, kostomuksha_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, smolensk_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, minsk_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, gomel_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, harkov_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, poltava_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kremenchug_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, krivoyrog_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kiev_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, nikolaev_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, odessa_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, kolomyya_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, kishinev_coord) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(c, base_date, riga_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, yurmala_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, tallin_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, vilnyus_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, varshava_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, vena_coord) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(c, base_date, marsel_coord) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(c, base_date, madrid_coord) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(c, base_date, london_coord) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(c, base_date, frederikton_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, toronto_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, mayami_coord) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(c, base_date, meadowlake_coord) == Vrata{Date{2019, 3, 1}});
}

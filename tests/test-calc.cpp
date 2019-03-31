#include <chrono>
#include <sstream>

#include "catch.hpp"

#include "calc.h"
#include "date.h"
#include "swe.h"
#include "swe_time.h"

TEST_CASE("find_next_ekadashi_sunrise") {
    Swe_Time start{2019, 3, 9};
    Coord coord{50.45, 30.523333};
    auto sunrise = Calc{coord}.find_next_ekadashi_sunrise(start);
    Swe_Time expected{2019, 3, 17, 4, 13, 36.270031};
    REQUIRE(sunrise.has_value());
    REQUIRE(*sunrise == expected);
}

TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    Date base_date{2019, 2, 28};
    Coord kiev{50.45, 30.523333};
    auto vrata = Calc{kiev}.find_next_vrata(base_date);
    REQUIRE(vrata.has_value());
    REQUIRE(Swe{kiev}.get_tithi(Swe_Time{vrata->date}).get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == Date{2019, 3, 2});
}

TEST_CASE("get_arunodaya") {
    Coord kiev{50.45, 30.523333};
    auto arunodaya = Calc{kiev}.get_arunodaya(Swe_Time{2019, 3, 2, 4, 45, 58.052015});
    REQUIRE(arunodaya.has_value());
    REQUIRE(arunodaya->first == Swe_Time{2019, 3, 2, 3, 0, 17.512880});
}

void test_ekadashi(Date base_date, Coord coord, Date vrata_date) {
    auto vrata = Calc{coord}.find_next_vrata(base_date);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == vrata_date);
}

Vrata vrata(const Calc &c, Date base_date) {
    return c.find_next_vrata(base_date).value();
}

TEST_CASE("Ekadashi 2019-02-28") {
    Date base_date{2019, 2, 28};
    Vrata sandigdha_mar02{Vrata_Type::Sandigdha_Ekadashi, Date{2019, 3, 2}};
    REQUIRE(vrata(Calc{udupi_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{gokarna_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{newdelhi_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{manali_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kalkuta_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{aktau_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{perm_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{ufa_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{ekaterinburg_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{surgut_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{chelyabinsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{ekaterinburg_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{bishkek_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{almaata_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{tekeli_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{omsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{novosibirsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{barnaul_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{tomsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kophangan_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{denpasar_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{mirnyy_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{habarovsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{vladivostok_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{petropavlovskkamchatskiy_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{erevan_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{tbilisi_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{samara_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{volgograd_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{ulyanovsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{pyatigorsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{stavropol_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{semikarakorsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{krasnodar_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{simferopol_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{donetsk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{staryyoskol_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{voronezh_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{tambov_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kazan_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kirov_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{ryazan_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{moskva_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{spb_coord}, base_date) == sandigdha_mar02); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(vrata(Calc{murmansk_coord}, base_date) == sandigdha_mar02); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(vrata(Calc{kostomuksha_coord}, base_date) == sandigdha_mar02); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(vrata(Calc{smolensk_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{minsk_coord}, base_date) == sandigdha_mar02); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(vrata(Calc{gomel_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{harkov_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{poltava_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kremenchug_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{krivoyrog_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kiev_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{nikolaev_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{odessa_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{kolomyya_coord}, base_date) == sandigdha_mar02); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(vrata(Calc{kishinev_coord}, base_date) == Vrata{Date{2019, 3, 2}});
    REQUIRE(vrata(Calc{riga_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{yurmala_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{tallin_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{vilnyus_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{varshava_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{vena_coord}, base_date) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(Calc{marsel_coord}, base_date) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(Calc{madrid_coord}, base_date) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(Calc{london_coord}, base_date) == Vrata{Date{2019, 3, 1}}); //atirikta dvadashi
    REQUIRE(vrata(Calc{frederikton_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{toronto_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{mayami_coord}, base_date) == Vrata{Date{2019, 3, 1}});
    REQUIRE(vrata(Calc{meadowlake_coord}, base_date) == Vrata{Date{2019, 3, 1}});
}

TEST_CASE("Ekadashi 2019-03-17") {
    Date d{2019, 3, 15};
    Vrata v17{Date{2019, 3, 17}};
    Vrata sandigdha_18{Vrata_Type::Sandigdha_Ekadashi, Date{2019, 3, 18}};
    REQUIRE(v17 == vrata(Calc{udupi_coord}, d));
    REQUIRE(v17 == vrata(Calc{gokarna_coord}, d));
    REQUIRE(v17 == vrata(Calc{newdelhi_coord}, d));
    REQUIRE(v17 == vrata(Calc{manali_coord}, d));
    REQUIRE(v17 == vrata(Calc{kalkuta_coord}, d));
    REQUIRE(v17 == vrata(Calc{aktau_coord}, d));
    REQUIRE(v17 == vrata(Calc{perm_coord}, d));
    REQUIRE(v17 == vrata(Calc{ufa_coord}, d));
    REQUIRE(v17 == vrata(Calc{ekaterinburg_coord}, d));
    REQUIRE(v17 == vrata(Calc{surgut_coord}, d));
    REQUIRE(v17 == vrata(Calc{chelyabinsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{ekaterinburg_coord}, d));
    REQUIRE(v17 == vrata(Calc{bishkek_coord}, d));
    REQUIRE(v17 == vrata(Calc{almaata_coord}, d));
    REQUIRE(v17 == vrata(Calc{tekeli_coord}, d));
    REQUIRE(v17 == vrata(Calc{omsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{novosibirsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{barnaul_coord}, d));
    REQUIRE(v17 == vrata(Calc{tomsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{kophangan_coord}, d));
    REQUIRE(v17 == vrata(Calc{denpasar_coord}, d));
    REQUIRE(v17 == vrata(Calc{mirnyy_coord}, d));
    REQUIRE(v17 == vrata(Calc{habarovsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{vladivostok_coord}, d));
    REQUIRE(sandigdha_18 == vrata(Calc{petropavlovskkamchatskiy_coord}, d)); // caused the code fix
    REQUIRE(v17 == vrata(Calc{erevan_coord}, d));
    REQUIRE(v17 == vrata(Calc{tbilisi_coord}, d));
    REQUIRE(v17 == vrata(Calc{samara_coord}, d));
    REQUIRE(v17 == vrata(Calc{volgograd_coord}, d));
    REQUIRE(v17 == vrata(Calc{ulyanovsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{pyatigorsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{stavropol_coord}, d));
    REQUIRE(v17 == vrata(Calc{semikarakorsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{krasnodar_coord}, d));
    REQUIRE(v17 == vrata(Calc{simferopol_coord}, d));
    REQUIRE(v17 == vrata(Calc{donetsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{staryyoskol_coord}, d));
    REQUIRE(v17 == vrata(Calc{voronezh_coord}, d));
    REQUIRE(v17 == vrata(Calc{tambov_coord}, d));
    REQUIRE(v17 == vrata(Calc{kazan_coord}, d));
    REQUIRE(v17 == vrata(Calc{kirov_coord}, d));
    REQUIRE(v17 == vrata(Calc{ryazan_coord}, d));
    REQUIRE(v17 == vrata(Calc{moskva_coord}, d));
    REQUIRE(v17 == vrata(Calc{spb_coord}, d));
    REQUIRE(v17 == vrata(Calc{murmansk_coord}, d));
    REQUIRE(v17 == vrata(Calc{kostomuksha_coord}, d));
    REQUIRE(v17 == vrata(Calc{smolensk_coord}, d));
    REQUIRE(v17 == vrata(Calc{minsk_coord}, d));
    REQUIRE(v17 == vrata(Calc{gomel_coord}, d));
    REQUIRE(v17 == vrata(Calc{harkov_coord}, d));
    REQUIRE(v17 == vrata(Calc{poltava_coord}, d));
    REQUIRE(v17 == vrata(Calc{kremenchug_coord}, d));
    REQUIRE(v17 == vrata(Calc{krivoyrog_coord}, d));
    REQUIRE(v17 == vrata(Calc{kiev_coord}, d));
    REQUIRE(v17 == vrata(Calc{nikolaev_coord}, d));
    REQUIRE(v17 == vrata(Calc{odessa_coord}, d));
    REQUIRE(v17 == vrata(Calc{kolomyya_coord}, d));
    REQUIRE(v17 == vrata(Calc{kishinev_coord}, d));
    REQUIRE(v17 == vrata(Calc{riga_coord}, d));
    REQUIRE(v17 == vrata(Calc{yurmala_coord}, d));
    REQUIRE(v17 == vrata(Calc{tallin_coord}, d));
    REQUIRE(v17 == vrata(Calc{vilnyus_coord}, d));
    REQUIRE(v17 == vrata(Calc{varshava_coord}, d));
    REQUIRE(v17 == vrata(Calc{vena_coord}, d));
    REQUIRE(v17 == vrata(Calc{marsel_coord}, d));
    REQUIRE(v17 == vrata(Calc{madrid_coord}, d));
    REQUIRE(v17 == vrata(Calc{london_coord}, d));
    REQUIRE(v17 == vrata(Calc{frederikton_coord}, d));
    REQUIRE(v17 == vrata(Calc{toronto_coord}, d));
    REQUIRE(v17 == vrata(Calc{mayami_coord}, d));
    REQUIRE(v17 == vrata(Calc{meadowlake_coord}, d));
}

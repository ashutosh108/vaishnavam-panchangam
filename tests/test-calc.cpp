#include <chrono>
#include <sstream>

#include "catch.hpp"

#include "calc.h"
#include "date.h"
#include "paran.h"
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
    REQUIRE(swe::Swe{kiev}.get_tithi(Swe_Time{vrata->date}).get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == Date{2019, 3, 2});
}

TEST_CASE("get_arunodaya") {
    Coord kiev{50.45, 30.523333};
    auto arunodaya = Calc{kiev}.get_arunodaya(Swe_Time{2019, 3, 2, 4, 45, 58.052015});
    REQUIRE(arunodaya.has_value());
    REQUIRE(arunodaya->first == Swe_Time{2019, 3, 2, 3, 0, 17.512880});
}

Vrata vrata(const Calc &c, Date base_date) {
    return c.find_next_vrata(base_date).value();
}

TEST_CASE("Ekadashi 2019-02-28") {
    Date d{2019, 2, 28};
    Vrata v01{Date{2019, 3, 1}};
    Vrata v02{Date{2019, 3, 2}};
    Vrata sandigdha_mar02{Vrata_Type::Sandigdha_Ekadashi, Date{2019, 3, 2}};
    REQUIRE(v02 == vrata(Calc{udupi_coord}, d));
    REQUIRE(v02 == vrata(Calc{gokarna_coord}, d));
    REQUIRE(v02 == vrata(Calc{newdelhi_coord}, d));
    REQUIRE(v02 == vrata(Calc{manali_coord}, d));
    REQUIRE(v02 == vrata(Calc{kalkuta_coord}, d));
    REQUIRE(v02 == vrata(Calc{aktau_coord}, d));
    REQUIRE(v02 == vrata(Calc{perm_coord}, d));
    REQUIRE(v02 == vrata(Calc{ufa_coord}, d));
    REQUIRE(v02 == vrata(Calc{ekaterinburg_coord}, d));
    REQUIRE(v02 == vrata(Calc{surgut_coord}, d));
    REQUIRE(v02 == vrata(Calc{chelyabinsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{ekaterinburg_coord}, d));
    REQUIRE(v02 == vrata(Calc{bishkek_coord}, d));
    REQUIRE(v02 == vrata(Calc{almaata_coord}, d));
    REQUIRE(v02 == vrata(Calc{tekeli_coord}, d));
    REQUIRE(v02 == vrata(Calc{omsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{novosibirsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{barnaul_coord}, d));
    REQUIRE(v02 == vrata(Calc{tomsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{kophangan_coord}, d));
    REQUIRE(v02 == vrata(Calc{denpasar_coord}, d));
    REQUIRE(v02 == vrata(Calc{mirnyy_coord}, d));
    REQUIRE(v02 == vrata(Calc{habarovsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{vladivostok_coord}, d));
    REQUIRE(v02 == vrata(Calc{petropavlovskkamchatskiy_coord}, d));
    REQUIRE(v02 == vrata(Calc{erevan_coord}, d));
    REQUIRE(v02 == vrata(Calc{tbilisi_coord}, d));
    REQUIRE(v02 == vrata(Calc{samara_coord}, d));
    REQUIRE(v02 == vrata(Calc{volgograd_coord}, d));
    REQUIRE(v02 == vrata(Calc{ulyanovsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{pyatigorsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{stavropol_coord}, d));
    REQUIRE(v02 == vrata(Calc{semikarakorsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{krasnodar_coord}, d));
    REQUIRE(v02 == vrata(Calc{simferopol_coord}, d));
    REQUIRE(v02 == vrata(Calc{donetsk_coord}, d));
    REQUIRE(v02 == vrata(Calc{staryyoskol_coord}, d));
    REQUIRE(v02 == vrata(Calc{voronezh_coord}, d));
    REQUIRE(v02 == vrata(Calc{tambov_coord}, d));
    REQUIRE(v02 == vrata(Calc{kazan_coord}, d));
    REQUIRE(v02 == vrata(Calc{kirov_coord}, d));
    REQUIRE(v02 == vrata(Calc{ryazan_coord}, d));
    REQUIRE(v02 == vrata(Calc{moskva_coord}, d));
    REQUIRE(sandigdha_mar02 == vrata(Calc{spb_coord}, d)); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(sandigdha_mar02 == vrata(Calc{murmansk_coord}, d)); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(sandigdha_mar02 == vrata(Calc{kostomuksha_coord}, d)); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{smolensk_coord}, d));
    REQUIRE(sandigdha_mar02 == vrata(Calc{minsk_coord}, d)); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{gomel_coord}, d));
    REQUIRE(v02 == vrata(Calc{harkov_coord}, d));
    REQUIRE(v02 == vrata(Calc{poltava_coord}, d));
    REQUIRE(v02 == vrata(Calc{kremenchug_coord}, d));
    REQUIRE(v02 == vrata(Calc{krivoyrog_coord}, d));
    REQUIRE(v02 == vrata(Calc{kiev_coord}, d));
    REQUIRE(v02 == vrata(Calc{nikolaev_coord}, d));
    REQUIRE(v02 == vrata(Calc{odessa_coord}, d));
    REQUIRE(sandigdha_mar02 == vrata(Calc{kolomyya_coord}, d)); // Sandighdha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{kishinev_coord}, d));
    REQUIRE(v01 == vrata(Calc{riga_coord}, d));     // > 14:15
    REQUIRE(v01 == vrata(Calc{yurmala_coord}, d));  // > 14:15
    REQUIRE(v01 == vrata(Calc{tallin_coord}, d));   // > 14:15
    REQUIRE(v01 == vrata(Calc{vilnyus_coord}, d));  // > 14:15
    REQUIRE(v01 == vrata(Calc{varshava_coord}, d)); // > 13:15
    REQUIRE(v01 == vrata(Calc{vena_coord}, d)); //atirikta dvadashi
    REQUIRE(v01 == vrata(Calc{marsel_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01 == vrata(Calc{madrid_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01 == vrata(Calc{london_coord}, d)); //atirikta dvadashi   < 8:14
    REQUIRE(v01 == vrata(Calc{frederikton_coord}, d));  // > 8:15
    REQUIRE(v01 == vrata(Calc{toronto_coord}, d));      // > 7:15
    REQUIRE(v01 == vrata(Calc{mayami_coord}, d));       // > 7:15
    REQUIRE(v01 == vrata(Calc{meadowlake_coord}, d));
}

class Expected_Vrata {
public:
    Vrata_Type type;
    Date date;
    Paran paran;
    Expected_Vrata(Vrata_Type _type, Date _date, Paran _paran)
        :type(_type), date(_date), paran(_paran)
    {}
};

std::ostream &operator <<(std::ostream &s, const Expected_Vrata &v) {
    s << v.type;
    {
        s << "{" << v.date;
        s << ", " << v.paran << "}";
    }
    return s;
}

bool operator==(const Expected_Vrata &e, const Vrata &v) {
    // if "expected" paran_start/end is nullopt, then we don't care
    // about v2's one.
    // Otherwise, if expected paran_start/end is given, it must be equal to v2's one.
    return e.type == v.type && e.date == v.date &&
            e.paran.type == v.paran.type &&
            (!e.paran.paran_start || e.paran.paran_start == v.paran.paran_start) &&
            (!e.paran.paran_end || e.paran.paran_end == v.paran.paran_end);
}

TEST_CASE("Ekadashi 2019-03-17") {
    Date d{2019, 3, 15};
    Vrata v17{Date{2019, 3, 17}};
    Expected_Vrata v17_paran_before{
        Vrata_Type::Ekadashi,
        Date{2019, 3, 17},
        Paran{Paran::Type::Until_Dvadashi_End, std::nullopt, Swe_Time{2019, 3, 18, 12, 13, 36.459301}}};
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
    REQUIRE(v17_paran_before == vrata(Calc{frederikton_coord}, d));
    REQUIRE(v17_paran_before == vrata(Calc{toronto_coord}, d));
    REQUIRE(v17_paran_before == vrata(Calc{mayami_coord}, d));
    Vrata v_meadowlake = vrata(Calc{meadowlake_coord}, d);
    REQUIRE(v17 == v_meadowlake);
    REQUIRE((v_meadowlake.paran.paran_start && v_meadowlake.paran.paran_end));
    REQUIRE(*v_meadowlake.paran.paran_start < *v_meadowlake.paran.paran_end);
}

TEST_CASE("get_next_tithi_start") {
    Swe_Time from{2019, 3, 17};
    Tithi tithi{Tithi::Dvadashi_End};
    Swe_Time expected{2019, 3, 18, 12, 13, 36.459301};
    std::optional<Swe_Time> actual = Calc{frederikton_coord}.get_next_tithi_start(from, tithi);
    REQUIRE(actual.has_value());
    REQUIRE(*actual == expected);
}

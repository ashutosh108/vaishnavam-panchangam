#include "calc.h"

#include "paran.h"
#include "swe.h"
#include "juldays_ut.h"

#include "catch.hpp"
#include <chrono>
#include <condition_variable>
#include "date-fixed.h"
#include <sstream>
#include "tz-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

class Expected_Vrata {
public:
    Vrata_Type type;
    date::year_month_day date;
    Paran paran;
    Expected_Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran)
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

class ApproximateJulDays_UT {
    JulDays_UT juldays_;
public:
    ApproximateJulDays_UT(const JulDays_UT & juldays)
        : juldays_(juldays)
    {}
    bool operator==(const JulDays_UT & other) {
        return juldays_.raw_julian_days_ut().count() == Approx(other.raw_julian_days_ut().count());
    }
};

bool operator==(const Expected_Vrata &e, const Vrata &v) {
    // if "expected" paran_start/end is nullopt, then we don't care
    // about v2's one.
    // Otherwise, if expected paran_start/end is given, it must be equal to v2's one.
    bool paran_start_is_missing_or_same =
            !e.paran.paran_start ||
            (
                v.paran.paran_start.has_value() &&
                ApproximateJulDays_UT{*e.paran.paran_start} == *v.paran.paran_start
            );
    bool paran_end_is_missing_or_same =
            !e.paran.paran_end ||
            (
                v.paran.paran_end.has_value() &&
                ApproximateJulDays_UT{*e.paran.paran_end} == *v.paran.paran_end
            );
    return
            e.type == v.type &&
            e.date == v.date &&
            e.paran.type == v.paran.type &&
            paran_start_is_missing_or_same &&
            paran_end_is_missing_or_same;
}

TEST_CASE("find_next_ekadashi_sunrise") {
    JulDays_UT start{2019_y/March/9};
    Location coord{50.45, 30.523333};
    auto sunrise = Calc{coord}.find_next_ekadashi_sunrise(start);
    JulDays_UT expected{2019_y/March/17, 4h + 13min + 36.270031s};
    REQUIRE(sunrise.has_value());
    REQUIRE(*sunrise == expected);
}

TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    date::year_month_day base_date{2019_y/February/28};
    Location kiev{50.45, 30.523333};
    auto vrata = Calc{kiev}.find_next_vrata(base_date);
    REQUIRE(vrata.has_value());
    REQUIRE(vp::Swe{kiev}.get_tithi(JulDays_UT{vrata->date}).get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == 2019_y/March/2);
}

TEST_CASE("get_arunodaya") {
    Location kiev{50.45, 30.523333};
    auto arunodaya = Calc{kiev}.get_arunodaya(JulDays_UT{2019_y/March/2, 4h + 45min + 58.052015s});
    REQUIRE(arunodaya.has_value());
    REQUIRE(arunodaya->first == JulDays_UT{2019_y/March/2, 3h + 0min + 17.512880s});
}

Vrata vrata(const Calc &c, date::year_month_day base_date) {
    return c.find_next_vrata(base_date).value();
}

TEST_CASE("Ekadashi 2019-02-28") {
    // https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-02-28/
    date::year_month_day d{2019_y/February/28};
    Vrata v01{2019_y/March/1};
    Vrata v02{2019_y/March/2};
    Vrata sandigdha_mar02{Vrata_Type::Sandigdha_Ekadashi, 2019_y/March/2};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/March/1, Paran{Paran::Type::Puccha_Dvadashi}};
    Vrata v01_atirikta_ekadashi{Vrata_Type::Atirikta_Ekadashi, 2019_y/March/1, Paran{Paran::Type::Puccha_Dvadashi}};
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
    Paran paran1415{Paran::Type::From_Quarter_Dvadashi, JulDays_UT{2019_y/March/2, 12h + 14min + 40.513510s}};
    Expected_Vrata v01_paran_after_quarter{Vrata_Type::Ekadashi, 2019_y/March/1, paran1415};
    REQUIRE(v01_atirikta_ekadashi == vrata(Calc{riga_coord}, d));     // > 14:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_ekadashi == vrata(Calc{yurmala_coord}, d));  // > 14:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_ekadashi == vrata(Calc{tallin_coord}, d));   // > 14:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_ekadashi == vrata(Calc{vilnyus_coord}, d));  // > 14:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_ekadashi == vrata(Calc{varshava_coord}, d)); // > 13:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{vena_coord}, d)); //atirikta dvadashi
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{marsel_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{madrid_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{london_coord}, d)); //atirikta dvadashi   < 8:14
    REQUIRE(v01_paran_after_quarter == vrata(Calc{frederikton_coord}, d));  // > 8:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{toronto_coord}, d));      // > 7:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{miami_coord}, d));       // > 7:15
    REQUIRE(v01 == vrata(Calc{meadowlake_coord}, d));
}

struct NamedVrata {
    const char *name;
    const Vrata & vrata;
    NamedVrata(const char * name_, const Vrata & vrata_)
        :name(name_), vrata(vrata_)
    {}
};

bool operator==(const NamedVrata &expected, const Vrata & actual) {
    return expected.vrata == actual;
}

std::ostream & operator<<(std::ostream & s, const NamedVrata &nv) {
    return s << nv.name << ": " << nv.vrata;
}

void check_atirikta_at_location(const char * name, const Location & location, const Vrata & expected_vrata, date::year_month_day date) {
    CAPTURE(name);
    auto actual_vrata = vrata(Calc{location}, date);
    NamedVrata expected_named_vrata{name, expected_vrata};
    CHECK(expected_named_vrata == actual_vrata);

    // 1. Paran start and end should exist.
    REQUIRE(actual_vrata.paran.paran_start.has_value());
    REQUIRE(actual_vrata.paran.paran_end.has_value());

    // 2. Paran *date* must be two days after vrata date.
    auto time_zone_at_location = date::locate_zone(location.timezone_name);

    auto zoned_paran_start = actual_vrata.paran.paran_start->as_zoned_time(time_zone_at_location);
    auto local_days_paran_start = date::floor<date::days>(zoned_paran_start.get_local_time());
    CHECK(date::local_days{actual_vrata.date} + date::days{2} == local_days_paran_start);

    auto zoned_paran_end = date::make_zoned(time_zone_at_location, actual_vrata.paran.paran_end->as_sys_time());
    auto local_days_paran_end = date::floor<date::days>(zoned_paran_end.get_local_time());
    CHECK(date::local_days{actual_vrata.date} + date::days{2} == local_days_paran_end);

    // 3. TODO: Paran_start must match sunrise
    // 4. TODO: Paran_end must match dvadashi_end
}

TEST_CASE("atiriktA-dvAdashI {vena,marsel,madrid,london} 2019-03-01-2 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-02-28/") {
    date::year_month_day d{2019_y/February/28};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/March/1, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("vena", vena_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("marsel", marsel_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("madrid", madrid_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("london", london_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {aktau, surgut, chelyabinsk, yerevan, tbilisi} 2019-12-07-8 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-12-01/") {
    date::year_month_day d{2019_y/December/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/December/7, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("aktau", aktau_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("surgut", surgut_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("chelyabinsk", chelyabinsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("erevan", erevan_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("tbilisi", tbilisi_coord, v01_atirikta_dvadashi, d);
}


TEST_CASE("atiriktA-dvAdashI {madrid,london} 2019-11-07-8 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-11-05/") {
    date::year_month_day d{2019_y/November/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/November/7, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("madrid", madrid_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("london", london_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI meadowlake 2019-10-08-9 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-10-04/") {
    date::year_month_day d{2019_y/October/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/October/8, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("meadowlake", meadowlake_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {hab,vlad} 2019-09-09-10 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-09-07/") {
    date::year_month_day d{2019_y/September/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, 2019_y/September/9, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("habarovsk", habarovsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("vladivostok", vladivostok_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {mirny,hab,vlad} 2019-04-30-01 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-04-27/") {
    date::year_month_day d{2019_y/April/30};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, d, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("mirnyy", mirnyy_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("habarovsk", habarovsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("vladivostok", vladivostok_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-ekAdashI {ppk} 2019-04-30-01 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-04-27/") {
    date::year_month_day d{2019_y/April/30};
    Vrata v01_atirikta_dvadashi{Vrata_Type::Atirikta_Ekadashi, d, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("petropavlovskkamchatskiy", petropavlovskkamchatskiy_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("Ekadashi 2019-03-17") {
    date::year_month_day d{2019_y/March/15};
    Vrata v17{2019_y/March/17};
    Expected_Vrata v17_paran_before{
        Vrata_Type::Ekadashi,
        2019_y/March/17,
        Paran{Paran::Type::Until_Dvadashi_End, std::nullopt, JulDays_UT{2019_y/March/18, 12h + 13min + 36.459301s}}};
    Vrata sandigdha_18{Vrata_Type::Sandigdha_Ekadashi, 2019_y/March/18};
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
    REQUIRE(v17_paran_before == vrata(Calc{miami_coord}, d));
    Vrata v_meadowlake = vrata(Calc{meadowlake_coord}, d);
    REQUIRE(v17 == v_meadowlake);
    REQUIRE((v_meadowlake.paran.paran_start && v_meadowlake.paran.paran_end));
    REQUIRE(*v_meadowlake.paran.paran_start < *v_meadowlake.paran.paran_end);
}

TEST_CASE("get_next_tithi_start gives what we expect (close to the target tithi)") {
    JulDays_UT from{2019_y/March/17};
    Tithi expected_tithi{Tithi::Dvadashi_End};
    Calc calc{frederikton_coord};

    std::optional<JulDays_UT> actual_time = calc.get_next_tithi_start(from, expected_tithi);

    REQUIRE(actual_time.has_value());
    Tithi actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(expected_tithi == actual_tithi);
}

auto get_next_tithi_wrapper(Calc const &calc, JulDays_UT from, Tithi tithi) {
    std::optional<JulDays_UT> retval;

    retval = calc.get_next_tithi_start(from, tithi);
    return retval;
}

auto get_next_tithi_wrapper(Location coord, JulDays_UT from, Tithi tithi) {
    Calc calc{coord};
    return get_next_tithi_wrapper(calc, from, tithi);
}

TEST_CASE("get_next_tithi_start breaks out from eternal loop") {
    JulDays_UT from{2019_y/April/29, double_hours{2.0411111153662205}};
    Tithi tithi{Tithi::Ekadashi};
    auto actual = get_next_tithi_wrapper(london_coord, from, tithi);
    REQUIRE(actual.has_value());
}

TEST_CASE("get_next_tithi() returns Shukla Ekadashi after Shukla something tithi") {
    Calc const calc{london_coord};
    JulDays_UT const from{2019_y/May/12};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= double_days{14});
}

TEST_CASE("get_next_tithi() returns Krishna Ekadashi after Krishna something tithi") {
    Calc const calc{london_coord};
    JulDays_UT const from{2019_y/April/25};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi-15.0 == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= double_days{14});
}

TEST_CASE("get_next_tithi() gives closest Ekadashi tithi for petropavlovsk after 2019-03-15") {
    Calc const calc{petropavlovskkamchatskiy_coord};
    JulDays_UT const from{2019_y/March/15};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= double_days{14});
}

TEST_CASE("paran not earlier than quarter of Dvadashi tithi") {
    Calc const calc{kiev_coord};
    date::year_month_day const from{2019_y/August/24};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata.has_value());
    REQUIRE(vrata->paran.type == Paran::Type::From_Quarter_Dvadashi);
    REQUIRE(vrata->paran.paran_start.has_value());
    JulDays_UT paran_start = vrata->paran.paran_start.value();
    JulDays_UT paran_start_not_before{2019_y/August/27, double_hours{5.0}};
    JulDays_UT paran_start_not_after{2019_y/August/27, double_hours{5.2}};
    REQUIRE(paran_start > paran_start_not_before);
    REQUIRE(paran_start < paran_start_not_after);
}

TEST_CASE("Udupi after 2019-11-20 returns an actual date") {
    Calc const calc{udupi_coord};
    date::year_month_day const from{2019_y/November/20};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata.has_value());
}

TEST_CASE("Udupi after 2020-02-06 does NOT return 2020-02-06") {
    Calc const calc{udupi_coord};
    date::year_month_day const from{2020_y/February/6};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata->date != from);
}

TEST_CASE("Kiev after 2020-01-21 does NOT return 2020-01-21") {
    Calc const calc{kiev_coord};
    date::year_month_day const from{2020_y/January/21};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata->date != from);
}

TEST_CASE("get_astronomical_midnight returns time *before* utc midnight for location to the east from greenwich") {
    vp::Calc c{moskva_coord};
    auto date = 2019_y/January/1;

    REQUIRE(c.calc_astronomical_midnight(date) < vp::JulDays_UT{date});
}

TEST_CASE("get_astronomical_midnight returns time *after* utc midnight for location to the west from greenwich") {
    vp::Calc c{losanjeles_coord};
    auto date = 2019_y/January/1;

    REQUIRE(c.calc_astronomical_midnight(date) > vp::JulDays_UT{date});
}

TEST_CASE("get_astronomical_midnight() adjusts to the right side") {
    Calc c{petropavlovskkamchatskiy_coord};
    auto date = 2019_y/March/18;

    auto local_midnight = c.calc_astronomical_midnight(date);
    JulDays_UT local_midnight_in_utc_earliest{2019_y/March/17};
    JulDays_UT local_midnight_in_utc_latest{2019_y/March/18};
    REQUIRE(local_midnight > local_midnight_in_utc_earliest);
    REQUIRE(local_midnight < local_midnight_in_utc_latest);
}

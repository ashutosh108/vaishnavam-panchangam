#include "calc.h"

#include "juldays_ut.h"
#include "paran.h"
#include "swe.h"
#include "vrata_detail_printer.h"

#include "catch-formatters.h"
#include <chrono>
#include <condition_variable>
#include "date-fixed.h"
#include <fmt/ostream.h>
#include <sstream>
#include "tz-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

class Expected_Vrata {
public:
    Vrata_Type type;
    date::local_days date;
    Paran paran;
    Chandra_Masa masa;
    Paksha paksha;
    Expected_Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran, Chandra_Masa _masa, Paksha _paksha)
        :type(_type), date(_date), paran(_paran), masa(_masa), paksha(_paksha)
    {}
};

std::ostream &operator <<(std::ostream &s, const Expected_Vrata &v) {
    fmt::print(s, FMT_STRING("{}{{{}, {}, {} māsa, {} pakṣa}}"), v.type, v.date, v.paran, v.masa, v.paksha);
    return s;
}

class ApproximateJulDays_UT {
    JulDays_UT juldays_;
public:
    template<class ... Types>
    explicit ApproximateJulDays_UT(Types ...args) : juldays_(args...) {}

    bool operator==(const JulDays_UT & other) const {
        constexpr double_days tolerance = 5s;
        auto delta = juldays_ - other;
        return (delta > -tolerance) && (delta < tolerance);
    }
    friend std::ostream & operator<<(std::ostream & s, const ApproximateJulDays_UT & u) {
        fmt::print(s, "Approx{{{}}}", u.juldays_);
        return s;
    }
};

bool operator == (const JulDays_UT & left, const ApproximateJulDays_UT & right) {
    return right == left;
}


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
            e.masa == v.masa &&
            e.paksha == v.paksha &&
            paran_start_is_missing_or_same &&
            paran_end_is_missing_or_same;
}

TEST_CASE("find_ekadashi_sunrise") {
    JulDays_UT start{2019_y/March/9};
    Location coord{50.45_N, 30.523333_E};
    auto sunrise = Calc{coord}.find_ekadashi_sunrise(start);
    auto expected = date::sys_days(2019_y/March/17) + 4h + 13min/* + 36.270031s*/;
    REQUIRE(sunrise.has_value());
    REQUIRE(sunrise->round_to_minute_down() == expected);
}

TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    date::local_days base_date{2019_y/February/28};
    Location kiev{50.45_N, 30.523333_E};
    auto vrata = Calc{kiev}.find_next_vrata(base_date);

    SECTION("basic") {
        REQUIRE(vrata.has_value());
        REQUIRE(vp::Swe{kiev}.get_tithi(JulDays_UT{vrata->date}).get_paksha() == Paksha::Krishna);
        REQUIRE(vrata->type == Vrata_Type::Ekadashi);
        REQUIRE(date::year_month_day{vrata->date} == 2019_y/March/2);
    }

    SECTION("has expected Hari-vAsara") {
        const auto expected = date::sys_days(2019_y/March/1) + 22h + 58min;
        const auto harivasara = vrata->harivasara();
        REQUIRE(harivasara.has_value());
        REQUIRE(harivasara->round_to_minute() == expected);
    }
}

TEST_CASE("arunodaya_for_sunrise") {
    Location kiev{50.45_N, 30.523333_E};
    auto arunodaya = Calc{kiev}.arunodaya_for_sunrise(JulDays_UT{2019_y/March/2, 4h + 45min + 58.052015s});
    REQUIRE(arunodaya.has_value());
    REQUIRE(arunodaya->round_to_minute_down() == date::sys_days(2019_y/March/2) + 3h + 0min /*+ 17.512880s*/);
}

namespace {
Vrata vrata(const Calc &c, date::local_days base_date) {
    return c.find_next_vrata(base_date).value();
}
}

TEST_CASE("Ekadashi 2019-02-28") {
    // https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-02-28/
    const date::local_days d{2019_y/February/28};
    Vrata v01{date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna};
    Vrata v02{date::local_days{2019_y/March/2}, Chandra_Masa::Magha, Paksha::Krishna};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};
    Vrata v01_atirikta_dvadashi_std_paran{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Standard}};
    Vrata v01_atirikta_ekadashi{Vrata_Type::With_Atirikta_Ekadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};
    Vrata v01_atirikta_ekadashi_std_paran{Vrata_Type::With_Atirikta_Ekadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Standard}};
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
    REQUIRE(v02 == vrata(Calc{spb_coord}, d)); // moved +1day because vRddha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{murmansk_coord}, d)); // moved +1day because vRddha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{kostomuksha_coord}, d)); // moved +1day because vRddha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{smolensk_coord}, d));
    REQUIRE(v02 == vrata(Calc{minsk_coord}, d)); // moved +1day because vRddha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{gomel_coord}, d));
    REQUIRE(v02 == vrata(Calc{harkov_coord}, d));
    REQUIRE(v02 == vrata(Calc{poltava_coord}, d));
    REQUIRE(v02 == vrata(Calc{kremenchug_coord}, d));
    REQUIRE(v02 == vrata(Calc{krivoyrog_coord}, d));
    REQUIRE(v02 == vrata(Calc{kiev_coord}, d));
    REQUIRE(v02 == vrata(Calc{nikolaev_coord}, d));
    REQUIRE(v02 == vrata(Calc{odessa_coord}, d));
    REQUIRE(v02 == vrata(Calc{kolomyya_coord}, d)); // moved +1day because vRddha, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{kishinev_coord}, d));
    Paran paran1415{Paran::Type::From_Quarter_Dvadashi, JulDays_UT{2019_y/March/2, 12h + 14min + 40.513510s}};
    Expected_Vrata v01_paran_after_quarter{Vrata_Type::Ekadashi, 2019_y/March/1, paran1415, Chandra_Masa::Magha, Paksha::Krishna};
    REQUIRE(v02 == vrata(Calc{riga_coord}, d));     // 55gh (vRddha) < ekAdashI start < arddha-ghaTika before aruNodaya0, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{jurmala_coord}, d));  // 55gh (vRddha) < ekAdashI start < arddha-ghaTika before aruNodaya0, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{tallin_coord}, d));   // 55gh (vRddha) < ekAdashI start < arddha-ghaTika before aruNodaya0, differs from Naarasimha's calendar
    REQUIRE(v02 == vrata(Calc{vilnyus_coord}, d));  // 55gh (vRddha) < ekAdashI start < arddha-ghaTika before aruNodaya0, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_ekadashi_std_paran == vrata(Calc{varshava_coord}, d)); // > 13:15, differs from Naarasimha's calendar
    REQUIRE(v01_atirikta_dvadashi_std_paran == vrata(Calc{vena_coord}, d)); //atirikta dvadashi
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{marsel_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{madrid_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_atirikta_dvadashi == vrata(Calc{london_coord}, d)); //atirikta dvadashi   < 8:14
    REQUIRE(v01_paran_after_quarter == vrata(Calc{fredericton_coord}, d));  // > 8:15
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
    fmt::print(s, "{}: {}", nv.name, nv.vrata);
    return s;
}

void check_atirikta_at_location(const char * name, const Location & location, const Vrata & expected_vrata, date::local_days date) {
    CAPTURE(name);
    auto actual_vrata = vrata(Calc{location}, date);
    NamedVrata expected_named_vrata{name, expected_vrata};
    CHECK(expected_named_vrata == actual_vrata);

    // 1. Paran start and end should exist.
    REQUIRE(actual_vrata.paran.paran_start.has_value());
    REQUIRE(actual_vrata.paran.paran_end.has_value());

    // 2. Paran *date* must be two days after vrata date.
    auto zoned_paran_start = actual_vrata.paran.paran_start->as_zoned_time(location.time_zone());
    auto local_days_paran_start = date::floor<date::days>(zoned_paran_start.get_local_time());
    CHECK(actual_vrata.date + date::days{2} == local_days_paran_start);

    auto zoned_paran_end = date::make_zoned(location.time_zone(), actual_vrata.paran.paran_end->as_sys_time());
    auto local_days_paran_end = date::floor<date::days>(zoned_paran_end.get_local_time());
    CHECK(actual_vrata.date + date::days{2} == local_days_paran_end);

    // 3. Paran_start must match sunrise
    CHECK(actual_vrata.sunrise3 == actual_vrata.paran.paran_start);

    // 4. Paran_end must be either == dvadashi_end
    // 5. or == 1/5 of daytime (and then paran_limit must be set to dvadashi_end)
    const auto fifth_of_daytime = proportional_time(actual_vrata.sunrise3, actual_vrata.sunset3, 0.2);
    CHECK((actual_vrata.paran.paran_end == actual_vrata.times.trayodashi_start ||
           (actual_vrata.paran.paran_end == fifth_of_daytime && actual_vrata.paran.paran_limit == actual_vrata.times.trayodashi_start)));

}

TEST_CASE("atiriktA-dvAdashI {vena,marsel,madrid,london} 2019-03-01-2 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-02-28/") {
    const date::local_days d{2019_y/February/28};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};
    Vrata v01_atirikta_dvadashi_std_paran{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/March/1}, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Standard}};

    check_atirikta_at_location("vena", vena_coord, v01_atirikta_dvadashi_std_paran, d);
    check_atirikta_at_location("marsel", marsel_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("madrid", madrid_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("london", london_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {aktau, surgut, chelyabinsk, yerevan, tbilisi} 2019-12-07-8 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-12-01/") {
    const date::local_days d{2019_y/December/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/December/7}, Chandra_Masa::Margashirsha, Paksha::Shukla, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("aktau", aktau_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("surgut", surgut_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("chelyabinsk", chelyabinsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("erevan", erevan_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("tbilisi", tbilisi_coord, v01_atirikta_dvadashi, d);
}


TEST_CASE("atiriktA-dvAdashI {madrid,london} 2019-11-07-8 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-11-05/") {
    const date::local_days d{2019_y/November/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/November/7}, Chandra_Masa::Kartika, Paksha::Shukla, Paran{Paran::Type::Standard}};

    check_atirikta_at_location("madrid", madrid_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("london", london_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI meadowlake 2019-10-08-9 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-10-04/") {
    const date::local_days d{2019_y/October/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/October/8}, Chandra_Masa::Ashvin, Paksha::Shukla, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("meadowlake", meadowlake_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {hab,vlad} 2019-09-09-10 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-09-07/") {
    const date::local_days d{2019_y/September/6};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, date::local_days{2019_y/September/9}, Chandra_Masa::Bhadrapada, Paksha::Shukla, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("habarovsk", habarovsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("vladivostok", vladivostok_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-dvAdashI {mirny,hab,vlad} 2019-04-30-01 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-04-27/") {
    const date::local_days d{2019_y/April/30};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Dvadashi, d, Chandra_Masa::Chaitra, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("mirnyy", mirnyy_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("habarovsk", habarovsk_coord, v01_atirikta_dvadashi, d);
    check_atirikta_at_location("vladivostok", vladivostok_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("atiriktA-ekAdashI {ppk} 2019-04-30-01 https://tatvavadi.ru/pa,.nchaa,ngam/posts/2019-04-27/") {
    const date::local_days d{2019_y/April/30};
    Vrata v01_atirikta_dvadashi{Vrata_Type::With_Atirikta_Ekadashi, d, Chandra_Masa::Chaitra, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};

    check_atirikta_at_location("petropavlovskkamchatskiy", petropavlovskkamchatskiy_coord, v01_atirikta_dvadashi, d);
}

TEST_CASE("Ekadashi 2019-03-17") {
    const date::local_days d{2019_y/March/15};
    Vrata v17{date::local_days{2019_y/March/17}, Chandra_Masa::Phalguna, Paksha::Shukla};
    Expected_Vrata v17_paran_before{
        Vrata_Type::Ekadashi,
        2019_y/March/17,
        Paran{Paran::Type::Puccha_Dvadashi, std::nullopt, JulDays_UT{2019_y/March/18, 12h + 13min + 36.459301s}},
        Chandra_Masa::Phalguna,
        Paksha::Shukla};
    Vrata v18{Vrata_Type::Ekadashi, date::local_days{2019_y/March/18}, Chandra_Masa::Phalguna, Paksha::Shukla};
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
    REQUIRE(v18 == vrata(Calc{petropavlovskkamchatskiy_coord}, d)); // 55gh_55vigh (hrasva) < dashamI end
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
    REQUIRE(v17 == vrata(Calc{jurmala_coord}, d));
    REQUIRE(v17 == vrata(Calc{tallin_coord}, d));
    REQUIRE(v17 == vrata(Calc{vilnyus_coord}, d));
    REQUIRE(v17 == vrata(Calc{varshava_coord}, d));
    REQUIRE(v17 == vrata(Calc{vena_coord}, d));
    REQUIRE(v17 == vrata(Calc{marsel_coord}, d));
    REQUIRE(v17 == vrata(Calc{madrid_coord}, d));
    REQUIRE(v17 == vrata(Calc{london_coord}, d));
    REQUIRE(v17_paran_before == vrata(Calc{fredericton_coord}, d));
    REQUIRE(v17_paran_before == vrata(Calc{toronto_coord}, d));
    REQUIRE(v17_paran_before == vrata(Calc{miami_coord}, d));
    Vrata v_meadowlake = vrata(Calc{meadowlake_coord}, d);
    REQUIRE(v17 == v_meadowlake);
    REQUIRE((v_meadowlake.paran.paran_start && v_meadowlake.paran.paran_end));
    REQUIRE(*v_meadowlake.paran.paran_start < *v_meadowlake.paran.paran_end);
}

TEST_CASE("find_tithi_start gives what we expect (close to the target tithi)") {
    JulDays_UT from{2019_y/March/17};
    Tithi expected_tithi{Tithi::Dvadashi_End()};
    Calc calc{fredericton_coord};

    auto actual_time = calc.find_either_tithi_start(from, expected_tithi);

    Tithi actual_tithi = calc.swe.get_tithi(actual_time);
    REQUIRE(expected_tithi == actual_tithi);
}

auto get_next_tithi_wrapper(Calc const & calc, JulDays_UT from, Tithi tithi) {
    return calc.find_either_tithi_start(from, tithi);
}

auto get_next_tithi_wrapper(const Location & coord, JulDays_UT from, Tithi tithi) {
    Calc calc{coord};
    return get_next_tithi_wrapper(calc, from, tithi);
}

TEST_CASE("find_tithi_start breaks out from eternal loop") {
    JulDays_UT from{2019_y/April/29, double_hours{2.0411111153662205}};
    (void) get_next_tithi_wrapper(london_coord, from, Tithi::Ekadashi());
}

TEST_CASE("get_next_tithi() returns Shukla Ekadashi after Shukla something tithi") {
    Calc const calc{london_coord};
    JulDays_UT const from{2019_y/May/12};
    auto actual_time = get_next_tithi_wrapper(calc, from, Tithi::Ekadashi());
    auto actual_tithi = calc.swe.get_tithi(actual_time);
    REQUIRE(actual_tithi.tithi == Approx{Tithi::Ekadashi().tithi});
    REQUIRE((actual_time - from) <= double_days{14});
}

TEST_CASE("get_next_tithi() returns Krishna Ekadashi after Krishna something tithi") {
    Calc const calc{london_coord};
    JulDays_UT const from{2019_y/April/25};
    auto actual_time = get_next_tithi_wrapper(calc, from, Tithi::Ekadashi());
    auto actual_tithi = calc.swe.get_tithi(actual_time);
    REQUIRE(actual_tithi.tithi-15.0 == Approx(Tithi::Ekadashi().tithi));
    REQUIRE((actual_time - from) <= double_days{14});
}

TEST_CASE("get_next_tithi() gives closest Ekadashi tithi for petropavlovsk after 2019-03-15") {
    Calc const calc{petropavlovskkamchatskiy_coord};
    JulDays_UT const from{2019_y/March/15};
    auto actual_time = get_next_tithi_wrapper(calc, from, Tithi::Ekadashi());
    auto actual_tithi = calc.swe.get_tithi(actual_time);
    REQUIRE(actual_tithi.tithi == Approx(Tithi::Ekadashi().tithi));
    REQUIRE((actual_time - from) <= double_days{14});
}

TEST_CASE("paran not earlier than quarter of Dvadashi tithi") {
    Calc const calc{kiev_coord};
    date::local_days const from{2019_y/August/24};
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
    date::local_days const from{2019_y/November/20};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata.has_value());
}

TEST_CASE("Udupi after 2020-02-06 does NOT return 2020-02-06") {
    Calc const calc{udupi_coord};
    date::local_days const from{2020_y/February/6};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata->date != from);
}

TEST_CASE("Kiev after 2020-01-21 does NOT return 2020-01-21") {
    Calc const calc{kiev_coord};
    date::local_days const from{2020_y/January/21};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata->date != from);
}

TEST_CASE("get_astronomical_midnight returns time *before* utc midnight for location to the east from greenwich") {
    vp::Calc c{moskva_coord};
    const date::local_days date{2019_y/January/1};

    REQUIRE(c.calc_astronomical_midnight(date) < vp::JulDays_UT{date});
}

TEST_CASE("get_astronomical_midnight returns time *after* utc midnight for location to the west from greenwich") {
    vp::Calc c{losanjeles_coord};
    const date::local_days date{2019_y/January/1};

    REQUIRE(c.calc_astronomical_midnight(date) > vp::JulDays_UT{date});
}

TEST_CASE("get_astronomical_midnight() adjusts to the right side") {
    Calc c{petropavlovskkamchatskiy_coord};
    const date::local_days date{2019_y/March/18};

    auto local_midnight = c.calc_astronomical_midnight(date);
    JulDays_UT local_midnight_in_utc_earliest{2019_y/March/17};
    JulDays_UT local_midnight_in_utc_latest{2019_y/March/18};
    REQUIRE(local_midnight > local_midnight_in_utc_earliest);
    REQUIRE(local_midnight < local_midnight_in_utc_latest);
}

TEST_CASE("Surgut 2019-12-07 gets paranam time +2days after atiriktA as it should") {
    auto vrata = Calc{surgut_coord}.find_next_vrata(date::local_days{2019_y/December/7});
    REQUIRE(vrata.has_value());
    auto expected_ymd = 2019_y/December/9;
    date::year_month_day local_ymd{date::floor<date::days>(vrata->paran.paran_start->as_zoned_time(surgut_coord.time_zone()).get_local_time())};
    CAPTURE(Vrata_Detail_Printer{*vrata});
    REQUIRE(expected_ymd == local_ymd);
}

TEST_CASE("after atiriktA when 1/5 of day fits before end of dvAdashI, pAraNam must be standard, not 'pucchA-dvAdashI'") {
    auto vrata = Calc{aktau_coord}.find_next_vrata(date::local_days{2018_y/August/21});
    REQUIRE(vrata.has_value());
    REQUIRE(date::year_month_day{vrata->date} == 2018_y/August/21);
    CAPTURE(Vrata_Detail_Printer{*vrata});
    REQUIRE(vrata->paran.type == Paran::Type::Standard);
    REQUIRE(vrata->paran.paran_limit.has_value());
    REQUIRE(vrata->paran.paran_limit > vrata->paran.paran_end);
}

double_days operator ""_hms(const char *s, const std::size_t size) {
    if (size != (2 + 1 + 2 + 1 + 2 + 1 + 6)) {
        throw std::length_error("expected format: hh:mm:ss.ssssss");
    }
    std::istringstream stream{s};
    std::chrono::microseconds h_m_s;
    stream >> date::parse("%H:%M:%S", h_m_s);
    return h_m_s;
}

TEST_CASE("ativRddhAdi gives correct sunset, sunris and four time points") {
    using namespace std::chrono_literals;
    Calc calc{kiev_coord};
    auto vrata = calc.find_next_vrata(date::local_days{2020_y/May/17});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->sunset0                     == ApproximateJulDays_UT{2020_y/May/17, "17:36:09.669959"_hms});
    REQUIRE(vrata->sunrise1                    == ApproximateJulDays_UT{2020_y/May/18, "02:12:02.827153"_hms});
    REQUIRE(vrata->times.ativrddha_54gh_40vigh == ApproximateJulDays_UT{2020_y/May/18, "00:40:20.043643"_hms});
    REQUIRE(vrata->times.vrddha_55gh           == ApproximateJulDays_UT{2020_y/May/18, "00:46:03.967627"_hms});
    REQUIRE(vrata->times.samyam_55gh_50vigh    == ApproximateJulDays_UT{2020_y/May/18, "01:00:23.777568"_hms});
    REQUIRE(vrata->times.hrasva_55gh_55vigh    == ApproximateJulDays_UT{2020_y/May/18, "01:01:49.758554"_hms});
}

TEST_CASE("ativRddhAdi gives dashamI ekAdashI, dvAdashI and trayodashI start") {
    Calc calc{kiev_coord};
    auto vrata = calc.find_next_vrata(date::local_days{2020_y/May/17});
    REQUIRE(vrata.has_value());

    REQUIRE(ApproximateJulDays_UT{vrata->times.dashami_start}    == JulDays_UT{2020_y/May/16, "04:53:25.843109"_hms});
    REQUIRE(ApproximateJulDays_UT{vrata->times.ekadashi_start}   == JulDays_UT{2020_y/May/17, "07:12:45.607582"_hms});
    REQUIRE(ApproximateJulDays_UT{vrata->times.dvadashi_start}   == JulDays_UT{2020_y/May/18, "09:39:01.295689"_hms});
    REQUIRE(ApproximateJulDays_UT{vrata->times.trayodashi_start} == JulDays_UT{2020_y/May/19, "12:01:51.058179"_hms});
}

TEST_CASE("ativRddhAdi gives expected dashamI ekAdashI and dvAdashI length") {
    Calc calc{kiev_coord};
    auto vrata = calc.find_next_vrata(date::local_days{2020_y/May/17});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->times.dashami_length().count() == Approx{65.805});
    REQUIRE(vrata->times.ekadashi_length().count() == Approx{66.094});
    REQUIRE(vrata->times.dvadashi_length().count() == Approx{65.951});
}

TEST_CASE("ativRddhAdi gives expected status for known cases (samyam)") {
    auto vrata = Calc{kiev_coord}.find_next_vrata(date::local_days{2020_y/May/17});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->times.ativrddhaadi() == Vrata_Time_Points::Ativrddhaadi::samyam);
}

TEST_CASE("ativRddhAdi gives expected status for known cases (hrasva)") {
    auto vrata = Calc{kiev_coord}.find_next_vrata(date::local_days{2020_y/May/25});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->times.ativrddhaadi() == Vrata_Time_Points::Ativrddhaadi::hrasva);
}

TEST_CASE("ativRddhAdi gives expected status for known cases (vRddha)") {
    auto vrata = Calc{kiev_coord}.find_next_vrata(date::local_days{2020_y/August/25});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->times.ativrddhaadi() == Vrata_Time_Points::Ativrddhaadi::vrddha);
}

TEST_CASE("ativRddhatvam gives relevant timpoint to be checked for being 'dashamI-free'") {
    auto vrata = Calc{kiev_coord}.find_next_vrata(date::local_days{2020_y/August/25});
    REQUIRE(vrata.has_value());

    REQUIRE(vrata->times.ativrddhaditvam_timepoint() == ApproximateJulDays_UT{2020_y/August/28, "01:27:50.835060"_hms});
    REQUIRE(vrata->times.ativrddhaditvam_timepoint() == vrata->times.vrddha_55gh);
}

TEST_CASE("ensure we get an error when we search for ekAdashI in Murmansk in the summer (with no sunset)") {
    const date::local_days date{2020_y/June/3};
    auto vrata = Calc{murmansk_coord}.find_next_vrata(date);
    REQUIRE_FALSE(vrata.has_value());
    const auto error = vrata.error();
    REQUIRE(std::holds_alternative<CantFindSunriseAfter>(error));
    auto e = std::get<CantFindSunriseAfter>(error);
    REQUIRE(e.after < JulDays_UT{date});
    REQUIRE(e.after > JulDays_UT{date - date::days{4}});
}

TEST_CASE("we get nearest next ekadashi start for petropavlovsk 2019-03-18") {
    auto vrata = Calc{petropavlovskkamchatskiy_coord}.find_next_vrata(date::local_days{2019_y/March/18});
    REQUIRE(vrata.has_value());
    JulDays_UT ekadashi_earliest{2019_y/March/16};
    JulDays_UT ekadashi_latest{2019_y/March/17};
    REQUIRE(vrata->times.ekadashi_start > ekadashi_earliest);
    REQUIRE(vrata->times.ekadashi_start < ekadashi_latest);
}

TEST_CASE("arunodaya0 and sunrise0 are from the same morning in the dAshaMi viddha case for Mirny 2020-07-30") {
    auto vrata = Calc{mirnyy_coord}.find_next_vrata(date::local_days{2020_y/July/30});
    CAPTURE(vrata->sunrise0);
    REQUIRE(vrata->sunrise0.has_value());
    JulDays_Zoned local_sunrise{vrata->location.time_zone(), *vrata->sunrise0};
    const auto timezone = vrata->location.time_zone();
    const auto sunrise0 = vrata->sunrise0->as_zoned_time(timezone);
    const auto min_acceptable_arunodaya0 = (*vrata->sunrise0 - 3h).as_zoned_time(timezone);
    const auto max_acceptable_arunodaya0 = (*vrata->sunrise0 - 30min).as_zoned_time(timezone);
    const auto arunodaya0 = vrata->times.arunodaya.as_zoned_time(timezone);
    CAPTURE(arunodaya0);
    CAPTURE(min_acceptable_arunodaya0);
    CAPTURE(max_acceptable_arunodaya0);
    CAPTURE(sunrise0);
    CHECK(arunodaya0 >= min_acceptable_arunodaya0);
    CHECK(arunodaya0 <= max_acceptable_arunodaya0);
}

TEST_CASE("paran after 1/4 of dvAdashI: don't specify end time when interval is short") {
    auto vrata = Calc{gokarna_coord}.find_next_vrata(date::local_days{2020_y/September/13});
    REQUIRE(vrata);
    REQUIRE(!vrata->paran.paran_end);
}

TEST_CASE("paran after 1/4 of dvAdashI: don't specify end time even when interval is long enough") {
    auto vrata = Calc{erevan_coord}.find_next_vrata(date::local_days{2020_y/September/13});
    REQUIRE(vrata);
    REQUIRE(!vrata->paran.paran_end);
}

namespace {
class EqualsRoundedToMinuteMatcher : public Catch::MatcherBase<vp::JulDays_UT> {
    date::sys_seconds m_time;
    std::chrono::seconds m_timezone_offset;
public:
    EqualsRoundedToMinuteMatcher(date::year_month_day date, std::chrono::seconds time, std::chrono::seconds timezone_offset)
        : m_time(date::sys_days{date} + time - timezone_offset), m_timezone_offset{timezone_offset} {}
    bool match(const vp::JulDays_UT & what) const override {
        return what.round_to_minute() == m_time;
    }

    std::string describe() const override {
        auto local = m_time + m_timezone_offset;
        return fmt::format(FMT_STRING("rounded to minute, equals to {} (local {})"), date::format("%Y-%m-%d %H:%M", m_time), date::format("%Y-%m-%d %H:%M", local));
    }
};

EqualsRoundedToMinuteMatcher EqualsRoundedToMinute(date::year_month_day date, std::chrono::seconds time, std::chrono::seconds timezone_offset) {
    return {date, time, timezone_offset};
}
}

TEST_CASE("find_nakshatra_start() works for simple cases from 2020-11 Palimaru panchangam") {
    Location udupi{28'39'00_N,  77'13'00_E};
    const auto timezone_offset = 5h + 30min;
    auto calc = Calc{udupi};

    struct test_data {
        date::year_month_day date;
        std::chrono::seconds time;
        Nakshatra nakshatra;
    };

    auto data = GENERATE(
        test_data{2020_y/11/1, 20h+57min, vp::Nakshatra::BHARANI_END()},
        test_data{2020_y/11/2, 23h+50min, vp::Nakshatra::KRITIKA_END()},
        test_data{2020_y/11/4, 2h+30min, vp::Nakshatra::ROHINI_END()},
        test_data{2020_y/11/5, 4h+51min, vp::Nakshatra::MRGASHIRSHA_END()},
        test_data{2020_y/11/6, 6h+45min, vp::Nakshatra::ARDRA_END()},
        test_data{2020_y/11/7, 8h+5min, vp::Nakshatra::PUNARVASU_END()},
        test_data{2020_y/11/8, 8h+45min, vp::Nakshatra::PUSHYA_END()}
        );

    auto from_date = vp::JulDays_UT{date::year_month_day{date::sys_days{data.date} - date::days{1}}};
    auto time = calc.find_nakshatra_start(from_date, data.nakshatra);
    REQUIRE_THAT(time, EqualsRoundedToMinute(data.date, data.time, timezone_offset));
}

TEST_CASE("find_exact_tithi_start() works for second paksha (tithis >= 15)") {
    auto calc = Calc{sample_location};
    const auto start_time = JulDays_UT{2020_y/November/20};
    const auto time = calc.find_exact_tithi_start(start_time, Tithi::Krishna_Saptami());

    // shukla saptami is very close (within 2 days) of start time. So ensure we found next saptami
    REQUIRE(time - start_time >= double_days{10});
    REQUIRE(calc.swe.get_tithi(time).tithi == Approx(Tithi::Krishna_Saptami().tithi));
}

TEST_CASE("saura_masa() works for simple cases") {
    auto calc = Calc{sample_location};
    REQUIRE(calc.saura_masa(JulDays_UT{2020_y/4/13}) == Saura_Masa::Mina);
    REQUIRE(calc.saura_masa(JulDays_UT{2020_y/4/15}) == Saura_Masa::Mesha);
}

TEST_CASE("chandra_masa() works for simple cases") {
    auto calc = Calc{sample_location};
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2020_y/11/21}) == Chandra_Masa::Kartika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2020_y/12/1}) == Chandra_Masa::Kartika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2020_y/12/16}) == Chandra_Masa::Margashirsha);
}

TEST_CASE("find_sankranti returns reasonable times") {
    auto calc = Calc{sample_location};
    auto actual = calc.find_sankranti(JulDays_UT{2020_y/11/21}, Saura_Masa::Dhanu);
    REQUIRE(actual >= JulDays_UT{2020_y/12/15});
    REQUIRE(actual <= JulDays_UT{2020_y/12/16});
}

TEST_CASE("Saura_Masa + operator works") {
    REQUIRE(Saura_Masa::Dhanu + 1 == Saura_Masa::Makara);
    REQUIRE(Saura_Masa::Dhanu + (-1) == Saura_Masa::Vrishchika);
    REQUIRE(Saura_Masa::Mina + 1 == Saura_Masa::Mesha);
    REQUIRE(Saura_Masa::Mesha + (-1) == Saura_Masa::Mina);
    REQUIRE(Saura_Masa::Mesha + (-1) == Saura_Masa::Mina);
    REQUIRE(Saura_Masa::Mina + 25 == Saura_Masa::Mesha);
    REQUIRE(Saura_Masa::Mina + (-11) == Saura_Masa::Mesha);
    REQUIRE(Saura_Masa::Mina + (-23) == Saura_Masa::Mesha);
}

TEST_CASE("chandra_masa_amanta() works for known adhika māsa cases") {
    auto calc = Calc{sample_location};
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2023_y/7/20}) == Chandra_Masa::Adhika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2020_y/9/18}) == Chandra_Masa::Adhika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2018_y/5/18}) == Chandra_Masa::Adhika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2015_y/6/25}) == Chandra_Masa::Adhika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2012_y/8/19}) == Chandra_Masa::Adhika);
}

TEST_CASE("chandra_masa_amanta() works for known kshaya māsa cases") {
    auto calc = Calc{sample_location};
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{1963_y/10/23}) == Chandra_Masa::Adhika);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{1963_y/11/23}) == Chandra_Masa::Kshaya);

    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{1983_y/1/23}) == Chandra_Masa::Kshaya);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{1983_y/2/23}) == Chandra_Masa::Adhika);

    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2123_y/12/23}) == Chandra_Masa::Kshaya);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2124_y/3/23}) == Chandra_Masa::Adhika);

    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2284_y/1/23}) == Chandra_Masa::Kshaya);
    REQUIRE(calc.chandra_masa_amanta(JulDays_UT{2284_y/3/23}) == Chandra_Masa::Adhika);
}

TEST_CASE("vrata::ekadashi_name() works for few known cases") {
    auto calc = Calc{udupi_coord};
    auto check = [&](date::year_month_day date, const char * expected_name) {
        SECTION(fmt::format(FMT_STRING("{} ekādaśī on {}"), expected_name, date)) {
            auto vrata = calc.find_next_vrata(date::local_days{date});
            REQUIRE(vrata.has_value());
            REQUIRE(date == date::year_month_day{vrata->date});
            REQUIRE(expected_name == vrata->ekadashi_name());
        }
    };
    // data from Puttige maṭha panchāṅgam 2020-21
    check(2020_y/4/4, "Kāmadā");
    check(2020_y/4/18, "Varūthinī");
    check(2020_y/5/4, "Mohinī");
    check(2020_y/5/18, "Aparā");
    check(2020_y/6/2, "Nirjalā");
    check(2020_y/6/17, "Yoginī");
    check(2020_y/7/1, "Śayanī");
    check(2020_y/7/16, "Kāmikā");
    check(2020_y/7/30, "Pavitrā");
    check(2020_y/8/15, "Ajā");
    check(2020_y/8/29, "Pārśva-parivartinī");
    check(2020_y/9/13, "Indirā");
    check(2020_y/9/27, "Kamalā");
    check(2020_y/10/13, "Padmā");
    check(2020_y/10/27, "Pāśāṅkuśā");
    check(2020_y/11/11, "Ramā");
    check(2020_y/11/25, "Prabodhinī"); // TODO: why our calculations show atiriktā, but Puttige shows one-day ekādaśī on 26th?
    check(2020_y/12/11, "Utpattikā");
    check(2020_y/12/25, "Mokṣadā");
    check(2021_y/1/9, "Saphalā");
    check(2021_y/1/24, "Putradā");
    check(2021_y/2/8, "Ṣaṭ-tilā");
    check(2021_y/2/23, "Jayā");
    check(2021_y/3/9, "Vijayā");
    check(2021_y/3/25, "Āmalakī");
    check(2021_y/4/7, "Pāpamocanī");
}

TEST_CASE("shravana dvadashi") {
    SECTION("Udupi 2020-03-19..20: Ekādaśī + Śravaṇa-dvādaśī next day") {
        Vrata v{Vrata_Type::With_Shravana_Dvadashi_Next_Day, date::local_days{2020_y/3/19}, Chandra_Masa::Phalguna, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};
        REQUIRE(v == vrata(Calc{udupi_coord}, date::local_days{2020_y/3/19}));
        REQUIRE(is_atirikta(v.type));
    }

    SECTION("Fredericton, Toronto, Meadow Lake : 2019-09-09..10") {
        Vrata v{Vrata_Type::With_Shravana_Dvadashi_Next_Day, date::local_days{2019_y/9/9}, Chandra_Masa::Bhadrapada, Paksha::Shukla, Paran{Paran::Type::Standard}};
        REQUIRE(v == vrata(Calc{fredericton_coord}, date::local_days{2019_y/9/9}));
        REQUIRE(v == vrata(Calc{toronto_coord}, date::local_days{2019_y/9/9}));
        REQUIRE(v == vrata(Calc{meadowlake_coord}, date::local_days{2019_y/9/9}));
    }

    SECTION("Cancun 2020-03-19: Ekādaśī without Śravaṇa-dvādaśī") {
        Vrata v{Vrata_Type::Ekadashi, date::local_days{2020_y/3/19}, Chandra_Masa::Phalguna, Paksha::Krishna, Paran{Paran::Type::Standard}};
        REQUIRE(v == vrata(Calc{cancun_coord}, date::local_days{2020_y/3/19}));
    }

    SECTION("Tekeli 2020-03-20: Ekādaśī + Śravaṇa-dvādaśī same day") {
        Vrata v{Vrata_Type::With_Shravana_Dvadashi_Same_Day, date::local_days{2020_y/3/20}, Chandra_Masa::Phalguna, Paksha::Krishna, Paran{Paran::Type::Puccha_Dvadashi}};
        REQUIRE(v == vrata(Calc{tekeli_coord}, date::local_days{2020_y/3/20}));
    }

    SECTION("Kiev 2021-03-09..10: Śravaṇa-dvādaśī with 12gh+ rule (default), simple Ekādaśī with 14gh+ rule") {
        const date::local_days date{2021_y/3/9};
        Vrata v_12gh_rule{Vrata_Type::With_Shravana_Dvadashi_Next_Day, date, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Standard}};
        Vrata v_14gh_rule{Vrata_Type::Ekadashi, date, Chandra_Masa::Magha, Paksha::Krishna, Paran{Paran::Type::Standard}};
        REQUIRE(v_12gh_rule == vrata(Calc{Swe{kiev_coord, CalcFlags::ShravanaDvadashi12ghPlus}}, date));
        REQUIRE(v_14gh_rule == vrata(Calc{Swe{kiev_coord, CalcFlags::ShravanaDvadashi14ghPlus}}, date));
    }
}

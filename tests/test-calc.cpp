#include <chrono>
#include <condition_variable>
#include <sstream>
#include <thread>

#include "catch.hpp"

#include "calc.h"
#include "date/date.h"
#include "paran.h"
#include "swe.h"
#include "swe_time.h"

using namespace date;

TEST_CASE("find_next_ekadashi_sunrise") {
    Swe_Time start{2019_y, March, 9_d};
    Location coord{50.45, 30.523333};
    auto sunrise = Calc{coord}.find_next_ekadashi_sunrise(start);
    Swe_Time expected{2019_y, March, 17_d, 4, 13, 36.270031};
    REQUIRE(sunrise.has_value());
    REQUIRE(*sunrise == expected);
}

TEST_CASE("Vijaya Ekadashi Kiev 2019") {
    date::year_month_day base_date{2019_y/February/28};
    Location kiev{50.45, 30.523333};
    auto vrata = Calc{kiev}.find_next_vrata(base_date);
    REQUIRE(vrata.has_value());
    REQUIRE(swe::Swe{kiev}.get_tithi(Swe_Time{vrata->date}).get_paksha() == Paksha::Krishna);
    REQUIRE(vrata->type == Vrata_Type::Ekadashi);
    REQUIRE(vrata->date == 2019_y/March/2);
}

TEST_CASE("get_arunodaya") {
    Location kiev{50.45, 30.523333};
    auto arunodaya = Calc{kiev}.get_arunodaya(Swe_Time{2019_y, March, 2_d, 4, 45, 58.052015});
    REQUIRE(arunodaya.has_value());
    REQUIRE(arunodaya->first == Swe_Time{2019_y, March, 2_d, 3, 0, 17.512880});
}

Vrata vrata(const Calc &c, date::year_month_day base_date) {
    return c.find_next_vrata(base_date).value();
}

TEST_CASE("Ekadashi 2019-02-28") {
    date::year_month_day d{2019_y/February/28};
    Vrata v01{2019_y/March/1};
    Vrata v02{2019_y/March/2};
    Vrata sandigdha_mar02{Vrata_Type::Sandigdha_Ekadashi, 2019_y/March/2};
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
    Paran paran1415{Paran::Type::From_Quarter_Dvadashi, Swe_Time{2019_y, March, 2_d, 12, 14, 40.513510}};
    Vrata v01_paran_after_quarter{Vrata_Type::Ekadashi, 2019_y/March/1, paran1415};
    REQUIRE(v01_paran_after_quarter == vrata(Calc{riga_coord}, d));     // > 14:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{yurmala_coord}, d));  // > 14:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{tallin_coord}, d));   // > 14:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{vilnyus_coord}, d));  // > 14:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{varshava_coord}, d)); // > 13:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{vena_coord}, d)); //atirikta dvadashi
    REQUIRE(v01_paran_after_quarter == vrata(Calc{marsel_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_paran_after_quarter == vrata(Calc{madrid_coord}, d)); //atirikta dvadashi   < 9:14
    REQUIRE(v01_paran_after_quarter == vrata(Calc{london_coord}, d)); //atirikta dvadashi   < 8:14
    REQUIRE(v01_paran_after_quarter == vrata(Calc{frederikton_coord}, d));  // > 8:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{toronto_coord}, d));      // > 7:15
    REQUIRE(v01_paran_after_quarter == vrata(Calc{miami_coord}, d));       // > 7:15
    REQUIRE(v01 == vrata(Calc{meadowlake_coord}, d));
}

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
    date::year_month_day d{2019_y/March/15};
    Vrata v17{2019_y/March/17};
    Expected_Vrata v17_paran_before{
        Vrata_Type::Ekadashi,
        2019_y/March/17,
        Paran{Paran::Type::Until_Dvadashi_End, std::nullopt, Swe_Time{2019_y, March, 18_d, 12, 13, 36.459301}}};
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

TEST_CASE("get_next_tithi_start") {
    Swe_Time from{2019_y, March, 17_d};
    Tithi tithi{Tithi::Dvadashi_End};
    Swe_Time expected{2019_y, March, 18_d, 12, 13, 36.459301};
    std::optional<Swe_Time> actual = Calc{frederikton_coord}.get_next_tithi_start(from, tithi);
    REQUIRE(actual.has_value());
    REQUIRE(*actual == expected);
}

auto get_next_tithi_wrapper(Calc const &calc, Swe_Time from, Tithi tithi) {
    std::optional<Swe_Time> retval;

#ifndef HAS_THREADS
#define HAS_THREADS 1
#endif

#if HAS_THREADS
    std::condition_variable cv;
    std::mutex cv_m;
    std::thread thread([&](){
        auto local_retval = calc.get_next_tithi_start(from, tithi);
        {
            std::lock_guard<std::mutex> l{cv_m};
            retval = std::move(local_retval);
            cv.notify_one();
        }
    });
    thread.detach();

    {
        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> l{cv_m};
        if (cv.wait_for(l, 100ms) == std::cv_status::timeout) {
            throw std::runtime_error("Timeout");
        }
    }
#else
    retval = calc.get_next_tithi_start(from, tithi);
#endif
    return retval;
}

auto get_next_tithi_wrapper(Location coord, Swe_Time from, Tithi tithi) {
    Calc calc{coord};
    return get_next_tithi_wrapper(calc, from, tithi);
}

TEST_CASE("get_next_tithi_start breaks out from eternal loop") {
    Swe_Time from{2019_y, April, 29_d, 2.0411111153662205};
    Tithi tithi{Tithi::Ekadashi};
    auto actual = get_next_tithi_wrapper(london_coord, from, tithi);
    REQUIRE(actual.has_value());
}

TEST_CASE("get_next_tithi() returns Shukla Ekadashi after Shukla something tithi") {
    Calc const calc{london_coord};
    Swe_Time const from{2019_y, May, 12_d};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= 14);
}

TEST_CASE("get_next_tithi() returns Krishna Ekadashi after Krishna something tithi") {
    Calc const calc{london_coord};
    Swe_Time const from{2019_y, April, 25_d};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi-15.0 == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= 14);
}

TEST_CASE("get_next_tithi() gives closest Ekadashi tithi for petropavlovsk after 2019-03-15") {
    Calc const calc{petropavlovskkamchatskiy_coord};
    Swe_Time const from{2019_y, March, 15_d};
    Tithi const tithi{Tithi::Ekadashi};
    auto actual_time = get_next_tithi_wrapper(calc, from, tithi);
    REQUIRE(actual_time.has_value());
    auto actual_tithi = calc.swe.get_tithi(*actual_time);
    REQUIRE(actual_tithi.tithi == Approx(Tithi{Tithi::Ekadashi}.tithi));
    REQUIRE((*actual_time - from) <= 14);
}

TEST_CASE("paran not earlier than quarter of Dvadashi tithi") {
    Calc const calc{kiev_coord};
    date::year_month_day const from{2019_y/August/24};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata.has_value());
    REQUIRE(vrata->paran.type == Paran::Type::From_Quarter_Dvadashi);
    REQUIRE(vrata->paran.paran_start.has_value());
    Swe_Time paran_start = vrata->paran.paran_start.value();
    Swe_Time paran_start_not_before{2019_y, August, 27_d, 5.0};
    Swe_Time paran_start_not_after{2019_y, August, 27_d, 5.2};
    REQUIRE(paran_start > paran_start_not_before);
    REQUIRE(paran_start < paran_start_not_after);
}

TEST_CASE("Udupi after 2019-11-20 returns actual date", "[.]") {
    Calc const calc{udupi_coord};
    date::year_month_day const from{2019_y/November/20};
    auto vrata = calc.find_next_vrata(from);
    REQUIRE(vrata.has_value());
}

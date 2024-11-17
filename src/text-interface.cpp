#include "text-interface.h"

#include "calc.h"
#include "nameworthy-dates.h"
#include "vrata_detail_printer.h"

#include <charconv>
#include <cstring>

using namespace vp;

namespace vp::text_ui {

namespace {
constexpr int len(const char * s) { return std::char_traits<char>::length(s); }
static_assert (len("123") == 3);
}

date::year_month_day parse_ymd(const std::string_view s) {
    // YYYY-MM-DD
    // 01234567890
    if (s.length() != len("YYYY-MM-DD")) return {};
    const char * const str = s.data();
    int year{};
    if (auto [p, e] = std::from_chars(str+0, str+len("YYYY"), year); e != std::errc{} || p != str+4)
        return {};
    unsigned int month{};
    if (auto [p, e] = std::from_chars(str+len("YYYY-"), str+len("YYYY-MM"), month); e != std::errc{} || p != str+7)
        return {};
    unsigned int day{};
    if (auto [p, e] = std::from_chars(str+len("YYYY-MM-"), str+len("YYYY-MM-DD"), day); e != std::errc{} || p != str+10)
        return {};
    return date::year{year}/date::month{month}/date::day{day};
}

const std::vector<Location> &LocationDb::locations() {
    static std::vector<Location> locations_ {
        { udupi_coord },                    // 13.3408_N,  74.7517_E, UTC+05:30 India (Asia/Kolkata)
        { kalkuta_coord },                  // 22.5333_N,  88.3667_E, UTC+05:30 India (Asia/Kolkata)
        { manali_coord },                   // 32.2667_N,  77.1667_E, UTC+05:30 India (Asia/Kolkata)
        { newdelhi_coord },                 // 28.6500_N,  77.2167_E, UTC+05:30 India (Asia/Kolkata)
        { bengaluru_coord },                // 12.9667_N,  77.5667_E, UTC+05:30 India (Asia/Kolkata)
        { gokarna_coord },                  // 14.5500_N,  74.3167_E, UTC+05:30 India (Asia/Kolkata)
        { morjim_coord },                   // 15.6294_N,  73.7358_E, UTC+05:30 India (Asia/Kolkata)
        { dushanbe_coord },                 // 38.5367_N,  68.7800_E, UTC+05:00 Tajikistan (Asia/Dushanbe)
        { perm_coord },                     // 58.0000_N,  56.2333_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { ufa_coord },                      // 54.7333_N,  55.9667_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { orenburg_coord },                 // 51.7667_N,  55.1000_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { ekaterinburg_coord },             // 56.8333_N,  60.5833_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { surgut_coord },                   // 61.2500_N,  73.4333_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { chelyabinsk_coord },              // 55.1622_N,  61.4031_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { aktobe_coord },                   // 50.3000_N,  57.1667_E, UTC+05:00 Kazakhstan (Asia/Aqtobe)
        { aktau_coord },                    // 43.6500_N,  51.1500_E, UTC+05:00 Kazakhstan (Asia/Aqtau)
        { kokshetau_coord },                // 53.2917_N,  69.3917_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { almaata_coord },                  // 43.2500_N,  76.9000_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { tekeli_coord },                   // 44.8631_N,  78.7642_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { ustkamenogorsk_coord },           // 49.9833_N,  82.6167_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { bishkek_coord },                  // 42.8667_N,  74.5667_E, UTC+06:00 Kyrgyzstan (Asia/Bishkek)
        { omsk_coord },                     // 54.9667_N,  73.3833_E, UTC+06:00 Russia (Asia/Omsk)
        { krasnoyarsk_coord },              // 56.0119_N,  92.8714_E, UTC+07:00 Russia (Asia/Krasnoyarsk)
        { novokuznetsk_coord },             // 53.7333_N,  87.0833_E, UTC+07:00 Russia (Asia/Krasnoyarsk)
        { novosibirsk_coord },              // 55.0167_N,  82.9167_E, UTC+07:00 Russia (Asia/Novosibirsk)
        { barnaul_coord },                  // 53.3500_N,  83.7667_E, UTC+07:00 Russia (Asia/Barnaul)
        { tomsk_coord },                    // 56.4833_N,  84.9500_E, UTC+07:00 Russia (Asia/Tomsk)
        { kophangan_coord },                //  9.7167_N, 100.0000_E, UTC+07:00 Thailand (Asia/Bangkok)
        { denpasar_coord },                 //  8.6500_S, 115.2167_E, UTC+08:00 Indonesia (Asia/Makassar)
        { mirnyy_coord },                   // 62.5333_N, 113.9500_E, UTC+09:00 Russia (Asia/Yakutsk)
        { habarovsk_coord },                // 48.4833_N, 135.0833_E, UTC+10:00 Russia (Asia/Vladivostok)
        { vladivostok_coord },              // 43.1167_N, 131.9000_E, UTC+10:00 Russia (Asia/Vladivostok)
        { petropavlovskkamchatskiy_coord }, // 53.0167_N, 158.6500_E, UTC+12:00 Russia (Asia/Kamchatka)
        { erevan_coord },                   // 40.1833_N,  44.5167_E, UTC+04/05 Armenia (Asia/Yerevan)
        { tbilisi_coord },                  // 41.7167_N,  44.7833_E, UTC+04:00 Georgia (Asia/Tbilisi)
        { samara_coord },                   // 53.1833_N,  50.1167_E, UTC+04:00 Russia (Europe/Samara)
        { volgograd_coord },                // 48.6992_N,  44.4733_E, UTC+03:00 Russia (Europe/Volgograd)
        { ulyanovsk_coord },                // 54.3167_N,  48.3667_E, UTC+04:00 Russia (Europe/Ulyanovsk)
        { pyatigorsk_coord },               // 44.0333_N,  43.0500_E, UTC+03:00 Russia (Europe/Moscow)
        { stavropol_coord },                // 45.0333_N,  41.9667_E, UTC+03:00 Russia (Europe/Moscow)
        { semikarakorsk_coord },            // 47.5167_N,  40.8000_E, UTC+03:00 Russia (Europe/Moscow)
        { krasnodar_coord },                // 45.0333_N,  38.9833_E, UTC+03:00 Russia (Europe/Moscow)
        { kerch_coord },                    // 45.3386_N,  36.4681_E, UTC+03:00 Russia (Europe/Simferopol)
        { simferopol_coord },               // 44.9333_N,  34.1000_E, UTC+03:00 Russia (Europe/Simferopol)
        { donetsk_coord },                  // 48.0000_N,  37.8000_E, UTC+03:00 DNR (Europe/Moscow)
        { staryyoskol_coord },              // 51.2833_N,  37.8333_E, UTC+03:00 Russia (Europe/Moscow)
        { lipetsk_coord },                  // 52.6167_N,  39.6000_E, UTC+03:00 Russia (Europe/Moscow)
        { voronezh_coord },                 // 51.6667_N,  39.2000_E, UTC+03:00 Russia (Europe/Moscow)
        { tambov_coord },                   // 52.7167_N,  41.4333_E, UTC+03:00 Russia (Europe/Moscow)
        { kazan_coord },                    // 55.7833_N,  49.1000_E, UTC+03:00 Russia (Europe/Moscow)
        { cheboksary_coord },               // 56.1167_N,  47.2333_E, UTC+03:00 Russia (Europe/Moscow)
        { kirov_coord },                    // 58.6000_N,  49.6500_E, UTC+03:00 Russia (Europe/Kirov)
        { rostov_na_donu_coord },           // 47.2406_N,  39.7106_E, UTC+03:00 Russia (Europe/Moscow)
        { ryazan_coord },                   // 54.6167_N,  39.7167_E, UTC+03:00 Russia (Europe/Moscow)
        { moskva_coord },                   // 55.7500_N,  37.5833_E, UTC+03:00 Russia (Europe/Moscow)
        { dubna_coord },                    // 56.7333_N,  37.1667_E, UTC+03:00 Russia (Europe/Moscow)
        { velikiy_novgorod_coord },         // 58.5250_N,  31.2750_E, UTC+03:00 Russia (Europe/Moscow)
        { spb_coord },                      // 59.9167_N,  30.2500_E, UTC+03:00 Russia (Europe/Moscow)
        { murmansk_coord },                 // 68.9667_N,  33.0833_E, UTC+03:00 Russia (Europe/Moscow)
        { kostomuksha_coord },              // 64.5833_N,  30.6000_E, UTC+03:00 Russia (Europe/Moscow)
        { arhangelsk_coord },               // 64.5500_N,  40.5333_E, UTC+03:00 Russia (Europe/Moscow)
        { smolensk_coord },                 // 54.7667_N,  32.0333_E, UTC+03:00 Russia (Europe/Moscow)
        { gomel_coord },                    // 52.4333_N,  30.9833_E, UTC+03:00 Belarus (Europe/Minsk)
        { minsk_coord },                    // 53.9167_N,  27.5833_E, UTC+03:00 Belarus (Europe/Minsk)
        { grodno_coord },                   // 53.6667_N,  23.8167_E, UTC+03:00 Belarus (Europe/Minsk)
        { harkov_coord },                   // 50.0000_N,  36.2500_E, UTC+02/03 Ukraine (Europe/Kiev)
        { poltava_coord },                  // 49.5667_N,  34.5667_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kremenchug_coord },               // 49.0667_N,  33.4167_E, UTC+02/03 Ukraine (Europe/Kiev)
        { krivoyrog_coord },                // 47.9500_N,  33.4167_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kiev_coord },                     // 50.4500_N,  30.5233_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kropyvnytskyi_coord },            // 48.5000_N,  32.2667_E, UTC+02/03 Ukraine (Europe/Kiev)
        { nikolaev_coord },                 // 46.9667_N,  32.0000_E, UTC+02/03 Ukraine (Europe/Kiev)
        { odessa_coord },                   // 46.4667_N,  30.7333_E, UTC+02/03 Ukraine (Europe/Kiev)
        { rovno_coord },                    // 50.6197_N,  26.2514_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kolomyya_coord },                 // 48.5167_N,  25.0333_E, UTC+02/03 Ukraine (Europe/Kiev)
        { ashkelon_coord },                 // 31.6658_N,  34.5664_E, UTC+02/03 Israel (Asia/Jerusalem)
        { kishinev_coord },                 // 47.0000_N,  28.8500_E, UTC+02/03 Moldova (Europe/Chisinau)
        { nicosia_coord },                  // 35.1725_N,  33.3650_E, UTC+02/03 Cyprus (Asia/Nicosia)
        { riga_coord },                     // 56.9333_N,  24.1000_E, UTC+02/03 Latvia (Europe/Riga)
        { jurmala_coord },                  // 56.9667_N,  23.7833_E, UTC+02/03 Latvia (Europe/Riga)
        { tallin_coord },                   // 59.4333_N,  24.7500_E, UTC+02/03 Estonia (Europe/Tallinn)
        { vilnyus_coord },                  // 54.6833_N,  25.2833_E, UTC+02/03 Lithuania (Europe/Vilnius)
        { varshava_coord },                 // 52.2167_N,  21.0333_E, UTC+01/02 Poland (Europe/Warsaw)
        { vena_coord },                     // 48.2000_N,  16.3667_E, UTC+01/02 Austria (Europe/Vienna)
        { hitzacker_coord },                // 53.1333_N,  11.0500_E, UTC+01/02 Germany (Europe/Berlin)
        { dillingen_an_der_donau_coord },   // 48.5667_N,  10.4667_E, UTC+01/02 Germany (Europe/Berlin)
        { eppingen_coord },                 // 49.1333_N,   8.9167_E, UTC+01/02 Germany (Europe/Berlin)
        { biel_bienne_coord },              // 47.1372_N,   7.2472_E, UTC+01/02 Switzerland (Europe/Zurich)
        { marsel_coord },                   // 43.2964_N,   5.3700_E, UTC+01/02 France (Europe/Paris)
        { barcelona_coord },                // 41.3833_N,   2.1833_E, UTC+01/02 Spain (Europe/Madrid)
        { madrid_coord },                   // 40.4000_N,   3.6833_W, UTC+01/02 Spain (Europe/Madrid)
        { ceuta_coord },                    // 35.8867_N,   5.3000_W, UTC+01/02 Spain (Europe/Madrid)
        { london_coord },                   // 51.5000_N,   0.1167_W, UTC+00/01 UK (Europe/London)
        { fredericton_coord },              // 45.9500_N,  66.6333_W, UTC-04/03 Canada (America/Moncton)
        { toronto_coord },                  // 42.7167_N,  79.4000_W, UTC-05/04 Canada (America/New_York)
        { newyork_coord },                  // 40.7125_N,  74.0061_W, UTC-05/04 USA (America/New_York)
        { miami_coord },                    // 25.7833_N,  80.2167_W, UTC-05/04 USA (America/New_York)
        { cancun_coord },                   // 21.1606_N,  86.8475_W, UTC-05:00 Mexico (America/Cancun)
        { meadowlake_coord },               // 54.1167_N, 108.4333_W, UTC-06:00 Canada (America/Regina)
        { losanjeles_coord },               // 34.0333_N, 118.2667_W, UTC-08/07 USA (America/Los_Angeles)
//        { tagbilaran_coord },               //  9.6500_N, 123.8500_E, UTC+08/09 Philippines (Asia/Manila)
//        { chita_coord },                    // 52.0333_N, 113.5000_E, UTC+09:00 Russia (Asia/Chita)
//        { pokhara_coord },                  // 28.2500_N,  83.9667_E, UTC+05:45 Nepal (Asia/Kathmandu)
//        { kolombo_coord },                  //  6.9167_N,  79.8333_E, UTC+05:30 SriLanka (Asia/Colombo)
//        { vrindavan_coord },                // 28.6000_N,  77.2000_E, UTC+05:30 India (Asia/Kolkata)
//        { pune_coord },                     // 18.5333_N,  73.8667_E, UTC+05:30 India (Asia/Kolkata)
//        { pernem_coord },                   // 15.7167_N,  73.7969_E, UTC+05:30 India (Asia/Kolkata)
//        { bombey_coord },                   // 18.9667_N,  72.8333_E, UTC+05:30 India (Asia/Kolkata)
//        { panaji_coord },                   // 15.4833_N,  72.8333_E, UTC+05:30 India (Asia/Kolkata)
//        { tashkent_coord },                 // 41.3000_N,  68.2667_E, UTC+06:00 Uzbekistan (Asia/Almaty)
//        { suhum_coord },                    // 43.0000_N,  41.0000_E, UTC+03:00 Abkhazia (Europe/Moscow)
//        { sochi_coord },                    // 43.6000_N,  39.7333_E, UTC+03:00 Russia (Europe/Moscow)
//        { lugansk_coord },                  // 48.5667_N,  39.3333_E, UTC+03:00 LNR (Europe/Moscow)
//        { ramenskoe_m_obl_coord },          // 55.5667_N,  38.2167_E, UTC+03:00 Russia (Europe/Moscow)
//        { telaviv_coord },                  // 32.0667_N,  34.7833_E, UTC+02/03 Israel (Asia/Jerusalem)
//        { novgorod_coord },                 // 58.5500_N,  31.2667_E, UTC+03:00 Russia (Europe/Moscow)
//        { tiraspol_coord },                 // 46.8500_N,  29.6000_E, UTC+02/03 Transnistria (Europe/Chisinau)
//        { vinnitsa_coord },                 // 49.2333_N,  28.4500_E, UTC+02/03 Ukraine (Europe/Kiev)
//        { hmelnitskiy_coord },              // 49.4167_N,  26.9833_E, UTC+02/03 Ukraine (Europe/Kiev)
//        { afiny_coord },                    // 37.9667_N,  23.7167_E, UTC+02/03 Greece (Europe/Athens)
//        { kiel_coord },                     // 54.3167_N,  10.1333_E, UTC+01/02 Germany (Europe/Berlin)
//        { freiburg_coord },                 // 48.0000_N,   7.8667_E, UTC+01/02 Germany (Europe/Paris)
//        { koeln_kkd_coord },                // 50.9364_N,   6.9528_E, UTC+01/02 Germany (Europe/Berlin)
//        { dusseldorf_coord },               // 51.2167_N,   6.7833_E, UTC+01/02 Germany (Europe/Berlin)
//        { manchester_coord },               // 53.5000_N,   2.2167_W, UTC+00/01 UK (Europe/London)
//        { puno_coord },                     // 15.8333_S,  70.0167_W, UTC-05:00 Peru (America/Lima)
//        { washington_coord },               // 38.8833_N,  77.0333_W, UTC-05/04 USA (America/New_York)
//        { mundelein_coord },                // 42.2667_N,  88.0000_W, UTC-06/05 USA (America/Chicago)
//        { edmonton_coord },                 // 53.5333_N, 113.5000_W, UTC-07/06 Canada (America/Edmonton)
//        { sanfrantsisko_coord },            // 37.7667_N, 122.4000_W, UTC-08/07 USA (America/Los_Angeles)
    };
    return locations_;
}

std::optional<Location> LocationDb::find_coord(const char *location_name) {
    auto found = std::find_if(
        std::begin(locations()),
        std::end(locations()),
        [=](const auto & named_coord){
            return named_coord.name == location_name;
        }
    );
    if (found == std::end(locations())) return std::nullopt;
    return *found;
}

namespace {
// try decreasing latitude until we get all necessary sunrises/sunsets
tl::expected<vp::Vrata, vp::CalcError> decrease_latitude_and_find_vrata(date::local_days base_date, const Location & location) {
    auto l = location;
    l.latitude_adjusted = true;
    while (1) {
        l.latitude.latitude -= 1.0;
        auto vrata = Calc{l}.find_next_vrata(base_date);
        // Return if have actually found vrata.
        // Also return if we ran down to low enough latitudes so that it doesn't
        // make sense to decrease it further; just report whatever error we got in that case.
        if (vrata || l.latitude.latitude <= 60.0) return vrata;
    }
}

// Try calculating, return true if resulting date range is small enough (suggesting that it's the same ekAdashI for all locations),
// false otherwise (suggesting that we should repeat calculation with adjusted base_date
bool try_calc_all(date::local_days base_date, vp::VratasForDate & vratas, CalcFlags flags) {
    std::transform(
        LocationDb().begin(),
        LocationDb().end(),
        std::back_inserter(vratas),
        [base_date, flags](const vp::Location & location) {
            return calc_one(base_date, location, flags);
        });
    return vratas.all_from_same_ekadashi();
}

struct CalcSettings {
    date::local_days date;
    vp::CalcFlags flags;
};

bool operator==(const CalcSettings & left, const CalcSettings & right);

namespace {
constexpr std::size_t hash_combine(std::size_t hash1, std::size_t hash2) {
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1<<6) + (hash1>>2));
}
}

struct MyHash {
    std::size_t operator()(const CalcSettings & key) const {
        const auto count = key.date.time_since_epoch().count();
        using type=std::remove_cv_t<decltype(count)>;
        auto hash_date = std::hash<type>{}(count);
        using FlagsT = std::underlying_type_t<vp::CalcFlags>;
        auto hash_flags = std::hash<FlagsT>{}(static_cast<FlagsT>(key.flags));
        return hash_combine(hash_date, hash_flags);
    }
};

std::unordered_map<CalcSettings, vp::VratasForDate, MyHash> cache;

bool operator==(const CalcSettings & left, const CalcSettings & right)
{
    return (left.date == right.date) && (left.flags == right.flags);
}

vp::VratasForDate calc_all(date::local_days base_date, CalcFlags flags)
{
    const auto key = CalcSettings{base_date, flags};
    if (auto found = cache.find(key); found != cache.end()) {
        return found->second;
    }
    vp::VratasForDate vratas;

    if (!try_calc_all(base_date, vratas, flags)) {
        vratas.clear();
        date::local_days adjusted_base_date = base_date - date::days{1};
        try_calc_all(adjusted_base_date, vratas, flags);
    }
    cache[key] = vratas;
    return vratas;
}

// Add other interesting dates to the
void add_nameworthy_dates_for_this_paksha(VratasForDate & vratas, CalcFlags flags) {
    for (auto & vrata : vratas) {
        if (vrata) {
            vrata->dates_for_this_paksha = vp::nameworthy_dates_for_this_paksha(vrata.value(), flags);
        }
    }
}

}

vp::MaybeVrata calc_one(date::local_days base_date, const Location & location, CalcFlags flags) {
    // Use immediately-called lambda to ensure Calc is destroyed before more
    // will be created in decrease_latitude_and_find_vrata()
    auto vrata = [&](){
        return Calc{Swe{location, flags}}.find_next_vrata(base_date);
    }();
    if (vrata) return vrata;

    auto e = vrata.error();
    // if we are in the northern areas and the error is that we can't find sunrise or sunset, then try decreasing latitude until it's OK.
    if ((std::holds_alternative<CantFindSunriseAfter>(e) || std::holds_alternative<CantFindSunsetAfter>(e)) && location.latitude.latitude > 60.0) {
        return decrease_latitude_and_find_vrata(base_date, location);
    }
    // Otherwise return whatever error we've got.
    return vrata;
}

vp::VratasForDate calc(date::year_month_day base_date, std::string location_name, CalcFlags flags)
{
    vp::VratasForDate vratas;
    if (location_name == "all") {
        vratas = calc_all(date::local_days{base_date}, flags);
    } else {
        auto location = LocationDb::find_coord(location_name.c_str());
        if (!location) {
            vratas.push_back(tl::make_unexpected(CantFindLocation{std::move(location_name)}));
        } else {
            vratas.push_back(calc_one(date::local_days{base_date}, *location, flags));
        }
    }
    add_nameworthy_dates_for_this_paksha(vratas, flags);
    return vratas;
}

namespace {
void report_details(const vp::MaybeVrata & vrata, const fmt::appender & out) {
    if (!vrata.has_value()) {
        fmt::format_to(out,
                       "# {}*\n"
                       "Can't find next Ekadashi, sorry.\n"
                       "* Error: {}\n",
                       vrata->location_name(),
                       vrata.error());
    } else {
        Vrata_Detail_Printer vd{*vrata};
        fmt::format_to(out, "{}\n\n", vd);
    }

}
}

// Find next ekAdashI vrata for the named location, report details to the output buffer.
tl::expected<vp::Vrata, vp::CalcError> calc_and_report_one(date::year_month_day base_date, const Location & location, const fmt::appender & out) {
    auto vrata = calc_one(date::local_days{base_date}, location);
    report_details(vrata, out);
    return vrata;
}

tl::expected<vp::Vrata, vp::CalcError> find_calc_and_report_one(date::year_month_day base_date, const char * location_name, const fmt::appender & out) {
    std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        fmt::format_to(out, "Location not found: '{}'\n", location_name);
        return tl::make_unexpected(CantFindLocation{location_name});
    }
    return calc_and_report_one(base_date, *coord, out);
}

namespace {
void daybyday_print_header(date::year_month_day base_date, const Location & coord, const DayByDayInfo & info, const fmt::appender & out)
{
    fmt::format_to(out,
                   "{} {} ({:w})\n",
                   coord.name, base_date, base_date);
    fmt::format_to(out, FMT_STRING("Saura māsa: {}"), info.saura_masa);
    if (info.saura_masa_until) {
        fmt::format_to(out, FMT_STRING(" (until {})"), *info.saura_masa_until);
    }
    fmt::format_to(out, FMT_STRING("\n"));
    fmt::format_to(out, FMT_STRING("Chāndra māsa: {}"), info.chandra_masa);
    if (info.chandra_masa_until) {
        fmt::format_to(out, FMT_STRING(" (until {})"), *info.chandra_masa_until);
    }
    fmt::format_to(out, FMT_STRING("\n"));
    auto tz = info.location.time_zone();
    if (info.sunrise1) {
        auto sunrise_date = date::floor<date::days>(date::make_zoned(tz, info.sunrise1->as_sys_time()).get_local_time());
        if (info.tithi_until) {
            const auto tithi_until = date::make_zoned(tz, info.tithi_until->round_to_minute());
            auto tithi_until_date{date::floor<date::days>(tithi_until.get_local_time())};
            fmt::format_to(out,
                           FMT_STRING("{} until {}{}\n"),
                           info.tithi,
                           date::format("%H:%M", tithi_until),
                           tithi_until_date == sunrise_date ? "" : " next day");
        }
        if (info.tithi2_until) {
            const auto tithi2_until = date::make_zoned(tz, info.tithi2_until->round_to_minute());
            auto tithi2_until_date{date::floor<date::days>(tithi2_until.get_local_time())};
            fmt::format_to(out,
                           FMT_STRING("{} until {}{}\n"),
                           info.tithi2,
                           date::format("%H:%M", tithi2_until),
                           tithi2_until_date == sunrise_date ? "" : " next day");
        }

        if (info.nakshatra_until) {
            const auto until = date::make_zoned(tz, info.nakshatra_until->round_to_minute());
            const auto date = date::floor<date::days>(until.get_local_time());
            fmt::format_to(out,
                           FMT_STRING("{} until {}{}\n"),
                           info.nakshatra,
                           date::format("%H:%M", until),
                           date == sunrise_date ? "" : " next day");
        }
        if (info.nakshatra2_until) {
            const auto until = date::make_zoned(tz, info.nakshatra2_until->round_to_minute());
            const auto date = date::floor<date::days>(until.get_local_time());
            fmt::format_to(out,
                           FMT_STRING("{} until {}{}\n"),
                           info.nakshatra2,
                           date::format("%H:%M", until),
                           date == sunrise_date ? "" : " next day");
        }
    }
}

void daybyday_add_tithi_events(vp::JulDays_UT from, vp::JulDays_UT to, const vp::Calc & calc, DayByDayInfo & info) {
    const auto min_tithi = calc.swe.tithi(from).floor();
    const auto max_tithi = calc.swe.tithi(to).ceil() + 1.0;
    auto start = from - std::chrono::hours{36};
    // need "!=" to handle cross-amavasya cases correctly, when max_tithi is less than min_tithi
    for (vp::Tithi tithi = min_tithi; tithi != max_tithi; tithi += 1.0) {
        auto tithi_start = calc.find_exact_tithi_start(start, tithi);
        if (tithi_start >= info.sunrise1) {
            if (info.tithi == vp::DiscreteTithi::Unknown()) {
                // -1.0 because local "tithi" variable holds the next tithi,
                // but info.{tithi,_until} refer to the previous tithi and it's end
                info.tithi = vp::DiscreteTithi{tithi-1.0};
                info.tithi_until = tithi_start;
            } else if (tithi_start < info.sunrise2 && info.tithi2 == vp::DiscreteTithi::Unknown()) {
                // -1.0 because local "tithi" variable holds the next tithi,
                // but info.{tithi,_until} refer to the previous tithi and it's end
                info.tithi2 = vp::DiscreteTithi{tithi-1.0};
                info.tithi2_until = tithi_start;
            }
        }
        auto description = fmt::format("{:d} starts", tithi);
        if (tithi.is_shukla_pratipat()) {
            description += fmt::format(FMT_STRING(", {} māsa starts"), calc.chandra_masa_amanta(tithi_start + double_days{1.0}));
        }
        if (tithi.is_krishna_ashtami()) {
            const auto saura_masa = calc.saura_masa(tithi_start);
            if (saura_masa == Saura_Masa::Simha) {
                const auto nakshatra = calc.swe.nakshatra(tithi_start);
                if (nakshatra.is_rohini()) {
                    description += " (**start of Siṁha+Rohiṇī+Kāḷāṣṭamī intersection**)";
                }
            }
        } else if (tithi.is_krishna_navami()) {
            const auto saura_masa = calc.saura_masa(tithi_start);
            if (saura_masa == Saura_Masa::Simha) {
                const auto nakshatra = calc.swe.nakshatra(tithi_start);
                if (nakshatra.is_rohini()) {
                    description += " (**end of Siṁha+Rohiṇī+Kāḷāṣṭamī intersection**)";
                }
            }
        }
        info.events.push_back(NamedTimePoint{description, tithi_start, TrackIntervalChange::Tithi});
        if (tithi.is_ekadashi()) {
            const auto ekadashi_end = calc.find_exact_tithi_start(tithi_start, tithi+1.0);
            const auto ekadashi_last_quarter_start = proportional_time(tithi_start, ekadashi_end, 0.75);
            info.events.push_back(NamedTimePoint{fmt::format("Last quarter of {:d} starts", tithi), ekadashi_last_quarter_start});
        }
        if (tithi.is_dvadashi()) {
            const auto dvadashi_end = calc.find_exact_tithi_start(tithi_start, tithi+1.0);
            const auto dvadashi_quarter_end = proportional_time(tithi_start, dvadashi_end, 0.25);
            //            auto dvadashi_quarter_end = calc.find_exact_tithi_start(start, tithi+0.25);
            info.events.push_back(NamedTimePoint{fmt::format("First quarter of {:d} ends", tithi), dvadashi_quarter_end});
        }
    }
}

void daybyday_add_nakshatra_events(vp::JulDays_UT from, vp::JulDays_UT to, const vp::Calc & calc, DayByDayInfo & info) {
    const auto min_nakshatra = calc.swe.nakshatra(from).floor();
    const auto max_nakshatra = calc.swe.nakshatra(to).ceil() + 1.0;
    auto start = from - std::chrono::hours{36}; // to ensure we get beginning of first nakshatra
    for (vp::Nakshatra n = min_nakshatra; n != max_nakshatra; ++n) {
        auto nakshatra_start = calc.find_nakshatra_start(start, n);
        if (nakshatra_start >= info.sunrise1) {
            if (info.nakshatra == DiscreteNakshatra::Unknown()) {
                info.nakshatra = DiscreteNakshatra{n - 1.0}; // we mark the end of the previous nakshatra
                info.nakshatra_until = nakshatra_start;
            } else if (info.nakshatra2 == DiscreteNakshatra::Unknown() && nakshatra_start < info.sunrise2) {
                info.nakshatra2 = DiscreteNakshatra{n - 1.0}; // we mark the end of the previous nakshatra
                info.nakshatra2_until = nakshatra_start;
            }
        }
        std::string description = fmt::format(FMT_STRING("{} starts"), DiscreteNakshatra{n});
        if (n.is_rohini()) {
            const auto saura_masa = calc.saura_masa(nakshatra_start);
            if (saura_masa == Saura_Masa::Simha) {
                const auto tithi = calc.swe.tithi(nakshatra_start);
                if (tithi.is_krishna_ashtami()) {
                    description += " (**start of Siṁha+Rohiṇī+Kāḷāṣṭamī intersection**)";
                }
            }
        } else if (n.is_mrgashira()) {
            const auto saura_masa = calc.saura_masa(nakshatra_start);
            if (saura_masa == Saura_Masa::Simha) {
                const auto tithi = calc.swe.tithi(nakshatra_start);
                if (tithi.is_krishna_ashtami()) {
                    description += " (**end of Siṁha+Rohiṇī+Kāḷāṣṭamī intersection**)";
                }
            }
        }
        info.events.push_back(NamedTimePoint{description, nakshatra_start, TrackIntervalChange::Nakshatra});
    }
}

static void
daybyday_add_moonrise_moonset_events(vp::JulDays_UT from, vp::JulDays_UT to, const vp::Calc & calc, DayByDayInfo & info) {
    for (auto time = from; time < to; time += double_days{0.001}) {
        const auto moonrise = calc.swe.next_moonrise(time);
        if (moonrise) {
            if (*moonrise < to) {
                info.events.push_back(NamedTimePoint{"moonrise", *moonrise});
            }
        }
        const auto moonset = calc.swe.next_moonset(time);
        if (moonset) {
            if (*moonset < to) {
                info.events.push_back(NamedTimePoint{"moonset", *moonset});
            }
        }
        if (moonrise) {
            time = *moonrise;
        }
        if (moonset) {
            time = std::max(time, *moonset);;
        }
    }
}


DayByDayInfo daybyday_events(date::year_month_day base_date, const vp::Calc & calc) {
    DayByDayInfo info;
    const auto local_astronomical_midnight = calc.calc_astronomical_midnight(date::local_days{base_date});
    const auto sunrise = calc.swe.next_sunrise(local_astronomical_midnight);
    if (sunrise) {
        info.sunrise1 = *sunrise;
        info.events.push_back(NamedTimePoint{"sunrise (prātaḥ-kāla begins)", *sunrise});
        const auto arunodaya = calc.arunodaya_for_sunrise(*sunrise);
        if (arunodaya) {
            info.events.push_back(NamedTimePoint{"arunodaya", *arunodaya});
        }

        const auto sunset = calc.swe.next_sunset(*sunrise);
        if (sunset) {
            info.sunset1 = *sunset;
            info.events.push_back(NamedTimePoint{"sunset", *sunset});
            info.events.push_back(NamedTimePoint{"1/5 of daytime (saṅgava-kāla begins)", proportional_time(*sunrise, *sunset, 0.2)});
            info.events.push_back(NamedTimePoint{"2/5 of daytime (madhyāhna-kāla begins)", proportional_time(*sunrise, *sunset, 0.4)});
            if ((calc.swe.calc_flags & CalcFlags::ShravanaDvadashiMask) == CalcFlags::ShravanaDvadashi14ghPlus) {
                info.events.push_back(NamedTimePoint{"14/30 of daytime (15th ghaṭika begins)", proportional_time(*sunrise, *sunset, 14.0/30)});
            }
            info.events.push_back(NamedTimePoint{"3/5 of daytime (aparāhna-kāla begins)", proportional_time(*sunrise, *sunset, 0.6)});
            info.events.push_back(NamedTimePoint{"4/5 of daytime (sāyāhna-kāla begins)", proportional_time(*sunrise, *sunset, 0.8)});
            info.events.push_back(NamedTimePoint{"middle of the day", proportional_time(*sunrise, *sunset, 0.5)});
            const auto sunrise2 = calc.swe.next_sunrise(*sunset);
            if (sunrise2) {
                info.sunrise2 = *sunrise2;
                const auto middle_of_night = proportional_time(*sunset, *sunrise2, 0.5);

                auto midnight_description = [](const Calc & calc, JulDays_UT time) {
                    fmt::memory_buffer buf;
                    fmt::appender app{buf};
                    fmt::format_to(app, "middle of the night");
                    if (calc.saura_masa(time) == Saura_Masa::Simha) {
                        bool is_rohini = calc.swe.nakshatra(time).is_rohini();
                        bool is_kalashtami = calc.swe.tithi(time).is_krishna_ashtami();
                        if (is_rohini || is_kalashtami) {
                            fmt::format_to(
                                        app,
                                        FMT_STRING(" (**Siṁha māsa, {}, {}**)"),
                                        is_rohini ? "Rohiṇī" : "no Rohiṇī",
                                        is_kalashtami ? "Kāḷāṣṭamī" : "no Kāḷāṣṭamī");
                        }
                    }
                    return fmt::to_string(buf);
                };
                info.events.push_back(NamedTimePoint{midnight_description(calc, middle_of_night), middle_of_night});
                info.events.push_back(NamedTimePoint{"next sunrise", *sunrise2});
                const auto earliest_timepoint = arunodaya ? * arunodaya : *sunrise;
                const auto latest_timepoint = *sunrise2;
                daybyday_add_tithi_events(earliest_timepoint, latest_timepoint, calc, info);
                daybyday_add_nakshatra_events(earliest_timepoint, latest_timepoint, calc, info);
                daybyday_add_moonrise_moonset_events(earliest_timepoint, latest_timepoint, calc, info);
            }
        }
    }
    return info;
}

auto NamedPointComparator = [](const NamedTimePoint & left, const NamedTimePoint & right) {
    return left.time_point < right.time_point;
};
void add_to_sorted(NamedTimePoints & points, NamedTimePoint point) {
    points.insert(
        std::upper_bound(
            points.begin(),
            points.end(),
            point,
            NamedPointComparator),
        point);
}

void daybyday_add_sauramasa_info(DayByDayInfo & info, const vp::Calc & calc) {
    if (info.events.empty()) {
        return;
    }

    if (info.sunrise1) {
        info.saura_masa = calc.saura_masa(*info.sunrise1);
    }

    const auto initial_time = info.events[0].time_point;
    const auto initial_masa = calc.saura_masa(initial_time);
    const auto last_time = info.events.back().time_point;

    const auto next_masa = initial_masa + 1;
    const auto next_masa_start = calc.find_sankranti(initial_time, next_masa);
    info.saura_masa_until = next_masa_start;
    if (next_masa_start <= last_time) {
        auto event_name = fmt::format(FMT_STRING("{} sankranti"), next_masa);
        add_to_sorted(info.events, NamedTimePoint{event_name, next_masa_start});
    }
}

void daybyday_add_chandramasa_info(DayByDayInfo & info, const vp::Calc & calc) {
    if (!info.sunrise1.has_value()) {
        return;
    }
    const auto initial_time = *info.sunrise1;
    info.chandra_masa = calc.chandra_masa_amanta(initial_time, &info.chandra_masa_until);
}
} // anonymous namespace

DayByDayInfo daybyday_calc_one(date::year_month_day base_date, const Location & coord, CalcFlags flags)
{
    Calc calc{Swe{coord, flags}};
    DayByDayInfo info = daybyday_events(base_date, calc);
    info.location = coord;
    info.date = base_date;
    std::stable_sort(info.events.begin(), info.events.end(), NamedPointComparator);
    daybyday_add_sauramasa_info(info, calc);
    daybyday_add_chandramasa_info(info, calc);
    return info;
}

namespace {
/* print day-by-day report (-d mode) for a single date and single location */
void daybyday_print_one(date::year_month_day base_date, const Location & coord, const fmt::appender & out, vp::CalcFlags flags) {
    auto info = daybyday_calc_one(base_date, coord, flags);

    daybyday_print_header(base_date, coord, info, out);

    for (const auto & e : info.events) {
        // add separator before sunrises to mark current day better
        if ((info.sunrise1 && e.time_point == *info.sunrise1) || (info.sunrise2 && e.time_point == *info.sunrise2)) {
            fmt::format_to(out, FMT_STRING("-----\n"));
        }
        fmt::format_to(out, "{} {}\n", vp::JulDays_Zoned{coord.time_zone(), e.time_point}, e.name);
    }
}
}

void daybyday_print_one(date::year_month_day base_date, const char * location_name, const fmt::appender & out, vp::CalcFlags flags) {
    const std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        fmt::format_to(out, "Location not found: '{}'\n", location_name);
        return;
    }
    daybyday_print_one(base_date, *coord, out, flags);
}

void calc_and_report_all(date::year_month_day d) {
    for (auto &l : LocationDb()) {
        fmt::memory_buffer buf;
        calc_and_report_one(d, l, fmt::appender{buf});
        fmt::print("{}", std::string_view{buf.data(), buf.size()});
    }
}

namespace detail {
    fs::path determine_exe_dir(const char* argv0) {
        return fs::absolute(fs::path{argv0}).parent_path();
    }

    fs::path determine_working_dir(const char* argv0) {
        auto exe_dir = detail::determine_exe_dir(argv0);

        constexpr int max_steps_up = 2;

        for (int step=0; step < max_steps_up; ++step) {
            // Most common case: "eph" and "tzdata" directories exist in the same directory as .exe
            if (fs::exists(exe_dir / "eph")) {
                return exe_dir;
            }

            // But when running exes from Debug/ or Release/ subdirectories, we need to make one step up.
            // But only make one step up: if the data dir still doesn't exist there either, too bad...
            exe_dir = exe_dir.parent_path();
        }
        // fallback: return whatever we got, even if we couldn't find proper working dir.
        return exe_dir;
    }
}

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir or the one above) */
void change_to_data_dir(const char* argv0)
{
    auto working_dir = detail::determine_working_dir(argv0);
    fs::current_path(working_dir);
}

namespace {
std::string version()
{
#ifdef VP_VERSION
    return std::string{VP_VERSION};
#else
    return std::string{"unknown"};
#endif
}
}

std::string program_name_and_version()
{
    return "Vaiṣṇavaṁ Pañcāṅgam " + version();
}

} // namespace vp::text_ui

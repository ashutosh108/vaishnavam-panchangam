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
        { udupi_coord },                    // 13'20'27_N,  74'45'06_E, UTC+05:30 India (Asia/Kolkata)
        { kalkuta_coord },                  // 22'32'00_N,  88'22'00_E, UTC+05:30 India (Asia/Kolkata)
        { manali_coord },                   // 32'16'00_N,  77'10'00_E, UTC+05:30 India (Asia/Kolkata)
        { newdelhi_coord },                 // 28'39'00_N,  77'13'00_E, UTC+05:30 India (Asia/Kolkata)
        { bengaluru_coord },                // 12'58'00_N,  77'34'00_E, UTC+05:30 India (Asia/Kolkata)
        { gokarna_coord },                  // 14'33'00_N,  74'19'00_E, UTC+05:30 India (Asia/Kolkata)
        { dushanbe_coord },                 // 38'32'12_N,  68'46'48_E, UTC+05:00 Tajikistan (Asia/Dushanbe)
        { perm_coord },                     // 58'00'00_N,  56'14'00_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { ufa_coord },                      // 54'44'00_N,  55'58'00_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { orenburg_coord },                 // 51'46'00_N,  55'06'00_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { ekaterinburg_coord },             // 56'50'00_N,  60'35'00_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { surgut_coord },                   // 61'15'00_N,  73'26'00_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { chelyabinsk_coord },              // 55'09'44_N,  61'24'11_E, UTC+05:00 Russia (Asia/Yekaterinburg)
        { aktobe_coord },                   // 50'18'00_N,  57'10'00_E, UTC+05:00 Kazakhstan (Asia/Aqtobe)
        { aktau_coord },                    // 43'39'00_N,  51'09'00_E, UTC+05:00 Kazakhstan (Asia/Aqtau)
        { kokshetau_coord },                // 53'17'30_N,  69'23'30_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { almaata_coord },                  // 43'15'00_N,  76'54'00_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { tekeli_coord },                   // 44'51'47_N,  78'45'51_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { ustkamenogorsk_coord },           // 49'59'00_N,  82'37'00_E, UTC+05:00 Kazakhstan (Asia/Almaty)
        { bishkek_coord },                  // 42'52'00_N,  74'34'00_E, UTC+06:00 Kyrgyzstan (Asia/Bishkek)
        { omsk_coord },                     // 54'58'00_N,  73'23'00_E, UTC+06:00 Russia (Asia/Omsk)
        { krasnoyarsk_coord },              // 56'00'43_N,  92'52'17_E, UTC+07:00 Russia (Asia/Krasnoyarsk)
        { novokuznetsk_coord },             // 53'44'00_N,  87'05'00_E, UTC+07:00 Russia (Asia/Krasnoyarsk)
        { novosibirsk_coord },              // 55'01'00_N,  82'55'00_E, UTC+07:00 Russia (Asia/Novosibirsk)
        { barnaul_coord },                  // 53'21'00_N,  83'46'00_E, UTC+07:00 Russia (Asia/Barnaul)
        { tomsk_coord },                    // 56'29'00_N,  84'57'00_E, UTC+07:00 Russia (Asia/Tomsk)
        { kophangan_coord },                //  9'43'00_N, 100'00'00_E, UTC+07:00 Thailand (Asia/Bangkok)
        { denpasar_coord },                 //  8'39'00_S, 115'13'00_E, UTC+08:00 Indonesia (Asia/Makassar)
        { mirnyy_coord },                   // 62'32'00_N, 113'57'00_E, UTC+09:00 Russia (Asia/Yakutsk)
        { habarovsk_coord },                // 48'29'00_N, 135'05'00_E, UTC+10:00 Russia (Asia/Vladivostok)
        { vladivostok_coord },              // 43'07'00_N, 131'54'00_E, UTC+10:00 Russia (Asia/Vladivostok)
        { petropavlovskkamchatskiy_coord }, // 53'01'00_N, 158'39'00_E, UTC+12:00 Russia (Asia/Kamchatka)
        { erevan_coord },                   // 40'11'00_N,  44'31'00_E, UTC+04/05 Armenia (Asia/Yerevan)
        { tbilisi_coord },                  // 41'43'00_N,  44'47'00_E, UTC+04:00 Georgia (Asia/Tbilisi)
        { samara_coord },                   // 53'11'00_N,  50'07'00_E, UTC+04:00 Russia (Europe/Samara)
        { volgograd_coord },                // 48'41'57_N,  44'28'24_E, UTC+03:00 Russia (Europe/Volgograd)
        { ulyanovsk_coord },                // 54'19'00_N,  48'22'00_E, UTC+04:00 Russia (Europe/Ulyanovsk)
        { pyatigorsk_coord },               // 44'02'00_N,  43'03'00_E, UTC+03:00 Russia (Europe/Moscow)
        { stavropol_coord },                // 45'02'00_N,  41'58'00_E, UTC+03:00 Russia (Europe/Moscow)
        { semikarakorsk_coord },            // 47'31'00_N,  40'48'00_E, UTC+03:00 Russia (Europe/Moscow)
        { krasnodar_coord },                // 45'02'00_N,  38'59'00_E, UTC+03:00 Russia (Europe/Moscow)
        { kerch_coord },                    // 45'20'19_N,  36'28'05_E, UTC+03:00 Russia (Europe/Simferopol)
        { simferopol_coord },               // 44'56'00_N,  34'06'00_E, UTC+03:00 Russia (Europe/Simferopol)
        { donetsk_coord },                  // 48'00'00_N,  37'48'00_E, UTC+03:00 DNR (Europe/Moscow)
        { staryyoskol_coord },              // 51'17'00_N,  37'50'00_E, UTC+03:00 Russia (Europe/Moscow)
        { lipetsk_coord },                  // 52'37'00_N,  39'36'00_E, UTC+03:00 Russia (Europe/Moscow)
        { voronezh_coord },                 // 51'40'00_N,  39'12'00_E, UTC+03:00 Russia (Europe/Moscow)
        { tambov_coord },                   // 52'43'00_N,  41'26'00_E, UTC+03:00 Russia (Europe/Moscow)
        { kazan_coord },                    // 55'47'00_N,  49'06'00_E, UTC+03:00 Russia (Europe/Moscow)
        { cheboksary_coord },               // 56'07'00_N,  47'14'00_E, UTC+03:00 Russia (Europe/Moscow)
        { kirov_coord },                    // 58'36'00_N,  49'39'00_E, UTC+03:00 Russia (Europe/Kirov)
        { rostov_na_donu_coord },           // 47'14'26_N,  39'42'38_E, UTC+03:00 Russia (Europe/Moscow)
        { ryazan_coord },                   // 54'37'00_N,  39'43'00_E, UTC+03:00 Russia (Europe/Moscow)
        { moskva_coord },                   // 55'45'00_N,  37'35'00_E, UTC+03:00 Russia (Europe/Moscow)
        { dubna_coord },                    // 56'44'00_N,  37'10'00_E, UTC+03:00 Russia (Europe/Moscow)
        { spb_coord },                      // 59'55'00_N,  30'15'00_E, UTC+03:00 Russia (Europe/Moscow)
        { murmansk_coord },                 // 68'58'00_N,  33'05'00_E, UTC+03:00 Russia (Europe/Moscow)
        { kostomuksha_coord },              // 64'35'00_N,  30'36'00_E, UTC+03:00 Russia (Europe/Moscow)
        { arhangelsk_coord },               // 64'33'00_N,  40'32'00_E, UTC+03:00 Russia (Europe/Moscow)
        { smolensk_coord },                 // 54'46'00_N,  32'02'00_E, UTC+03:00 Russia (Europe/Moscow)
        { gomel_coord },                    // 52'26'00_N,  30'59'00_E, UTC+03:00 Belarus (Europe/Minsk)
        { minsk_coord },                    // 53'55'00_N,  27'35'00_E, UTC+03:00 Belarus (Europe/Minsk)
        { grodno_coord },                   // 53'40'00_N,  23'49'00_E, UTC+03:00 Belarus (Europe/Minsk)
        { harkov_coord },                   // 50'00'00_N,  36'15'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { poltava_coord },                  // 49'34'00_N,  34'34'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kremenchug_coord },               // 49'04'00_N,  33'25'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { krivoyrog_coord },                // 47'57'00_N,  33'25'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kiev_coord },                     // 50'27'00_N,  30'31'24_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kropyvnytskyi_coord },            // 48'30'00_N,  32'16'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { nikolaev_coord },                 // 46'58'00_N,  32'00'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { odessa_coord },                   // 46'28'00_N,  30'44'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { kolomyya_coord },                 // 48'31'00_N,  25'02'00_E, UTC+02/03 Ukraine (Europe/Kiev)
        { ashkelon_coord },                 // 31'39'57_N,  34'33'59_E, UTC+02/03 Israel (Asia/Jerusalem)
        { kishinev_coord },                 // 47'00'00_N,  28'51'00_E, UTC+02/03 Moldova (Europe/Chisinau)
        { nicosia_coord },                  // 35'10'21_N,  33'21'54_E, UTC+02/03 Cyprus (Asia/Nicosia)
        { riga_coord },                     // 56'56'00_N,  24'06'00_E, UTC+02/03 Latvia (Europe/Riga)
        { jurmala_coord },                  // 56'58'00_N,  23'47'00_E, UTC+02/03 Latvia (Europe/Riga)
        { tallin_coord },                   // 59'26'00_N,  24'45'00_E, UTC+02/03 Estonia (Europe/Tallinn)
        { vilnyus_coord },                  // 54'41'00_N,  25'17'00_E, UTC+02/03 Lithuania (Europe/Vilnius)
        { varshava_coord },                 // 52'13'00_N,  21'02'00_E, UTC+01/02 Poland (Europe/Warsaw)
        { dillingen_an_der_donau_coord },   // 48'34'00_N,  10'28'00_E, UTC+01/02 Germany (Europe/Berlin)
        { eppingen_coord },                 // 49'08'00_N,   8'55'00_E, UTC+01/02 Germany (Europe/Berlin)
        { vena_coord },                     // 48'12'00_N,  16'22'00_E, UTC+01/02 Austria (Europe/Vienna)
        { biel_bienne_coord },              // 47'08'14_N,   7'14'50_E, UTC+01/02 Switzerland (Europe/Zurich)
        { marsel_coord },                   // 43'17'47_N,   5'22'12_E, UTC+01/02 France (Europe/Paris)
        { barcelona_coord },                // 41'23'00_N,   2'11'00_E, UTC+01/02 Spain (Europe/Madrid)
        { madrid_coord },                   // 40'24'00_N,   3'41'00_W, UTC+01/02 Spain (Europe/Madrid)
        { london_coord },                   // 51'30'00_N,      7'00_W, UTC+00/01 UK (Europe/London)
        { fredericton_coord },              // 45'57'00_N,  66'38'00_W, UTC-04/03 Canada (America/Moncton)
        { toronto_coord },                  // 42'43'00_N,  79'24'00_W, UTC-05/04 Canada (America/New_York)
        { newyork_coord },                  // 40'42'45_N,  74'00'22_W, UTC-05/04 USA (America/New_York)
        { miami_coord },                    // 25'47'00_N,  80'13'00_W, UTC-05/04 USA (America/New_York)
        { cancun_coord },                   // 21'09'38_N,  86'50'51_W, UTC-05:00 Mexico (America/Cancun)
        { meadowlake_coord },               // 54'07'00_N, 108'26'00_W, UTC-06:00 Canada (America/Regina)
        { losanjeles_coord },               // 34'02'00_N, 118'16'00_W, UTC-08/07 USA (America/Los_Angeles)
//        { tagbilaran_coord },               //  9'39'00_N, 123'51'00_E, UTC+08/09 Philippines (Asia/Manila)
//        { chita_coord },                    // 52'02'00_N, 113'30'00_E, UTC+09:00 Russia (Asia/Chita)
//        { pokhara_coord },                  // 28'15'00_N,  83'58'00_E, UTC+05:45 Nepal (Asia/Kathmandu)
//        { kolombo_coord },                  //  6'55'00_N,  79'50'00_E, UTC+05:30 SriLanka (Asia/Colombo)
//        { vrindavan_coord },                // 28'36'00_N,  77'12'00_E, UTC+05:30 India (Asia/Kolkata)
//        { pune_coord },                     // 18'32'00_N,  73'52'00_E, UTC+05:30 India (Asia/Kolkata)
//        { pernem_coord },                   // 15'43'00_N,  73'47'49_E, UTC+05:30 India (Asia/Kolkata)
//        { bombey_coord },                   // 18'58'00_N,  72'50'00_E, UTC+05:30 India (Asia/Kolkata)
//        { panaji_coord },                   // 15'29'00_N,  72'50'00_E, UTC+05:30 India (Asia/Kolkata)
//        { tashkent_coord },                 // 41'18'00_N,  68'16'00_E, UTC+06:00 Uzbekistan (Asia/Almaty)
//        { suhum_coord },                    // 43'00'00_N,  41'00'00_E, UTC+03:00 Abkhazia (Europe/Moscow)
//        { sochi_coord },                    // 43'36'00_N,  39'44'00_E, UTC+03:00 Russia (Europe/Moscow)
//        { lugansk_coord },                  // 48'34'00_N,  39'20'00_E, UTC+03:00 LNR (Europe/Moscow)
//        { ramenskoe_m_obl_coord },          // 55'34'00_N,  38'13'00_E, UTC+03:00 Russia (Europe/Moscow)
//        { telaviv_coord },                  // 32'04'00_N,  34'47'00_E, UTC+02/03 Israel (Asia/Jerusalem)
//        { novgorod_coord },                 // 58'33'00_N,  31'16'00_E, UTC+03:00 Russia (Europe/Moscow)
//        { tiraspol_coord },                 // 46'51'00_N,  29'36'00_E, UTC+02/03 Transnistria (Europe/Chisinau)
//        { vinnitsa_coord },                 // 49'14'00_N,  28'27'00_E, UTC+02/03 Ukraine (Europe/Kiev)
//        { hmelnitskiy_coord },              // 49'25'00_N,  26'59'00_E, UTC+02/03 Ukraine (Europe/Kiev)
//        { afiny_coord },                    // 37'58'00_N,  23'43'00_E, UTC+02/03 Greece (Europe/Athens)
//        { kiel_coord },                     // 54'19'00_N,  10'08'00_E, UTC+01/02 Germany (Europe/Berlin)
//        { freiburg_coord },                 // 48'00'00_N,   7'52'00_E, UTC+01/02 Germany (Europe/Paris)
//        { koeln_kkd_coord },                // 50'56'11_N,   6'57'10_E, UTC+01/02 Germany (Europe/Berlin)
//        { dusseldorf_coord },               // 51'13'00_N,   6'47'00_E, UTC+01/02 Germany (Europe/Berlin)
//        { manchester_coord },               // 53'30'00_N,   2'13'00_W, UTC+00/01 UK (Europe/London)
//        { puno_coord },                     // 15'50'00_S,  70'01'00_W, UTC-05:00 Peru (America/Lima)
//        { washington_coord },               // 38'53'00_N,  77'02'00_W, UTC-05/04 USA (America/New_York)
//        { mundelein_coord },                // 42'16'00_N,  88'00'00_W, UTC-06/05 USA (America/Chicago)
//        { edmonton_coord },                 // 53'32'00_N, 113'30'00_W, UTC-07/06 Canada (America/Edmonton)
//        { sanfrantsisko_coord },            // 37'46'00_N, 122'24'00_W, UTC-08/07 USA (America/Los_Angeles)
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

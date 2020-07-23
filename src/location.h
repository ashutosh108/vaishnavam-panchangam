#ifndef LOCATION_H
#define LOCATION_H

#include <cmath> // fabs
#include <cstring>
#include "fmt-format-fixed.h"
#include <string_view>
#include <tuple> // for std::tie() in comparison operators.

namespace vp {

struct Latitude {
    double latitude;
};

struct Longitude {
    double longitude;
};

constexpr double int_deg_min_sec_to_double_degrees(const unsigned long long val) {
    if (val > 180'00'00) { throw std::logic_error("Value for degrees must be in 0..180'00'00 range"); }
    auto deg = static_cast<int>(val / 10'000);
    if (deg > 180) { throw std::logic_error("degrees part in dd'mm'ss must be in 0..180 range"); }
    auto min = static_cast<int>((val / 100) % 100);
    if (min > 59) { throw std::logic_error("minutes part in dd'mm'ss must be in 0..59 range"); }
    auto sec = static_cast<int>(val % 100);
    if (sec > 59) { throw std::logic_error("seconds part in dd'mm'ss must be in 0..59 range"); }
    return deg + static_cast<double>(min)/60.0 + static_cast<double>(sec)/3600.0;
}

constexpr Latitude operator ""_N(const unsigned long long lat) {
    return Latitude{int_deg_min_sec_to_double_degrees(lat)};
}

constexpr Latitude operator ""_S(const unsigned long long lat) {
    return Latitude{-int_deg_min_sec_to_double_degrees(lat)};
}

constexpr Longitude operator ""_E(const unsigned long long lng) {
    return Longitude{int_deg_min_sec_to_double_degrees(lng)};
}

constexpr Longitude operator ""_W(const unsigned long long lng) {
    return Longitude{-int_deg_min_sec_to_double_degrees(lng)};
}

constexpr Latitude operator ""_N(const long double lat) {
    if (lat < 0.0l || lat > 90.0l) { throw std::logic_error("Latitude must be in 0.0..90.0 range"); }
    return Latitude{static_cast<double>(lat)};
}

constexpr Latitude operator ""_S(const long double lat) {
    if (lat < 0.0l || lat > 90.0l) { throw std::logic_error("Latitude must be in 0.0..90.0 range"); }
    return Latitude{-static_cast<double>(lat)};
}

constexpr Longitude operator ""_E(const long double lng) {
    if (lng < 0.0l || lng > 180.0l) { throw std::logic_error("Longitude must be in 0.0..180.0 range"); }
    return Longitude{static_cast<double>(lng)};
}

constexpr Longitude operator ""_W(const long double lng) {
    if (lng < 0.0l || lng > 180.0l) { throw std::logic_error("Longitude must be in 0.0..180.0 range"); }
    return Longitude{-static_cast<double>(lng)};
}

struct Location
{
    Latitude latitude;
    Longitude longitude;
    const char *timezone_name;
    const char *name;

    // If true, then the latitude has been decreased until we found location
    // where sunrises/sunsets do exist.
    bool latitude_adjusted = false;

    explicit constexpr Location(Latitude lat = Latitude{0.0}, Longitude lng = Longitude{0.0},
                                const char * _name = "Custom Location",
                                const char * _timezone_name="UTC")
        : latitude(lat),
          longitude(lng),
          timezone_name(_timezone_name),
          name(_name) {}
};

inline bool operator==(const Location & one, const Location & other) {
    return one.latitude.latitude == other.latitude.latitude && one.longitude.longitude == other.longitude.longitude && std::strcmp(one.timezone_name, other.timezone_name) == 0 && std::strcmp(one.name, other.name) == 0;
}

inline bool operator!=(const Location & one, const Location & other) {
    return !(one == other);
}

inline bool operator<(const Location & one, const Location & two) {
    std::string_view one_tz{one.timezone_name};
    std::string_view two_tz{two.timezone_name};
    std::string_view one_name{one.name};
    std::string_view two_name{two.name};
    return
            std::tie(one.latitude.latitude, one.longitude.longitude, one_tz, one_name) <
            std::tie(two.latitude.latitude, two.longitude.longitude, two_tz, two_name);
}


[[maybe_unused]] constexpr Location odessa_coord{                  46'28'00_N,  30'44'00_E, "Odessa", "Europe/Kiev"};
[[maybe_unused]] constexpr Location vinnitsa_coord{                49'14'00_N,  28'27'00_E, "Vinnitsa", "Europe/Kiev"};
[[maybe_unused]] constexpr Location kiev_coord{                    50'27'00_N,  30'31'24_E, "Kiev", "Europe/Kiev"}; // en Wikipedia
[[maybe_unused]] constexpr Location sanfrantsisko_coord{           37'46'00_N, 122'24'00_W, "San Francisco", "America/Los_Angeles"};
[[maybe_unused]] constexpr Location tiraspol_coord{                46'51'00_N,  29'36'00_E, "Tiraspol", "Europe/Chisinau"};
[[maybe_unused]] constexpr Location hmelnitskiy_coord{             49'25'00_N,  26'59'00_E, "Khmelnytskyi", "Europe/Kiev"};
[[maybe_unused]] constexpr Location kishinev_coord{                47'00'00_N,  28'51'00_E, "Kishinev", "Europe/Chisinau"};
[[maybe_unused]] constexpr Location voronezh_coord{                51'40'00_N,  39'12'00_E, "Voronezh", "Europe/Moscow"};
[[maybe_unused]] constexpr Location harkov_coord{                  50'00'00_N,  36'15'00_E, "Kharkov", "Europe/Kiev"};
[[maybe_unused]] constexpr Location habarovsk_coord{               48'29'00_N, 135'05'00_E, "Khabarovsk", "Asia/Vladivostok"};
[[maybe_unused]] constexpr Location lugansk_coord{                 48'34'00_N,  39'20'00_E, "Lugansk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location moskva_coord{                  55'45'00_N,  37'35'00_E, "Moscow", "Europe/Moscow"};
[[maybe_unused]] constexpr Location vrindavan_coord{               28'36'00_N,  77'12'00_E, "Vrindavan", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location losanjeles_coord{              34'03'00_N, 118'14'00_W, "Los Angeles", "America/Los_Angeles"};
[[maybe_unused]] constexpr Location kalkuta_coord{                 22'32'00_N,  88'22'00_E, "Kolkata", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location spb_coord{                     59'55'00_N,  30'15'00_E, "Saint Petersburg", "Europe/Moscow"};
[[maybe_unused]] constexpr Location freiburg_coord{                48'00'00_N,   7'52'00_E, "Freiburg im Breisgau", "Europe/Paris"};
[[maybe_unused]] constexpr Location nikolaev_coord{                46'58'00_N,  32'00'00_E, "Nikolaev", "Europe/Kiev"};
[[maybe_unused]] constexpr Location ramenskoe_m_obl_coord{         55'34'00_N,  38'13'00_E, "Ramenskoye, Moscow Oblast", "Europe/Moscow"}; // en Wikipedia
[[maybe_unused]] constexpr Location minsk_coord{                   53'55'00_N,  27'35'00_E, "Minsk", "Europe/Minsk"};
[[maybe_unused]] constexpr Location barnaul_coord{                 53'21'00_N,  83'46'00_E, "Barnaul", "Asia/Barnaul"};
[[maybe_unused]] constexpr Location newdelhi_coord{                28'39'00_N,  77'13'00_E, "New Delhi", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location dusseldorf_coord{              51'13'00_N,   6'47'00_E, "Dusseldorf", "Europe/Berlin"};
[[maybe_unused]] constexpr Location koeln_kkd_coord{               50'56'11_N,   6'57'10_E, "Cologne", "Europe/Berlin"}; // en Wikipedia
[[maybe_unused]] constexpr Location sochi_coord{                   43'36'00_N,  39'44'00_E, "Sochi", "Europe/Moscow"};
[[maybe_unused]] constexpr Location novgorod_coord{                58'33'00_N,  31'16'00_E, "Velikiy Novgorod", "Europe/Moscow"}; // en Wikipedia
[[maybe_unused]] constexpr Location london_coord{                  51'30'00_N,      7'00_W, "London", "Europe/London"};
[[maybe_unused]] constexpr Location manchester_coord{              53'30'00_N,   2'13'00_W, "Manchester", "Europe/London"};
[[maybe_unused]] constexpr Location panaji_coord{                  15'29'00_N,  72'50'00_E, "Panaji", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location bombey_coord{                  18'58'00_N,  72'50'00_E, "Mumbai", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location pune_coord{                    18'32'00_N,  73'52'00_E, "Pune", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location simferopol_coord{              44'56'00_N,  34'06'00_E, "Simferopol", "Europe/Simferopol"};
[[maybe_unused]] constexpr Location manali_coord{                  32'16'00_N,  77'10'00_E, "Manali, Himachal Pradesh", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location pyatigorsk_coord{              44'02'00_N,  43'03'00_E, "Pyatigorsk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location kirov_coord{                   58'36'00_N,  49'39'00_E, "Kirov", "Europe/Kirov"};
[[maybe_unused]] constexpr Location washington_coord{              38'53'00_N,  77'02'00_W, "Washington, D.C.", "America/New_York"};
[[maybe_unused]] constexpr Location gokarna_coord{                 14'33'00_N,  74'19'00_E, "Gokarna, Karnataka", "Asia/Kolkata"};
[[maybe_unused]] constexpr Location telaviv_coord{                 32'04'00_N,  34'47'00_E, "Tel Aviv", "Asia/Jerusalem"};
[[maybe_unused]] constexpr Location tomsk_coord{                   56'29'00_N,  84'57'00_E, "Tomsk", "Asia/Tomsk"};
[[maybe_unused]] constexpr Location kiel_coord{                    54'19'00_N,  10'08'00_E, "Kiel", "Europe/Berlin"};
[[maybe_unused]] constexpr Location omsk_coord{                    54'58'00_N,  73'23'00_E, "Omsk", "Asia/Omsk"};
[[maybe_unused]] constexpr Location tashkent_coord{                41'18'00_N,  68'16'00_E, "Tashkent", "Asia/Almaty"};
[[maybe_unused]] constexpr Location dushanbe_coord{                38'32'12_N,  68'46'48_E, "Dushanbe", "Asia/Dushanbe"};
[[maybe_unused]] constexpr Location udupi_coord{                   13'20'27_N,  74'45'06_E, "Udupi", "Asia/Kolkata"}; // Ananteshvara temple
[[maybe_unused]] constexpr Location varshava_coord{                52'13'00_N,  21'02'00_E, "Warsaw", "Europe/Warsaw"};
[[maybe_unused]] constexpr Location donetsk_coord{                 48'00'00_N,  37'48'00_E, "Donetsk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location tbilisi_coord{                 41'43'00_N,  44'47'00_E, "Tbilisi", "Asia/Tbilisi"};
[[maybe_unused]] constexpr Location suhum_coord{                   43'00'00_N,  41'00'00_E, "Sukhum", "Europe/Moscow"};
[[maybe_unused]] constexpr Location kremenchug_coord{              49'04'00_N,  33'25'00_E, "Kremenchug", "Europe/Kiev"};
[[maybe_unused]] constexpr Location puno_coord{                    15'50'00_S,  70'01'00_W, "Puno", "America/Lima"};
[[maybe_unused]] constexpr Location vladivostok_coord{             43'07'00_N, 131'54'00_E, "Vladivostok", "Asia/Vladivostok"};
[[maybe_unused]] constexpr Location pernem_coord{                  15'43'00_N,  73'47'49_E, "Pernem", "Asia/Kolkata"}; // en Wikipedia
[[maybe_unused]] constexpr Location krasnodar_coord{               45'02'00_N,  38'59'00_E, "Krasnodar", "Europe/Moscow"};
[[maybe_unused]] constexpr Location meadowlake_coord{              54'07'00_N, 108'26'00_W, "Meadow Lake, Canada", "America/Regina"};
[[maybe_unused]] constexpr Location toronto_coord{                 42'43'00_N,  79'24'00_W, "Toronto", "America/New_York"};
[[maybe_unused]] constexpr Location fredericton_coord{             45'57'00_N,  66'38'00_W, "Fredericton", "America/Moncton"};
[[maybe_unused]] constexpr Location perm_coord{                    58'00'00_N,  56'14'00_E, "Perm", "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location ufa_coord{                     54'44'00_N,  55'58'00_E, "Ufa", "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location smolensk_coord{                54'46'00_N,  32'02'00_E, "Smolensk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location krivoyrog_coord{               47'57'00_N,  33'25'00_E, "Krivoy Rog", "Europe/Kiev"};
[[maybe_unused]] constexpr Location petropavlovskkamchatskiy_coord{53'01'00_N, 158'39'00_E, "Petropavlovsk-Kamchatsky", "Asia/Kamchatka"};
[[maybe_unused]] constexpr Location kophangan_coord{                9'43'00_N, 100'00'00_E, "Ko Pha-ngan", "Asia/Bangkok"};
[[maybe_unused]] constexpr Location denpasar_coord{                 8'39'00_S, 115'13'00_E, "Denpasar", "Asia/Makassar"};
[[maybe_unused]] constexpr Location mundelein_coord{               42'16'00_N,  88'00'00_W, "Mundelein", "America/Chicago"};
[[maybe_unused]] constexpr Location bishkek_coord{                 42'52'00_N,  74'34'00_E, "Bishkek", "Asia/Bishkek"};
[[maybe_unused]] constexpr Location vena_coord{                    48'12'00_N,  16'22'00_E, "Vienna", "Europe/Vienna"};
[[maybe_unused]] constexpr Location staryyoskol_coord{             51'17'00_N,  37'50'00_E, "Stary Oskol", "Europe/Moscow"};
[[maybe_unused]] constexpr Location edmonton_coord{                53'32'00_N, 113'30'00_W, "Edmonton", "America/Edmonton"};
[[maybe_unused]] constexpr Location novosibirsk_coord{             55'01'00_N,  82'55'00_E, "Novosibirsk", "Asia/Novosibirsk"};
[[maybe_unused]] constexpr Location erevan_coord{                  40'11'00_N,  44'31'00_E, "Yerevan", "Asia/Yerevan"};
[[maybe_unused]] constexpr Location stavropol_coord{               45'02'00_N,  41'58'00_E, "Stavropol", "Europe/Moscow"};
[[maybe_unused]] constexpr Location pokhara_coord{                 28'15'00_N,  83'58'00_E, "Pokhara", "Asia/Kathmandu"};
[[maybe_unused]] constexpr Location murmansk_coord{                68'58'00_N,  33'05'00_E, "Murmansk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location mirnyy_coord{                  62'32'00_N, 113'57'00_E, "Mirny, Sakha Republic", "Asia/Yakutsk"};
[[maybe_unused]] constexpr Location riga_coord{                    56'56'00_N,  24'06'00_E, "Riga", "Europe/Riga"};
[[maybe_unused]] constexpr Location surgut_coord{                  61'15'00_N,  73'26'00_E, "Surgut", "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location ryazan_coord{                  54'37'00_N,  39'43'00_E, "Ryazan", "Europe/Moscow"};
[[maybe_unused]] constexpr Location afiny_coord{                   37'58'00_N,  23'43'00_E, "Athens", "Europe/Athens"};
[[maybe_unused]] constexpr Location chita_coord{                   52'02'00_N, 113'30'00_E, "Chita", "Asia/Chita"};
[[maybe_unused]] constexpr Location poltava_coord{                 49'34'00_N,  34'34'00_E, "Poltava", "Europe/Kiev"};
[[maybe_unused]] constexpr Location kazan_coord{                   55'47'00_N,  49'06'00_E, "Kazan", "Europe/Moscow"};
[[maybe_unused]] constexpr Location aktau_coord{                   43'39'00_N,  51'09'00_E, "Aktau", "Asia/Aqtau"};
[[maybe_unused]] constexpr Location aktobe_coord{                  50'18'00_N,  57'10'00_E, "Aktobe", "Asia/Aqtobe"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tallin_coord{                  59'26'00_N,  24'45'00_E, "Tallinn", "Europe/Tallinn"};
[[maybe_unused]] constexpr Location jurmala_coord{                 56'58'00_N,  23'47'00_E, "Jurmala", "Europe/Riga"};
[[maybe_unused]] constexpr Location semikarakorsk_coord{           47'31'00_N,  40'48'00_E, "Semikarakorsk", "Europe/Moscow"};
[[maybe_unused]] constexpr Location kolombo_coord{                  6'55'00_N,  79'50'00_E, "Colombo", "Asia/Colombo"};
[[maybe_unused]] constexpr Location ulyanovsk_coord{               54'19'00_N,  48'22'00_E, "Ulyanovsk", "Europe/Ulyanovsk"};
[[maybe_unused]] constexpr Location tagbilaran_coord{               9'39'00_N, 123'51'00_E, "Tagbilaran", "Asia/Manila"};
[[maybe_unused]] constexpr Location gomel_coord{                   52'26'00_N,  30'59'00_E, "Gomel", "Europe/Minsk"};
[[maybe_unused]] constexpr Location ekaterinburg_coord{            56'50'00_N,  60'35'00_E, "Yekaterinburg", "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location vilnyus_coord{                 54'41'00_N,  25'17'00_E, "Vilnius", "Europe/Vilnius"};
[[maybe_unused]] constexpr Location kostomuksha_coord{             64'35'00_N,  30'36'00_E, "Kostomuksha", "Europe/Moscow"};
[[maybe_unused]] constexpr Location almaata_coord{                 43'15'00_N,  76'54'00_E, "Almaty", "Asia/Almaty"};
[[maybe_unused]] constexpr Location kolomyya_coord{                48'31'00_N,  25'02'00_E, "Kolomyia", "Europe/Kiev"};
[[maybe_unused]] constexpr Location samara_coord{                  53'11'00_N,  50'07'00_E, "Samara", "Europe/Samara"};
[[maybe_unused]] constexpr Location chelyabinsk_coord{             55'09'44_N,  61'24'11_E, "Chelyabinsk", "Asia/Yekaterinburg"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tekeli_coord{                  44'51'47_N,  78'45'51_E, "Tekeli, Kazakhstan", "Asia/Almaty"}; // ru Wikipedia
[[maybe_unused]] constexpr Location ustkamenogorsk_coord{          49'59'00_N,  82'37'00_E, "Ust-Kamenogorsk (Oskemen)", "Asia/Almaty"}; // en Wikipedia
[[maybe_unused]] constexpr Location volgograd_coord{               48'41'57_N,  44'28'24_E, "Volgograd", "Europe/Volgograd"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tambov_coord{                  52'43'00_N,  41'26'00_E, "Tambov", "Europe/Moscow"}; // ru Wikipedia
[[maybe_unused]] constexpr Location marsel_coord{                  43'17'47_N,   5'22'12_E, "Marseille", "Europe/Paris"}; // ru Wikipedia
[[maybe_unused]] constexpr Location barcelona_coord{               41'23'00_N,   2'11'00_E, "Barcelona", "Europe/Madrid"}; // en Wikipedia
[[maybe_unused]] constexpr Location madrid_coord{                  40'24'00_N,   3'41'00_W, "Madrid", "Europe/Madrid"}; // ru Wikipedia
[[maybe_unused]] constexpr Location miami_coord{                   25'47'00_N,  80'13'00_W, "Miami", "America/New_York"}; // ru Wikipedia
[[maybe_unused]] constexpr Location cancun_coord{                  21'09'38_N,  86'50'51_W, "Cancun", "America/Cancun"}; // en Wikipedia
[[maybe_unused]] constexpr Location nicosia_coord{                 35'10'21_N,  33'21'54_E, "Nicosia (Cyprus)", "Asia/Nicosia"}; // en Wikipedia


[[maybe_unused]] constexpr Location dummy_coord{0_N, 0_E, "Dummy location"};



} // namespace vp

namespace {
template<typename Out>
inline auto print_deg_min_sec(const Out & out, double degrees) {
    const int sec_total = static_cast<int>(std::round(degrees * 3600.0));
    const int sec = sec_total % 60;
    const int min_remain = (sec_total-sec) / 60;
    const int min = min_remain % 60;
    int deg = (min_remain - min) / 60;
    return fmt::format_to(out, "{}°{:02}′{:02}″", deg, min, sec);
}
}

template<>
struct fmt::formatter<vp::Latitude> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Latitude & lat, FormatContext & ctx) {
        print_deg_min_sec(ctx.out(), std::fabs(lat.latitude));
        return fmt::format_to(ctx.out(), "{}", lat.latitude >= 0 ? 'N' : 'S');
    }
};

template<>
struct fmt::formatter<vp::Longitude> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Longitude & lng, FormatContext & ctx) {
        print_deg_min_sec(ctx.out(), std::fabs(lng.longitude));
        return fmt::format_to(ctx.out(), "{}", lng.longitude >= 0 ? 'E' : 'W');
    }
};

template<>
struct fmt::formatter<vp::Location> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Location & l, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), "{}, {}", l.latitude, l.longitude);
    }
};

#endif // LOCATION_H

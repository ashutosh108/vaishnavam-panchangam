#ifndef LOCATION_H
#define LOCATION_H

#include <cstring>
#include <string_view>
#include <tuple>

namespace vp {

struct Latitude {
    double latitude;
};

struct Longitude {
    double longitude;
};

constexpr double int_deg_min_sec_to_double_degrees(const unsigned long long val) {
    if (val < 1'00 || val > 180'00'00) { throw std::logic_error("Value for degrees must me in 1'00..180'00'00 range"); }
    int deg = static_cast<int>(val / 10'000);
    if (deg > 179) { throw std::logic_error("degrees part in dd'mm'ss must be in 0..179 range"); }
    int min = (val / 100) % 100;
    if (min > 59) { throw std::logic_error("minutes part in dd'mm'ss must be in 0..59 range"); }
    int sec = val % 100;
    if (sec > 59) { throw std::logic_error("seconds part in dd'mm'ss must be in 0..59 range"); }
    return deg + min/60.0 + sec/3600.0;
}

constexpr Latitude operator ""_N(const unsigned long long lat) {
    return Latitude{int_deg_min_sec_to_double_degrees(lat)};
}

constexpr Latitude operator ""_S(const unsigned long long lat) {
    return Latitude{-int_deg_min_sec_to_double_degrees(lat)};
}

constexpr Longitude operator ""_E(const unsigned long long lat) {
    return Longitude{int_deg_min_sec_to_double_degrees(lat)};
}

constexpr Longitude operator ""_W(const unsigned long long lat) {
    return Longitude{-int_deg_min_sec_to_double_degrees(lat)};
}

struct Location
{
    double latitude, longitude;
    const char *timezone_name;
    const char *name;
    explicit constexpr Location(double _latitude=0., double _longitude=0.)
        : latitude(_latitude), longitude(_longitude), timezone_name("UTC"), name("Custom location") {}
    explicit constexpr Location(const char * _name, double _latitude, double _longitude)
        : latitude(_latitude), longitude(_longitude), timezone_name("UTC"), name(_name) {}
    explicit constexpr Location(const char * _name, double _latitude, double _longitude, const char *_timezone_name)
        : latitude(_latitude), longitude(_longitude), timezone_name(_timezone_name), name(_name) {}
    explicit constexpr Location(const char * _name, int lat_deg, int lat_min, double lat_sec,
                                int lng_deg, int lng_min, double lng_sec)
        : latitude(lat_deg + lat_min/60.0 + lat_sec/3600.0),
          longitude(lng_deg + lng_min/60.0 + lng_sec/3600.0),
          timezone_name("UTC"),
          name(_name) {}
    explicit constexpr Location(const char * _name, int lat_deg, int lat_min, double lat_sec,
                                int lng_deg, int lng_min, double lng_sec,
                                const char *_timezone_name)
        : latitude(lat_deg + lat_min/60.0 + lat_sec/3600.0),
          longitude(lng_deg + lng_min/60.0 + lng_sec/3600.0),
          timezone_name(_timezone_name),
          name(_name) {}
    explicit constexpr Location(const char * _name, Latitude lat, Longitude lng,
                                const char *_timezone_name)
        : latitude(lat.latitude),
          longitude(lng.longitude),
          timezone_name(_timezone_name),
          name(_name) {}
};

inline bool operator==(const Location & one, const Location & other) {
    return one.latitude == other.latitude && one.longitude == other.longitude && std::strcmp(one.timezone_name, other.timezone_name) == 0 && std::strcmp(one.name, other.name) == 0;
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
            std::tie(one.latitude, one.longitude, one_tz, one_name) <
            std::tie(two.latitude, two.longitude, two_tz, two_name);
}


[[maybe_unused]] constexpr Location odessa_coord{"Odessa",                                     46'28'00_N,  30'44'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location vinnitsa_coord{"Vinnitsa",                                 49'14'00_N,  28'27'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location kiev_coord{"Kiev",                                         50'27'00_N,  30'31'24_E, "Europe/Kiev"}; // en Wikipedia
[[maybe_unused]] constexpr Location sanfrantsisko_coord{"San Francisco",                       37'46'00_N, 122'24'00_W, "America/Los_Angeles"};
[[maybe_unused]] constexpr Location tiraspol_coord{"Tiraspol",                                 46'51'00_N,  29'36'00_E, "Europe/Chisinau"};
[[maybe_unused]] constexpr Location hmelnitskiy_coord{"Khmelnytskyi",                          49'25'00_N,  26'59'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location kishinev_coord{"Kishinev",                                 47'00'00_N,  28'51'00_E, "Europe/Chisinau"};
[[maybe_unused]] constexpr Location voronezh_coord{"Voronezh",                                 51'40'00_N,  39'12'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location harkov_coord{"Kharkov",                                    50'00'00_N,  36'15'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location habarovsk_coord{"Khabarovsk",                              48'29'00_N, 135'05'00_E, "Asia/Vladivostok"};
[[maybe_unused]] constexpr Location lugansk_coord{"Lugansk",                                   48'34'00_N,  39'20'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location moskva_coord{"Moscow",                                     55'45'00_N,  37'35'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location vrindavan_coord{"Vrindavan",                               28'36'00_N,  77'12'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location losanjeles_coord{"Los Angeles",                            34'03'00_N, 118'14'00_W, "America/Los_Angeles"};
[[maybe_unused]] constexpr Location kalkuta_coord{"Kolkata",                                   22'32'00_N,  88'22'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location spb_coord{"Saint Petersburg",                              59'55'00_N,  30'15'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location freiburg_coord{"Freiburg im Breisgau",                     48'00'00_N,   7'52'00_E, "Europe/Paris"};
[[maybe_unused]] constexpr Location nikolaev_coord{"Nikolaev",                                 46'58'00_N,  32'00'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location ramenskoe_m_obl_coord{"Ramenskoye, Moscow Oblast",         55'34'00_N,  38'13'00_E, "Europe/Moscow"}; // en Wikipedia
[[maybe_unused]] constexpr Location minsk_coord{"Minsk",                                       53'55'00_N,  27'35'00_E, "Europe/Minsk"};
[[maybe_unused]] constexpr Location barnaul_coord{"Barnaul",                                   53'21'00_N,  83'46'00_E, "Asia/Barnaul"};
[[maybe_unused]] constexpr Location newdelhi_coord{"New Delhi",                                28'39'00_N,  77'13'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location dusseldorf_coord{"Dusseldorf",                             51'13'00_N,   6'47'00_E, "Europe/Berlin"};
[[maybe_unused]] constexpr Location koeln_kkd_coord{"Cologne",                                 50'56'11_N,   6'57'10_E, "Europe/Berlin"}; // en Wikipedia
[[maybe_unused]] constexpr Location sochi_coord{"Sochi",                                       43'36'00_N,  39'44'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location novgorod_coord{"Velikiy Novgorod",                         58'33'00_N,  31'16'00_E, "Europe/Moscow"}; // en Wikipedia
[[maybe_unused]] constexpr Location london_coord{"London",                                     51'30'00_N,      7'00_W, "Europe/London"};
[[maybe_unused]] constexpr Location manchester_coord{"Manchester",                             53'30'00_N,   2'13'00_W, "Europe/London"};
[[maybe_unused]] constexpr Location panaji_coord{"Panaji",                                     15'29'00_N,  72'50'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location bombey_coord{"Mumbai",                                     18'58'00_N,  72'50'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location pune_coord{"Pune",                                         18'32'00_N,  73'52'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location simferopol_coord{"Simferopol",                             44'56'00_N,  34'06'00_E, "Europe/Simferopol"};
[[maybe_unused]] constexpr Location manali_coord{"Manali, Himachal Pradesh",                   32'16'00_N,  77'10'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location pyatigorsk_coord{"Pyatigorsk",                             44'02'00_N,  43'03'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location kirov_coord{"Kirov",                                       58'36'00_N,  49'39'00_E, "Europe/Kirov"};
[[maybe_unused]] constexpr Location washington_coord{"Washington, D.C.",                       38'53'00_N,  77'02'00_W, "America/New_York"};
[[maybe_unused]] constexpr Location gokarna_coord{"Gokarna, Karnataka",                        14'33'00_N,  74'19'00_E, "Asia/Kolkata"};
[[maybe_unused]] constexpr Location telaviv_coord{"Tel Aviv",                                  32'04'00_N,  34'47'00_E, "Asia/Jerusalem"};
[[maybe_unused]] constexpr Location tomsk_coord{"Tomsk",                                       56'29'00_N,  84'57'00_E, "Asia/Tomsk"};
[[maybe_unused]] constexpr Location kiel_coord{"Kiel",                                         54'19'00_N,  10'08'00_E, "Europe/Berlin"};
[[maybe_unused]] constexpr Location omsk_coord{"Omsk",                                         54'58'00_N,  73'23'00_E, "Asia/Omsk"};
[[maybe_unused]] constexpr Location tashkent_coord{"Tashkent",                                 41'18'00_N,  68'16'00_E, "Asia/Almaty"};
[[maybe_unused]] constexpr Location dushanbe_coord{"Dushanbe",                                 38'32'12_N,  68'46'48_E, "Asia/Dushanbe"};
[[maybe_unused]] constexpr Location udupi_coord{"Udupi",                                       13'20'27_N,  74'45'06_E, "Asia/Kolkata"}; // Ananteshvara temple
[[maybe_unused]] constexpr Location varshava_coord{"Warsaw",                                   52'13'00_N,  21'02'00_E, "Europe/Warsaw"};
[[maybe_unused]] constexpr Location donetsk_coord{"Donetsk",                                   48'00'00_N,  37'48'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location tbilisi_coord{"Tbilisi",                                   41'43'00_N,  44'47'00_E, "Asia/Tbilisi"};
[[maybe_unused]] constexpr Location suhum_coord{"Sukhum",                                      43'00'00_N,  41'00'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location kremenchug_coord{"Kremenchug",                             49'04'00_N,  33'25'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location puno_coord{"Puno",                                         15'50'00_S,  70'01'00_W, "America/Lima"};
[[maybe_unused]] constexpr Location vladivostok_coord{"Vladivostok",                           43'07'00_N, 131'54'00_E, "Asia/Vladivostok"};
[[maybe_unused]] constexpr Location pernem_coord{"Pernem",                                     15'43'00_N,  73'47'49_E, "Asia/Kolkata"}; // en Wikipedia
[[maybe_unused]] constexpr Location krasnodar_coord{"Krasnodar",                               45'02'00_N,  38'59'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location meadowlake_coord{"Meadow Lake, Canada",                    54'07'00_N, 108'26'00_W, "America/Regina"};
[[maybe_unused]] constexpr Location toronto_coord{"Toronto",                                   42'43'00_N,  79'24'00_W, "America/New_York"};
[[maybe_unused]] constexpr Location fredericton_coord{"Fredericton",                           45'57'00_N,  66'38'00_W, "America/Moncton"};
[[maybe_unused]] constexpr Location perm_coord{"Perm",                                         58'00'00_N,  56'14'00_E, "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location ufa_coord{"Ufa",                                           54'44'00_N,  55'58'00_E, "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location smolensk_coord{"Smolensk",                                 54'46'00_N,  32'02'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location krivoyrog_coord{"Krivoy Rog",                              47'57'00_N,  33'25'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location petropavlovskkamchatskiy_coord{"Petropavlovsk-Kamchatsky", 53'01'00_N, 158'39'00_E, "Asia/Kamchatka"};
[[maybe_unused]] constexpr Location kophangan_coord{"Ko Pha-ngan",                              9'43'00_N, 100'00'00_E, "Asia/Bangkok"};
[[maybe_unused]] constexpr Location denpasar_coord{"Denpasar",                                  8'39'00_S, 115'13'00_E, "Asia/Makassar"};
[[maybe_unused]] constexpr Location mundelein_coord{"Mundelein",                               42'16'00_N,  88'00'00_W, "America/Chicago"};
[[maybe_unused]] constexpr Location bishkek_coord{"Bishkek",                                   42'52'00_N,  74'34'00_E, "Asia/Bishkek"};
[[maybe_unused]] constexpr Location vena_coord{"Vienna",                                       48'12'00_N,  16'22'00_E, "Europe/Vienna"};
[[maybe_unused]] constexpr Location staryyoskol_coord{"Stary Oskol",                           51'17'00_N,  37'50'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location edmonton_coord{"Edmonton",                                 53'32'00_N, 113'30'00_W, "America/Edmonton"};
[[maybe_unused]] constexpr Location novosibirsk_coord{"Novosibirsk",                           55'01'00_N,  82'55'00_E, "Asia/Novosibirsk"};
[[maybe_unused]] constexpr Location erevan_coord{"Yerevan",                                    40'11'00_N,  44'31'00_E, "Asia/Yerevan"};
[[maybe_unused]] constexpr Location stavropol_coord{"Stavropol",                               45'02'00_N,  41'58'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location pokhara_coord{"Pokhara",                                   28'15'00_N,  83'58'00_E, "Asia/Kathmandu"};
[[maybe_unused]] constexpr Location murmansk_coord{"Murmansk",                                 68'58'00_N,  33'05'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location mirnyy_coord{"Mirny, Sakha Republic",                      62'32'00_N, 113'57'00_E, "Asia/Yakutsk"};
[[maybe_unused]] constexpr Location riga_coord{"Riga",                                         56'56'00_N,  24'06'00_E, "Europe/Riga"};
[[maybe_unused]] constexpr Location surgut_coord{"Surgut",                                     61'15'00_N,  73'26'00_E, "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location ryazan_coord{"Ryazan",                                     54'37'00_N,  39'43'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location afiny_coord{"Athens",                                      37'58'00_N,  23'43'00_E, "Europe/Athens"};
[[maybe_unused]] constexpr Location chita_coord{"Chita",                                       52'02'00_N, 113'30'00_E, "Asia/Chita"};
[[maybe_unused]] constexpr Location poltava_coord{"Poltava",                                   49'34'00_N,  34'34'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location kazan_coord{"Kazan",                                       55'47'00_N,  49'06'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location aktau_coord{"Aktau",                                       43'39'00_N,  51'09'00_E, "Asia/Aqtau"};
[[maybe_unused]] constexpr Location aktobe_coord{"Aktobe",                                     50'18'00_N,  57'10'00_E, "Asia/Aqtobe"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tallin_coord{"Tallinn",                                    59'26'00_N,  24'45'00_E, "Europe/Tallinn"};
[[maybe_unused]] constexpr Location jurmala_coord{"Jurmala",                                   56'58'00_N,  23'47'00_E, "Europe/Riga"};
[[maybe_unused]] constexpr Location semikarakorsk_coord{"Semikarakorsk",                       47'31'00_N,  40'48'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location kolombo_coord{"Colombo",                                    6'55'00_N,  79'50'00_E, "Asia/Colombo"};
[[maybe_unused]] constexpr Location ulyanovsk_coord{"Ulyanovsk",                               54'19'00_N,  48'22'00_E, "Europe/Ulyanovsk"};
[[maybe_unused]] constexpr Location tagbilaran_coord{"Tagbilaran",                              9'39'00_N, 123'51'00_E, "Asia/Manila"};
[[maybe_unused]] constexpr Location gomel_coord{"Gomel",                                       52'26'00_N,  30'59'00_E, "Europe/Minsk"};
[[maybe_unused]] constexpr Location ekaterinburg_coord{"Yekaterinburg",                        56'50'00_N,  60'35'00_E, "Asia/Yekaterinburg"};
[[maybe_unused]] constexpr Location vilnyus_coord{"Vilnius",                                   54'41'00_N,  25'17'00_E, "Europe/Vilnius"};
[[maybe_unused]] constexpr Location kostomuksha_coord{"Kostomuksha",                           64'35'00_N,  30'36'00_E, "Europe/Moscow"};
[[maybe_unused]] constexpr Location almaata_coord{"Almaty",                                    43'15'00_N,  76'54'00_E, "Asia/Almaty"};
[[maybe_unused]] constexpr Location kolomyya_coord{"Kolomyia",                                 48'31'00_N,  25'02'00_E, "Europe/Kiev"};
[[maybe_unused]] constexpr Location samara_coord{"Samara",                                     53'11'00_N,  50'07'00_E, "Europe/Samara"};
[[maybe_unused]] constexpr Location chelyabinsk_coord{"Chelyabinsk",                           55'09'44_N,  61'24'11_E, "Asia/Yekaterinburg"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tekeli_coord{"Tekeli, Kazakhstan",                         44'51'47_N,  78'45'51_E, "Asia/Almaty"}; // ru Wikipedia
[[maybe_unused]] constexpr Location ustkamenogorsk_coord{"Ust-Kamenogorsk (Oskemen)",          49'59'00_N,  82'37'00_E, "Asia/Almaty"}; // en Wikipedia
[[maybe_unused]] constexpr Location volgograd_coord{"Volgograd",                               48'41'57_N,  44'28'24_E, "Europe/Volgograd"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tambov_coord{"Tambov",                                     52'43'00_N,  41'26'00_E, "Europe/Moscow"}; // ru Wikipedia
[[maybe_unused]] constexpr Location marsel_coord{"Marseille",                                  43'17'47_N,   5'22'12_E, "Europe/Paris"}; // ru Wikipedia
[[maybe_unused]] constexpr Location barcelona_coord{"Barcelona",                               41'23'00_N,   2'11'00_E, "Europe/Madrid"}; // en Wikipedia
[[maybe_unused]] constexpr Location madrid_coord{"Madrid",                                     40'24'00_N,   3'41'00_W, "Europe/Madrid"}; // ru Wikipedia
[[maybe_unused]] constexpr Location miami_coord{"Miami",                                       25'47'00_N,  80'13'00_W, "America/New_York"}; // ru Wikipedia
[[maybe_unused]] constexpr Location cancun_coord{"Cancun",                                     21'09'38_N,  86'50'51_W, "America/Cancun"}; // en Wikipedia
[[maybe_unused]] constexpr Location nicosia_coord{"Nicosia (Cyprus)",                          35'10'21_N,  33'21'54_E, "Asia/Nicosia"}; // en Wikipedia





} // namespace vp

#endif // LOCATION_H

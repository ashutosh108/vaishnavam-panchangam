#ifndef LOCATION_H
#define LOCATION_H

#include <cmath> // fabs
#include <cstring>
#include "fmt-format-fixed.h"
#include <string_view>
#include <tuple> // for std::tie() in comparison operators.
#include "tz-fixed.h"

namespace vp {

struct Latitude {
    double latitude;
};

bool operator ==(Latitude l1, Latitude l2);
bool operator !=(Latitude l1, Latitude l2);
bool operator <(Latitude l1, Latitude l2);
bool operator <=(Latitude l1, Latitude l2);
bool operator >(Latitude l1, Latitude l2);
bool operator >=(Latitude l1, Latitude l2);

struct Longitude {
    double longitude;
};

bool operator ==(Longitude l1, Longitude l2);
bool operator !=(Longitude l1, Longitude l2);
bool operator <(Longitude l1, Longitude l2);
bool operator <=(Longitude l1, Longitude l2);
bool operator >(Longitude l1, Longitude l2);
bool operator >=(Longitude l1, Longitude l2);

struct Coord {
    Latitude latitude{0.0};
    Longitude longitude{0.0};
};

bool operator ==(Coord c1, Coord c2);
bool operator !=(Coord c1, Coord c2);
bool operator <(Coord c1, Coord c2);
bool operator <=(Coord c1, Coord c2);
bool operator >(Coord c1, Coord c2);
bool operator >=(Coord c1, Coord c2);

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

constexpr Longitude operator+(Longitude l, double delta) {
    return Longitude{l.longitude + delta};
}

constexpr Latitude operator+(Latitude l, double delta) {
    return Latitude{l.latitude + delta};
}

struct Location
{
    Latitude latitude;
    Longitude longitude;
    std::string_view time_zone_name;
    std::string_view name;
    std::string_view country;

    // If true, then the latitude has been decreased until we found location
    // where sunrises/sunsets do exist.
    bool latitude_adjusted = false;

    explicit constexpr Location(Latitude lat = Latitude{0.0}, Longitude lng = Longitude{0.0},
                                const char * _name = "Custom Location",
                                const char * _time_zone_name = "UTC",
                                const char * _country = "Unknown"
                                ) noexcept
        : latitude(lat),
          longitude(lng),
          time_zone_name(_time_zone_name),
          name(_name),
          country(_country) {}
    const date::time_zone * time_zone() const {
        if (!_time_zone) {
            _time_zone = date::locate_zone(time_zone_name);
        }
        return _time_zone;
    }
private:
    mutable const date::time_zone * _time_zone = nullptr;
};

inline bool operator==(const Location & one, const Location & other) {
    return one.latitude.latitude == other.latitude.latitude && one.longitude.longitude == other.longitude.longitude && one.time_zone_name == other.time_zone_name && one.name == other.name;
}

inline bool operator!=(const Location & one, const Location & other) {
    return !(one == other);
}

inline bool operator<(const Location & one, const Location & two) {
    return
            std::tie(one.latitude.latitude, one.longitude.longitude, one.time_zone_name, one.name, one.country) <
            std::tie(two.latitude.latitude, two.longitude.longitude, two.time_zone_name, two.name, two.country);
}

[[maybe_unused]] constexpr Location petropavlovskkamchatskiy_coord{53.0167_N, 158.6500_E, "Petropavlovsk-Kamchatsky", "Asia/Kamchatka", "Russia"};
[[maybe_unused]] constexpr Location habarovsk_coord{               48.4833_N, 135.0833_E, "Khabarovsk", "Asia/Vladivostok", "Russia"};
[[maybe_unused]] constexpr Location vladivostok_coord{             43.1167_N, 131.9000_E, "Vladivostok", "Asia/Vladivostok", "Russia"};
[[maybe_unused]] constexpr Location tagbilaran_coord{               9.6500_N, 123.8500_E, "Tagbilaran", "Asia/Manila", "Philippines"};
[[maybe_unused]] constexpr Location denpasar_coord{                 8.6500_S, 115.2167_E, "Denpasar", "Asia/Makassar", "Indonesia"};
[[maybe_unused]] constexpr Location mirnyy_coord{                  62.5333_N, 113.9500_E, "Mirny", "Asia/Yakutsk", "Russia"};
[[maybe_unused]] constexpr Location chita_coord{                   52.0333_N, 113.5000_E, "Chita", "Asia/Chita", "Russia"};
[[maybe_unused]] constexpr Location kophangan_coord{                9.7167_N, 100.0000_E, "Ko Pha-ngan", "Asia/Bangkok", "Thailand"};
[[maybe_unused]] constexpr Location krasnoyarsk_coord{             56.0119_N,  92.8714_E, "Krasnoyarsk", "Asia/Krasnoyarsk", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location kalkuta_coord{                 22.5333_N,  88.3667_E, "Kolkata", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location novokuznetsk_coord{            53.7333_N,  87.0833_E, "Novokuznetsk", "Asia/Krasnoyarsk", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location tomsk_coord{                   56.4833_N,  84.9500_E, "Tomsk", "Asia/Tomsk", "Russia"};
[[maybe_unused]] constexpr Location pokhara_coord{                 28.2500_N,  83.9667_E, "Pokhara", "Asia/Kathmandu", "Nepal"};
[[maybe_unused]] constexpr Location barnaul_coord{                 53.3500_N,  83.7667_E, "Barnaul", "Asia/Barnaul", "Russia"};
[[maybe_unused]] constexpr Location novosibirsk_coord{             55.0167_N,  82.9167_E, "Novosibirsk", "Asia/Novosibirsk", "Russia"};
[[maybe_unused]] constexpr Location ustkamenogorsk_coord{          49.9833_N,  82.6167_E, "Ust-Kamenogorsk (Oskemen)", "Asia/Almaty", "Kazakhstan"}; // en Wikipedia
[[maybe_unused]] constexpr Location kolombo_coord{                  6.9167_N,  79.8333_E, "Colombo", "Asia/Colombo", "SriLanka"};
[[maybe_unused]] constexpr Location tekeli_coord{                  44.8631_N,  78.7642_E, "Tekeli", "Asia/Almaty", "Kazakhstan"}; // ru Wikipedia
[[maybe_unused]] constexpr Location bengaluru_coord{               12.9667_N,  77.5667_E, "Bengaluru", "Asia/Kolkata", "India"}; // ru Wikipedia
[[maybe_unused]] constexpr Location newdelhi_coord{                28.6500_N,  77.2167_E, "New Delhi", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location vrindavan_coord{               28.6000_N,  77.2000_E, "Vrindavan", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location manali_coord{                  32.2667_N,  77.1667_E, "Manali", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location almaata_coord{                 43.2500_N,  76.9000_E, "Almaty", "Asia/Almaty", "Kazakhstan"};
[[maybe_unused]] constexpr Location udupi_coord{                   13.3408_N,  74.7517_E, "Udupi", "Asia/Kolkata", "India"}; // Ananteshvara temple
[[maybe_unused]] constexpr Location bishkek_coord{                 42.8667_N,  74.5667_E, "Bishkek", "Asia/Bishkek", "Kyrgyzstan"};
[[maybe_unused]] constexpr Location morjim_coord{                  15.6294_N,  73.7358_E, "Morjim", "Asia/Kolkata", "India"}; // en Wikipedia
[[maybe_unused]] constexpr Location gokarna_coord{                 14.5500_N,  74.3167_E, "Gokarna", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location pune_coord{                    18.5333_N,  73.8667_E, "Pune", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location pernem_coord{                  15.7167_N,  73.7969_E, "Pernem", "Asia/Kolkata", "India"}; // en Wikipedia
[[maybe_unused]] constexpr Location surgut_coord{                  61.2500_N,  73.4333_E, "Surgut", "Asia/Yekaterinburg", "Russia"};
[[maybe_unused]] constexpr Location omsk_coord{                    54.9667_N,  73.3833_E, "Omsk", "Asia/Omsk", "Russia"};
[[maybe_unused]] constexpr Location bombey_coord{                  18.9667_N,  72.8333_E, "Mumbai", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location panaji_coord{                  15.4833_N,  72.8333_E, "Panaji", "Asia/Kolkata", "India"};
[[maybe_unused]] constexpr Location kokshetau_coord{               53.2917_N,  69.3917_E, "Kokshetau", "Asia/Almaty", "Kazakhstan"}; // ru Wikipedia
[[maybe_unused]] constexpr Location dushanbe_coord{                38.5367_N,  68.7800_E, "Dushanbe", "Asia/Dushanbe", "Tajikistan"};
[[maybe_unused]] constexpr Location tashkent_coord{                41.3000_N,  68.2667_E, "Tashkent", "Asia/Tashkent", "Uzbekistan"};
[[maybe_unused]] constexpr Location chelyabinsk_coord{             55.1622_N,  61.4031_E, "Chelyabinsk", "Asia/Yekaterinburg", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location ekaterinburg_coord{            56.8333_N,  60.5833_E, "Yekaterinburg", "Asia/Yekaterinburg", "Russia"};
[[maybe_unused]] constexpr Location aktobe_coord{                  50.3000_N,  57.1667_E, "Aktobe", "Asia/Aqtobe", "Kazakhstan"}; // ru Wikipedia
[[maybe_unused]] constexpr Location perm_coord{                    58.0000_N,  56.2333_E, "Perm", "Asia/Yekaterinburg", "Russia"};
[[maybe_unused]] constexpr Location ufa_coord{                     54.7333_N,  55.9667_E, "Ufa", "Asia/Yekaterinburg", "Russia"};
[[maybe_unused]] constexpr Location orenburg_coord{                51.7667_N,  55.1000_E, "Orenburg", "Asia/Yekaterinburg", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location aktau_coord{                   43.6500_N,  51.1500_E, "Aktau", "Asia/Aqtau", "Kazakhstan"};
[[maybe_unused]] constexpr Location samara_coord{                  53.1833_N,  50.1167_E, "Samara", "Europe/Samara", "Russia"};
[[maybe_unused]] constexpr Location kirov_coord{                   58.6000_N,  49.6500_E, "Kirov", "Europe/Kirov", "Russia"};
[[maybe_unused]] constexpr Location ulyanovsk_coord{               54.3167_N,  48.3667_E, "Ulyanovsk", "Europe/Ulyanovsk", "Russia"};
[[maybe_unused]] constexpr Location kazan_coord{                   55.7833_N,  49.1000_E, "Kazan", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location cheboksary_coord{              56.1167_N,  47.2333_E, "Cheboksary", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location tbilisi_coord{                 41.7167_N,  44.7833_E, "Tbilisi", "Asia/Tbilisi", "Georgia"};
[[maybe_unused]] constexpr Location erevan_coord{                  40.1833_N,  44.5167_E, "Yerevan", "Asia/Yerevan", "Armenia"};
[[maybe_unused]] constexpr Location volgograd_coord{               48.6992_N,  44.4733_E, "Volgograd", "Europe/Volgograd", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location pyatigorsk_coord{              44.0333_N,  43.0500_E, "Pyatigorsk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location stavropol_coord{               45.0333_N,  41.9667_E, "Stavropol", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location tambov_coord{                  52.7167_N,  41.4333_E, "Tambov", "Europe/Moscow", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location suhum_coord{                   43.0000_N,  41.0000_E, "Sukhum", "Europe/Moscow", "Abkhazia"};
[[maybe_unused]] constexpr Location kurdzhinovo_coord{             43.9833_N,  40.9558_E, "Kurdzhinovo", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location semikarakorsk_coord{           47.5167_N,  40.8000_E, "Semikarakorsk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location arhangelsk_coord{              64.5500_N,  40.5333_E, "Arkhangelsk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location sochi_coord{                   43.6000_N,  39.7333_E, "Sochi", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location ryazan_coord{                  54.6167_N,  39.7167_E, "Ryazan", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location rostov_na_donu_coord{          47.2406_N,  39.7106_E, "Rostov-on-Don", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location lipetsk_coord{                 52.6167_N,  39.6000_E, "Lipetsk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location lugansk_coord{                 48.5667_N,  39.3333_E, "Lugansk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location voronezh_coord{                51.6667_N,  39.2000_E, "Voronezh", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location krasnodar_coord{               45.0333_N,  38.9833_E, "Krasnodar", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location ramenskoe_m_obl_coord{         55.5667_N,  38.2167_E, "Ramenskoye", "Europe/Moscow", "Russia"}; // en Wikipedia
[[maybe_unused]] constexpr Location staryyoskol_coord{             51.2833_N,  37.8333_E, "Stary Oskol", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location donetsk_coord{                 48.0000_N,  37.8000_E, "Donetsk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location moskva_coord{                  55.7500_N,  37.5833_E, "Moscow", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location dubna_coord{                   56.7333_N,  37.1667_E, "Dubna", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location harkov_coord{                  50.0000_N,  36.2500_E, "Kharkov", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location telaviv_coord{                 32.0667_N,  34.7833_E, "Tel Aviv", "Asia/Jerusalem", "Israel"};
[[maybe_unused]] constexpr Location poltava_coord{                 49.5667_N,  34.5667_E, "Poltava", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location ashkelon_coord{                31.6658_N,  34.5664_E, "Ashkelon", "Asia/Jerusalem", "Israel"}; // ru Wikipedia
[[maybe_unused]] constexpr Location kerch_coord{                   45.3386_N,  36.4681_E, "Kerch", "Europe/Simferopol", "Russia"}; // ru Wikipedia
[[maybe_unused]] constexpr Location simferopol_coord{              44.9333_N,  34.1000_E, "Simferopol", "Europe/Simferopol", "Russia"};
[[maybe_unused]] constexpr Location nicosia_coord{                 35.1725_N,  33.3650_E, "Nicosia", "Asia/Nicosia", "Cyprus"}; // en Wikipedia
[[maybe_unused]] constexpr Location murmansk_coord{                68.9667_N,  33.0833_E, "Murmansk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location kremenchug_coord{              49.0667_N,  33.4167_E, "Kremenchug", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location krivoyrog_coord{               47.9500_N,  33.4167_E, "Krivoy Rog", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location kropyvnytskyi_coord{           48.5000_N,  32.2667_E, "Kropyvnytskyi (Kirovograd)", "Europe/Kiev", "Ukraine"}; // en Wikipedia
[[maybe_unused]] constexpr Location smolensk_coord{                54.7667_N,  32.0333_E, "Smolensk", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location nikolaev_coord{                46.9667_N,  32.0000_E, "Nikolaev", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location novgorod_coord{                58.5500_N,  31.2667_E, "Velikiy Novgorod", "Europe/Moscow", "Russia"}; // en Wikipedia
[[maybe_unused]] constexpr Location gomel_coord{                   52.4333_N,  30.9833_E, "Gomel", "Europe/Minsk", "Belarus"};
[[maybe_unused]] constexpr Location odessa_coord{                  46.4667_N,  30.7333_E, "Odessa", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location kostomuksha_coord{             64.5833_N,  30.6000_E, "Kostomuksha", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location kiev_coord{                    50.4500_N,  30.5233_E, "Kiev", "Europe/Kiev", "Ukraine"}; // en Wikipedia
[[maybe_unused]] constexpr Location velikiy_novgorod_coord{        58.5250_N,  31.2750_E, "Velikiy Novgorod", "Europe/Moscow", "Russia"}; // ru wikipedia
[[maybe_unused]] constexpr Location spb_coord{                     59.9167_N,  30.2500_E, "Saint Petersburg", "Europe/Moscow", "Russia"};
[[maybe_unused]] constexpr Location tiraspol_coord{                46.8500_N,  29.6000_E, "Tiraspol", "Europe/Chisinau", "Transnistria"};
[[maybe_unused]] constexpr Location vinnitsa_coord{                49.2333_N,  28.4500_E, "Vinnitsa", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location kishinev_coord{                47.0000_N,  28.8500_E, "Kishinev", "Europe/Chisinau", "Moldova"};
[[maybe_unused]] constexpr Location minsk_coord{                   53.9167_N,  27.5833_E, "Minsk", "Europe/Minsk", "Belarus"};
[[maybe_unused]] constexpr Location hmelnitskiy_coord{             49.4167_N,  26.9833_E, "Khmelnytskyi", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location rovno_coord{                   50.6197_N,  26.2514_E, "Rovno", "Europe/Kiev", "Ukraine"}; // ru Wikipedia
[[maybe_unused]] constexpr Location vilnyus_coord{                 54.6833_N,  25.2833_E, "Vilnius", "Europe/Vilnius", "Lithuania"};
[[maybe_unused]] constexpr Location kolomyya_coord{                48.5167_N,  25.0333_E, "Kolomyia", "Europe/Kiev", "Ukraine"};
[[maybe_unused]] constexpr Location tallin_coord{                  59.4333_N,  24.7500_E, "Tallinn", "Europe/Tallinn", "Estonia"};
[[maybe_unused]] constexpr Location riga_coord{                    56.9333_N,  24.1000_E, "Riga", "Europe/Riga", "Latvia"};
[[maybe_unused]] constexpr Location grodno_coord{                  53.6667_N,  23.8167_E, "Grodno", "Europe/Minsk", "Belarus"}; // ru Wikipedia
[[maybe_unused]] constexpr Location jurmala_coord{                 56.9667_N,  23.7833_E, "Jurmala", "Europe/Riga", "Latvia"};
[[maybe_unused]] constexpr Location afiny_coord{                   37.9667_N,  23.7167_E, "Athens", "Europe/Athens", "Greece"};
[[maybe_unused]] constexpr Location varshava_coord{                52.2167_N,  21.0333_E, "Warsaw", "Europe/Warsaw", "Poland"};
[[maybe_unused]] constexpr Location vena_coord{                    48.2000_N,  16.3667_E, "Vienna", "Europe/Vienna", "Austria"};
[[maybe_unused]] constexpr Location hitzacker_coord{               53.1333_N,  11.0500_E, "Hitzacker", "Europe/Berlin", "Germany"}; // ru Wikipedia
[[maybe_unused]] constexpr Location oslo_coord{                    59.9111_N,  10.7528_E, "Oslo", "Europe/Oslo", "Norway"}; // ru Wikipedia
[[maybe_unused]] constexpr Location dillingen_an_der_donau_coord{  48.5667_N,  10.4667_E, "Dillingen an der Donau", "Europe/Berlin", "Germany"}; // en Wikipedia
[[maybe_unused]] constexpr Location kiel_coord{                    54.3167_N,  10.1333_E, "Kiel", "Europe/Berlin", "Germany"};
[[maybe_unused]] constexpr Location eppingen_coord{                49.1333_N,   8.9167_E, "Eppingen", "Europe/Berlin", "Germany"}; // en Wikipedia
[[maybe_unused]] constexpr Location freiburg_coord{                48.0000_N,   7.8667_E, "Freiburg im Breisgau", "Europe/Paris", "Germany"};
[[maybe_unused]] constexpr Location biel_bienne_coord{             47.1372_N,   7.2472_E, "Biel/Biennne", "Europe/Zurich", "Switzerland"};
[[maybe_unused]] constexpr Location koeln_kkd_coord{               50.9364_N,   6.9528_E, "Cologne", "Europe/Berlin", "Germany"}; // en Wikipedia
[[maybe_unused]] constexpr Location dusseldorf_coord{              51.2167_N,   6.7833_E, "Dusseldorf", "Europe/Berlin", "Germany"};
[[maybe_unused]] constexpr Location marsel_coord{                  43.2964_N,   5.3700_E, "Marseille", "Europe/Paris", "France"}; // ru Wikipedia
[[maybe_unused]] constexpr Location manchester_coord{              53.5000_N,   2.2167_W, "Manchester", "Europe/London", "UK"};
[[maybe_unused]] constexpr Location barcelona_coord{               41.3833_N,   2.1833_E, "Barcelona", "Europe/Madrid", "Spain"}; // en Wikipedia
[[maybe_unused]] constexpr Location ceuta_coord{                   35.8867_N,   5.3000_W, "Ceuta", "Europe/Madrid", "Spain"}; // ru Wikipedia
[[maybe_unused]] constexpr Location london_coord{                  51.5000_N,   0.1167_W, "London", "Europe/London", "UK"};
[[maybe_unused]] constexpr Location madrid_coord{                  40.4000_N,   3.6833_W, "Madrid", "Europe/Madrid", "Spain"}; // ru Wikipedia
[[maybe_unused]] constexpr Location fredericton_coord{             45.9500_N,  66.6333_W, "Fredericton", "America/Moncton", "Canada"};
[[maybe_unused]] constexpr Location puno_coord{                    15.8333_S,  70.0167_W, "Puno", "America/Lima", "Peru"};
[[maybe_unused]] constexpr Location newyork_coord{                 40.7125_N,  74.0061_W, "New York City", "America/New_York", "USA"}; // en Wikipedia
[[maybe_unused]] constexpr Location philadelphia_coord{            39.9531_N,  75.1633_W, "Philadelphia, PA", "America/New_York", "USA"}; // ru Wikipedia
[[maybe_unused]] constexpr Location washington_coord{              38.8833_N,  77.0333_W, "Washington, D.C.", "America/New_York", "USA"};
[[maybe_unused]] constexpr Location toronto_coord{                 42.7167_N,  79.4000_W, "Toronto", "America/New_York", "Canada"};
[[maybe_unused]] constexpr Location miami_coord{                   25.7833_N,  80.2167_W, "Miami", "America/New_York", "USA"}; // ru Wikipedia
[[maybe_unused]] constexpr Location cancun_coord{                  21.1606_N,  86.8475_W, "Cancun", "America/Cancun", "Mexico"}; // en Wikipedia
[[maybe_unused]] constexpr Location mundelein_coord{               42.2667_N,  88.0000_W, "Mundelein", "America/Chicago", "USA"};
[[maybe_unused]] constexpr Location meadowlake_coord{              54.1167_N, 108.4333_W, "Meadow Lake", "America/Regina", "Canada"};
[[maybe_unused]] constexpr Location edmonton_coord{                53.5333_N, 113.5000_W, "Edmonton", "America/Edmonton", "Canada"};
[[maybe_unused]] constexpr Location losanjeles_coord{              34.0333_N, 118.2667_W, "Los Angeles", "America/Los_Angeles", "USA"}; // ru Wikipedia
[[maybe_unused]] constexpr Location sanfrantsisko_coord{           37.7667_N, 122.4000_W, "San Francisco", "America/Los_Angeles", "USA"};
[[maybe_unused]] constexpr Location dummy_coord{0.0_N, 0.0_E, "Dummy location"};
[[maybe_unused]] constexpr Location sample_location{0.0_N, 0.0_E, "Sample location"};

} // namespace vp

template<>
struct fmt::formatter<vp::Latitude> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Latitude & lat, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), FMT_STRING("{:.4f}"), lat.latitude);
    }
};

template<>
struct fmt::formatter<vp::Longitude> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Longitude & lng, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), FMT_STRING("{:.4f}"), lng.longitude);
    }
};

template<>
struct fmt::formatter<vp::Location> : fmt::formatter<string_view> {
    template<typename FormatContext>
    auto format(const vp::Location & l, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), FMT_STRING("{}, {}"), l.latitude, l.longitude);
    }
};

#endif // LOCATION_H

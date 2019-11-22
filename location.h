#ifndef LOCATION_H
#define LOCATION_H


struct Location
{
    double latitude, longitude;
    const char *timezone_name;
    explicit constexpr Location(double _latitude, double _longitude)
        : latitude(_latitude), longitude(_longitude), timezone_name("UTC") {}
    explicit constexpr Location(double _latitude, double _longitude, const char *_timezone_name)
        : latitude(_latitude), longitude(_longitude), timezone_name(_timezone_name) {}
    explicit constexpr Location(int lat_deg, int lat_min, double lat_sec,
                                int lng_deg, int lng_min, double lng_sec)
        : latitude(lat_deg + lat_min/60.0 + lat_sec/3600.0),
          longitude(lng_deg + lng_min/60.0 + lng_sec/3600.0),
          timezone_name("UTC") {}
    explicit constexpr Location(int lat_deg, int lat_min, double lat_sec,
                                int lng_deg, int lng_min, double lng_sec,
                                const char *_timezone_name)
        : latitude(lat_deg + lat_min/60.0 + lat_sec/3600.0),
          longitude(lng_deg + lng_min/60.0 + lng_sec/3600.0),
          timezone_name(_timezone_name) {}
};

[[maybe_unused]] constexpr Location odessa_coord{46, 28, 0, 30, 44, 0, "Europe/Kiev"};
[[maybe_unused]] constexpr Location vinnitsa_coord{49, 14, 0, 28, 27, 0, "Europe/Kiev"};
[[maybe_unused]] constexpr Location kiev_coord{50, 26, 0, 30, 37, 0, "Europe/Kiev"};
[[maybe_unused]] constexpr Location sanfrantsisko_coord{37, 46, 0, -122, -24, 0};
[[maybe_unused]] constexpr Location tiraspol_coord{46, 51, 0, 29, 36, 0};
[[maybe_unused]] constexpr Location hmelnitskiy_coord{49, 25, 0, 26, 59, 0};
[[maybe_unused]] constexpr Location kishinev_coord{47, 0, 0, 28, 51, 0};
[[maybe_unused]] constexpr Location voronezh_coord{51, 40, 0, 39, 12, 0};
[[maybe_unused]] constexpr Location harkov_coord{50, 0, 0, 36, 15, 0};
[[maybe_unused]] constexpr Location habarovsk_coord{48, 29, 0, 135, 5, 0};
[[maybe_unused]] constexpr Location lugansk_coord{48, 34, 0, 39, 20, 0};
[[maybe_unused]] constexpr Location moskva_coord{55, 45, 0, 37, 35, 0};
[[maybe_unused]] constexpr Location vrindavan_coord{28, 36, 0, 77, 12, 0};
[[maybe_unused]] constexpr Location losanjeles_coord{34, 3, 0, -118, -14, 0};
[[maybe_unused]] constexpr Location kalkuta_coord{22, 32, 0, 88, 22, 0};
[[maybe_unused]] constexpr Location spb_coord{59, 55, 0, 30, 15, 0};
[[maybe_unused]] constexpr Location freiburg_coord{48, 0, 0, 7, 52, 0};
[[maybe_unused]] constexpr Location nikolaev_coord{46, 58, 0, 32, 0, 0};
[[maybe_unused]] constexpr Location ramenskoe_m_obl_coord{55, 39, 0, 37, 57, 0};
[[maybe_unused]] constexpr Location minsk_coord{53, 55, 0, 27, 35, 0};
[[maybe_unused]] constexpr Location barnaul_coord{53, 21, 0, 83, 46, 0};
[[maybe_unused]] constexpr Location newdelhi_coord{28, 39, 0, 77, 13, 0};
[[maybe_unused]] constexpr Location dusseldorf_coord{51, 13, 0, 6, 47, 0};
[[maybe_unused]] constexpr Location koeln_kkd_coord{50, 58, 0, 6, 56, 0};
[[maybe_unused]] constexpr Location sochi_coord{43, 36, 0, 39, 44, 0};
[[maybe_unused]] constexpr Location novgorod_coord{58, 31, 0, 30, 17, 0};
[[maybe_unused]] constexpr Location london_coord{51, 30, 0, 0, -7, 0};
[[maybe_unused]] constexpr Location manchester_coord{53, 30, 0, -2, -13, 0};
[[maybe_unused]] constexpr Location panaji_coord{15, 29, 0, 72, 50, 0};
[[maybe_unused]] constexpr Location bombey_coord{18, 58, 0, 72, 50, 0};
[[maybe_unused]] constexpr Location pune_coord{18, 32, 0, 73, 52, 0};
[[maybe_unused]] constexpr Location simferopol_coord{44, 56, 0, 34, 6, 0};
[[maybe_unused]] constexpr Location manali_coord{32, 16, 0, 77, 10, 0};
[[maybe_unused]] constexpr Location pyatigorsk_coord{44, 2, 0, 43, 3, 0};
[[maybe_unused]] constexpr Location kirov_coord{58, 36, 0, 49, 39, 0};
[[maybe_unused]] constexpr Location washington_coord{38, 53, 0, -77, -2, 0};
[[maybe_unused]] constexpr Location gokarna_coord{14, 33, 0, 74, 19, 0};
[[maybe_unused]] constexpr Location telaviv_coord{32, 4, 0, 34, 47, 0};
[[maybe_unused]] constexpr Location tomsk_coord{56, 29, 0, 84, 57, 0};
[[maybe_unused]] constexpr Location kiel_coord{54, 19, 0, 10, 8, 0};
[[maybe_unused]] constexpr Location omsk_coord{54, 58, 0, 73, 23, 0};
[[maybe_unused]] constexpr Location tashkent_coord{41, 18, 0, 68, 16, 0};
[[maybe_unused]] constexpr Location udupi_coord{13, 59, 0, 74, 75, 0};
[[maybe_unused]] constexpr Location varshava_coord{52, 13, 0, 21, 2, 0};
[[maybe_unused]] constexpr Location donetsk_coord{48, 0, 0, 37, 48, 0};
[[maybe_unused]] constexpr Location tbilisi_coord{41, 43, 0, 44, 47, 0};
[[maybe_unused]] constexpr Location suhum_coord{43, 0, 0, 41, 0, 0};
[[maybe_unused]] constexpr Location kremenchug_coord{49, 4, 0, 33, 25, 0};
[[maybe_unused]] constexpr Location puno_coord{-15, -50, 0, -70, -1, 0};
[[maybe_unused]] constexpr Location vladivostok_coord{43, 7, 0, 131, 54, 0};
[[maybe_unused]] constexpr Location pernem_coord{15, 72, 0, 73, 80, 0};
[[maybe_unused]] constexpr Location krasnodar_coord{45, 2, 0, 38, 59, 0};
[[maybe_unused]] constexpr Location meadowlake_coord{54, 7, 0, -108, -26, 0};
[[maybe_unused]] constexpr Location toronto_coord{42, 43, 0, -79, -24, 0};
[[maybe_unused]] constexpr Location frederikton_coord{45, 57, 0, -66, -38, 0};
[[maybe_unused]] constexpr Location perm_coord{58, 0, 0, 56, 14, 0};
[[maybe_unused]] constexpr Location ufa_coord{54, 44, 0, 55, 58, 0};
[[maybe_unused]] constexpr Location smolensk_coord{54, 46, 0, 32, 2, 0};
[[maybe_unused]] constexpr Location krivoyrog_coord{47, 57, 0, 33, 25, 0};
[[maybe_unused]] constexpr Location petropavlovskkamchatskiy_coord{53, 1, 0, 158, 39, 0};
[[maybe_unused]] constexpr Location kophangan_coord{9, 43, 0, 100, 0, 0};
[[maybe_unused]] constexpr Location denpasar_coord{-8, -39, 0, 115, 13, 0};
[[maybe_unused]] constexpr Location mundelein_coord{42, 16, 0, -88, 0, 0};
[[maybe_unused]] constexpr Location bishkek_coord{42, 52, 0, 74, 34, 0};
[[maybe_unused]] constexpr Location vena_coord{48, 12, 0, 16, 22, 0};
[[maybe_unused]] constexpr Location staryyoskol_coord{51, 17, 0, 37, 50, 0};
[[maybe_unused]] constexpr Location edmonton_coord{53, 32, 0, -113, -30, 0};
[[maybe_unused]] constexpr Location novosibirsk_coord{55, 1, 0, 82, 55, 0};
[[maybe_unused]] constexpr Location erevan_coord{40, 11, 0, 44, 31, 0};
[[maybe_unused]] constexpr Location stavropol_coord{45, 2, 0, 41, 58, 0};
[[maybe_unused]] constexpr Location pokhara_coord{28, 15, 0, 83, 58, 0};
[[maybe_unused]] constexpr Location murmansk_coord{68, 58, 0, 33, 5, 0};
[[maybe_unused]] constexpr Location mirnyy_coord{62, 32, 0, 113, 57, 0};
[[maybe_unused]] constexpr Location riga_coord{56, 56, 0, 24, 6, 0};
[[maybe_unused]] constexpr Location surgut_coord{61, 15, 0, 73, 26, 0};
[[maybe_unused]] constexpr Location ryazan_coord{54, 37, 0, 39, 43, 0};
[[maybe_unused]] constexpr Location afiny_coord{37, 58, 0, 23, 43, 0};
[[maybe_unused]] constexpr Location chita_coord{52, 2, 0, 113, 30, 0};
[[maybe_unused]] constexpr Location poltava_coord{49, 34, 0, 34, 34, 0};
[[maybe_unused]] constexpr Location kazan_coord{55, 47, 0, 49, 6, 0};
[[maybe_unused]] constexpr Location aktau_coord{43, 39, 0, 51, 9, 0};
[[maybe_unused]] constexpr Location tallin_coord{59, 26, 0, 24, 45, 0};
[[maybe_unused]] constexpr Location yurmala_coord{56, 58, 0, 23, 47, 0};
[[maybe_unused]] constexpr Location semikarakorsk_coord{47, 31, 0, 40, 48, 0};
[[maybe_unused]] constexpr Location kolombo_coord{6, 55, 0, 79, 50, 0};
[[maybe_unused]] constexpr Location ulyanovsk_coord{54, 19, 0, 48, 22, 0};
[[maybe_unused]] constexpr Location tagbilaran_coord{9, 39, 0, 123, 51, 0};
[[maybe_unused]] constexpr Location gomel_coord{52, 26, 0, 30, 59, 0};
[[maybe_unused]] constexpr Location ekaterinburg_coord{56, 50, 0, 60, 35, 0};
[[maybe_unused]] constexpr Location vilnyus_coord{54, 41, 0, 25, 17, 0};
[[maybe_unused]] constexpr Location kostomuksha_coord{64, 35, 0, 30, 36, 0};
[[maybe_unused]] constexpr Location almaata_coord{43, 15, 0, 76, 54, 0};
[[maybe_unused]] constexpr Location kolomyya_coord{48, 31, 0, 25, 2, 0};
[[maybe_unused]] constexpr Location samara_coord{53, 11, 0, 50, 7, 0};
[[maybe_unused]] constexpr Location chelyabinsk_coord{55, 9, 44, 61, 24, 11}; // ru Wikipedia
[[maybe_unused]] constexpr Location tekeli_coord{44, 51, 47, 78, 45, 51}; // ru Wikipedia
[[maybe_unused]] constexpr Location volgograd_coord{48, 41, 57, 44, 28, 24}; // ru Wikipedia
[[maybe_unused]] constexpr Location tambov_coord{52, 43, 0, 41, 26, 0}; // ru Wikipedia
[[maybe_unused]] constexpr Location marsel_coord{43, 17, 47, 5, 22, 12}; // ru Wikipedia
[[maybe_unused]] constexpr Location madrid_coord{40, 24, 0, -3, -41, -0}; // ru Wikipedia
[[maybe_unused]] constexpr Location miami_coord{25, 47, 0, -80, -13, -0}; // ru Wikipedia

#endif // LOCATION_H

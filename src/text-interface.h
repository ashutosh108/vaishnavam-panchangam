#ifndef VP_SRC_TEXT_INTERFACE_H
#define VP_SRC_TEXT_INTERFACE_H

#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>

#include "location.h"
#include "tz-fixed.h"

namespace vp::text_ui {

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir) */
void change_to_data_dir(const char* argv0);

date::year_month_day parse_ymd(const char *s);
void calc_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o=std::cout);
void calc_one(date::year_month_day base_date, const char * location_name, std::ostream &o=std::cout);
void print_detail_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o=std::cout);
void print_detail_one(date::year_month_day base_date, const char * location_name, std::ostream &o=std::cout);
void calc_all(date::year_month_day d, std::ostream &o=std::cout);

namespace detail {

class LocationDb {
public:
    struct NamedCoord {
        const char *name;
        Location coord;
    };

    static const std::vector<NamedCoord> & locations() {
        static std::vector<NamedCoord> locations_ {
            { "udupi", udupi_coord },
            { "gokarna", gokarna_coord },
            { "newdelhi", newdelhi_coord },
            { "manali", manali_coord },
            { "kalkuta", kalkuta_coord },
            { "aktau", aktau_coord },
            { "perm", perm_coord },
            { "ufa", ufa_coord },
            { "ekaterinburg", ekaterinburg_coord },
            { "surgut", surgut_coord },
            { "chelyabinsk", chelyabinsk_coord },
            { "bishkek", bishkek_coord },
            { "almaata", almaata_coord },
            { "tekeli", tekeli_coord },
            { "omsk", omsk_coord },
            { "novosibirsk", novosibirsk_coord },
            { "barnaul", barnaul_coord },
            { "tomsk", tomsk_coord },
            { "kophangan", kophangan_coord },
            { "denpasar", denpasar_coord },
            { "mirnyy", mirnyy_coord },
            { "habarovsk", habarovsk_coord },
            { "vladivostok", vladivostok_coord },
            { "petropavlovskkamchatskiy", petropavlovskkamchatskiy_coord },
            { "erevan", erevan_coord },
            { "tbilisi", tbilisi_coord },
            { "samara", samara_coord },
            { "volgograd", volgograd_coord },
            { "ulyanovsk", ulyanovsk_coord },
            { "pyatigorsk", pyatigorsk_coord },
            { "stavropol", stavropol_coord },
            { "semikarakorsk", semikarakorsk_coord },
            { "krasnodar", krasnodar_coord },
            { "simferopol", simferopol_coord },
            { "donetsk", donetsk_coord },
            { "staryyoskol", staryyoskol_coord },
            { "voronezh", voronezh_coord },
            { "tambov", tambov_coord },
            { "kazan", kazan_coord },
            { "kirov", kirov_coord },
            { "ryazan", ryazan_coord },
            { "moskva", moskva_coord },
            { "spb", spb_coord },
            { "murmansk", murmansk_coord },
            { "kostomuksha", kostomuksha_coord },
            { "smolensk", smolensk_coord },
            { "gomel", gomel_coord },
            { "minsk", minsk_coord },
            { "harkov", harkov_coord },
            { "poltava", poltava_coord },
            { "kremenchug", kremenchug_coord },
            { "krivoyrog", krivoyrog_coord },
            { "kiev", kiev_coord },
            { "nikolaev", nikolaev_coord },
            { "odessa", odessa_coord },
            { "kolomyya", kolomyya_coord },
            { "kishinev", kishinev_coord },
            { "riga", riga_coord },
            { "yurmala", yurmala_coord },
            { "tallin", tallin_coord },
            { "vilnyus", vilnyus_coord },
            { "varshava", varshava_coord },
            { "vena", vena_coord },
            { "marsel", marsel_coord },
            { "madrid", madrid_coord },
            { "london", london_coord },
            { "frederikton", frederikton_coord },
            { "toronto", toronto_coord },
            { "miami", miami_coord },
            { "meadowlake", meadowlake_coord },
        };
        return locations_;
    }
    static std::optional<Location> find_coord(const char *location_name);
};

std::filesystem::path determine_exe_dir(const char* argv0);
std::filesystem::path determine_working_dir(const char* argv0);

} // namespace detail

} // namespace vp::text_ui

#endif // VP_SRC_TEXT_INTERFACE_H

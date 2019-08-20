#include <algorithm>
#include <iostream>
#include <chrono>
#include <cstring>
#include <tuple>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "calc.h"
#include "swe.h"
#include "swe_time.h"
#include "vrata_detail.h"

void print_usage() {
    std::cout << "USAGE:\n";
    std::cout << "ekadashi-calculator YYYY-MM-DD latitude longitude\n";
    std::cout << "ekadashi-calculator YYYY-MM-DD location-name\n";
    std::cout << "\n";
    std::cout << "    latitude and longitude are given as decimal degrees (e.g. 30.7)\n";
}

std::tuple<int, int, int> parse_ymd(const char *s) {
    std::tm tm{};
    std::istringstream stream{s};
    stream >> std::get_time(&tm, "%Y-%m-%d");
    return std::tuple<int, int, int>{tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday};
}

struct NamedCoord {
    const char *name;
    Coord coord;
};

static std::vector<NamedCoord> locations {
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
    { "ekaterinburg", ekaterinburg_coord },
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
    { "minsk", minsk_coord },
    { "gomel", gomel_coord },
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

std::optional<Coord> find_coord(const char *location_name) {
    auto found = std::find_if(
                std::begin(locations),
                std::end(locations),
                [=](auto named_coord){
                    return strcmp(named_coord.name, location_name) == 0;
                }
    );
    if (found == std::end(locations)) return std::nullopt;
    return found->coord;
}

void calc_one(Date base_date, const char *location_name, Coord coord) {
    auto vrata = Calc{coord}.find_next_vrata(base_date);
    Vrata_Detail vd{*vrata, coord};
    std::cout << location_name << '\n' << vd << "\n\n";
}

void calc_one(Date base_date, const char * location_name) {
    std::optional<Coord> coord = find_coord(location_name);
    if (!coord) {
        std::cerr << "Location not found: '" << location_name << "'\n";
        return;
    }
    calc_one(base_date, location_name, *coord);
}

void print_detail_one(Date base_date, const char *location_name, Coord coord) {
    std::cout << location_name << ' ' << base_date << '\n';
    std::cout << "<to be implemented>\n";
    Calc calc{coord};
    auto sunrise = calc.swe.get_sunrise(Swe_Time{base_date});
    if (sunrise) {
        auto arunodaya_info = calc.get_arunodaya(*sunrise);
        if (arunodaya_info) {
            auto [arunodaya, arunodaya_half_ghatika_before] = *arunodaya_info;
            std::cout << "arunodaya-1/2ghatika: " << arunodaya_half_ghatika_before
                    << ' ' << calc.swe.get_tithi(arunodaya_half_ghatika_before) << '\n';
            std::cout << "arunodaya: " << arunodaya
                    << ' ' << calc.swe.get_tithi(arunodaya) << '\n';
        }
        std::cout << "sunrise: " << *sunrise << ' ' << calc.swe.get_tithi(*sunrise) << '\n';
    }
}

void print_detail_one(Date base_date, const char * location_name) {
    std::optional<Coord> coord = find_coord(location_name);
    if (!coord) {
        std::cerr << "Location not found: '" << location_name << "'\n";
        return;
    }
    print_detail_one(base_date, location_name, *coord);
}

void calc_all(Date d) {
    for (auto &l : locations) {
        calc_one(d, l.name, l.coord);
    }
}

int main(int argc, char *argv[])
{
    if (argc-1 >= 1 && strcmp(argv[1], "-d") == 0) {
        if (argc-1 != 3) {
            print_usage();
            exit(-1);
        }
        auto [y, m, d] = parse_ymd(argv[2]);
        Date base_date{y, m, d};
        const char * const location_name = argv[3];
        print_detail_one(base_date, location_name);
    } else {
        if (argc-1 != 1 && argc-1 != 2 && argc-1 != 3) {
            print_usage();
            exit(-1);
        }

        auto [y, m, d] = parse_ymd(argv[1]);
        Date base_date{y, m, d};
        if (argc-1 <= 1) {
            calc_all(base_date);
        } else if (argc-1 == 2) {
            const char * const location_name = argv[2];
            calc_one(base_date, location_name);
        } else {
            double latitude = std::stod(argv[2]);
            double longitude = std::stod(argv[3]);
            calc_one(base_date, "<custom location>", Coord{latitude, longitude});
        }
    }
}

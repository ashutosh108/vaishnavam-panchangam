#include <iostream>
#include <chrono>
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
    std::cout << "\n";
    std::cout << "    latitude and longitude are given as decimal degrees (e.g. 30.7)\n";
}

std::tuple<int, int, int> parse_ymd(const char *s) {
    std::tm tm{};
    std::istringstream stream{s};
    stream >> std::get_time(&tm, "%Y-%m-%d");
    return std::tuple<int, int, int>{tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday};
}

void calc_all(Date d) {
    std::vector<std::pair<std::string, Coord>> locations {
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
        { "mayami", mayami_coord },
        { "meadowlake", meadowlake_coord },
    };
    for (auto &l : locations) {
        auto vrata = Calc{l.second}.find_next_vrata(d);
        Vrata_Detail vd{*vrata, l.second};
        std::cout << l.first << '\n' << vd << "\n\n";
    }

}

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 4) {
        print_usage();
        exit(-1);
    }

    auto [y, m, d] = parse_ymd(argv[1]);
    Date base_date{y, m, d};
    if (argc <= 2) {
        calc_all(base_date);
        return 0;
    }
    double latitude = std::stod(argv[2]);
    double longitude = std::stod(argv[3]);

    std::cout << "base date: " << base_date << '\n';

    Coord coord{latitude, longitude};
    auto vrata = Calc{coord}.find_next_vrata(base_date);
    if (vrata) {
        std::cout << "Date:        " << vrata->date << '\n';
    } else {
        std::cout << "Can't find ekadashi sunrise\n";
    }

}

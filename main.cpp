#include <iostream>
#include <chrono>
#include <tuple>
#include <sstream>
#include <iomanip>

#include "calc.h"
#include "swe.h"
#include "swe_time.h"

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

int main(int argc, char *argv[])
{
    if (argc < 1+3) {
        print_usage();
        exit(-1);
    }

    auto [y, m, d] = parse_ymd(argv[1]);
    double latitude = std::stod(argv[2]);
    double longitude = std::stod(argv[3]);

    Date base_date{y, m, d};
    std::cout << "base date: " << base_date << '\n';

    auto vrata = Calc{}.find_next_vrata(base_date, Coord{latitude, longitude});
    if (vrata) {
        std::cout << "Date:        " << vrata->date << '\n';
    } else {
        std::cout << "Can't find ekadashi sunrise\n";
    }

}

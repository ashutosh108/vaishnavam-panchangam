#include <iostream>
#include <chrono>
#include <tuple>
#include <sstream>
#include <iomanip>

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

    auto midnight = Swe_Time{y, m, d};

    double latitude = std::stod(argv[2]);
    double longitude = std::stod(argv[3]);

    std::cout << "JD     : " << std::fixed << std::setprecision(15) << midnight.as_julian_days() << '\n';
    auto sunrise = Swe{}.get_sunrise(midnight, latitude, longitude);
    std::cout << "Sunrise: " << *sunrise << '\n';
}

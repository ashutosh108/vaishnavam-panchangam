#include <iostream>
#include <chrono>
#include <tuple>
#include <sstream>
#include <iomanip>
#include "swephexp.h"

#include "swe_time.h"

const double latitude=50.45, longitude=30.523333;

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

double get_sunrise(double jd, double latitude, double longitude) {
    // calculation flag for Hindu risings/settings

    int rsmi = SE_CALC_RISE | SE_BIT_HINDU_RISING;
         // or SE_CALC_RISE | SE_BIT_DISC_CENTER | SE_BIT_NO_REFRACTION | SE_BIT_GEOCTR_NO_ECL_LAT;
    double geopos[3] = {longitude, latitude, 0};
    const double atmospheric_pressure = 1013.25;
    const double atmospheric_temperature = 15;
    double trise;
    char serr[AS_MAXCH];
    int return_code = swe_rise_trans(jd, SE_SUN, nullptr, SEFLG_SWIEPH, rsmi, geopos,
                                     atmospheric_pressure, atmospheric_temperature, &trise, serr);
    return trise;
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
    auto sunrise = Swe_Time{get_sunrise(midnight.as_julian_days(), latitude, longitude)};
    std::cout << "Sunrise: " << sunrise << '\n';
}

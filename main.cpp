#include <iostream>
#include <chrono>
#include <tuple>
#include <sstream>
#include <iomanip>
#include "swephexp.h"

const double latitude=50.45, longitude=30.523333;

void print_usage() {
    std::cout << "ekadashi-calculator YYYY-MM-DD\n";
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
    if (argc < 2) {
        print_usage();
        exit(-1);
    }

    auto [y, m, d] = parse_ymd(argv[1]);

    std::cout << y << '-' << m << '-' << d << '\n';
    swe_set_ephe_path(nullptr);
    double jd = swe_julday(y, m, d, 0, SE_GREG_CAL);
    std::cout << "JD     : " << std::fixed << std::setprecision(15) << jd << '\n';
    double sunrise = get_sunrise(jd, latitude, longitude);
    std::cout << "Sunrise: " << sunrise << '\n';
    int year, month, day;
    double hour;
    swe_revjul(sunrise, SE_GREG_CAL, &year, &month, &day, &hour);
    std::cout << year << '-' << month << '-' << day << ' ' << hour << '\n';
}

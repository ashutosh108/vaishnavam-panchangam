#include "location.h"

#include <cmath> // std::round
#include <iomanip>

namespace vp {

namespace {
inline void print_deg_min_sec(std::ostream & o, double degrees) {
    const int sec_total = static_cast<int>(std::round(degrees * 3600.0));
    const int sec = sec_total % 60;
    const int min_remain = (sec_total-sec) / 60;
    const int min = min_remain % 60;
    int deg = (min_remain - min) / 60;
    constexpr std::string_view deg_str = "°";
    constexpr std::string_view min_str = "′";
    constexpr std::string_view sec_str = "″";
    o << std::setfill('0') << deg << deg_str << std::setw(2) << min << min_str << std::setw(2) << sec << sec_str;
}
}

std::ostream & operator <<(std::ostream & o, const Latitude & latitude) {
    char n_or_s = (latitude.latitude >= 0 ? 'N' : 'S');
    print_deg_min_sec(o, std::abs(latitude.latitude));
    return o << n_or_s;
}

std::ostream & operator <<(std::ostream & o, const Longitude & longitude) {
    char e_or_w = (longitude.longitude >= 0 ? 'E' : 'W');
    print_deg_min_sec(o, std::abs(longitude.longitude));
    return o << e_or_w;
}

std::ostream &operator <<(std::ostream &o, const Location &location)
{
    return o << location.latitude << ", " << location.longitude;
}

} // namespace vp

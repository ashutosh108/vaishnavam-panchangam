#include <optional>

#include "calc.h"
#include "swe.h"

std::optional<Swe_Time> Calc::find_next_ekadashi_sunrise(Swe_Time after, double latitude, double longitude)
{
    Swe s;
    int max_days_left = 16;
    std::optional<Swe_Time> sunrise = after;
    while ((sunrise = s.get_sunrise(*sunrise, latitude, longitude)).has_value() && max_days_left--) {
        double tithi = s.get_tithi(*sunrise);
        if (tithi >= 10 && tithi < 11) {
            return sunrise;
        }
        sunrise = Swe_Time{sunrise->as_julian_days()+0.1};
    }
    return {};
}

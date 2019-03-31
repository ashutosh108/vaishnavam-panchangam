#include <cassert>
#include <optional>

#include "calc.h"
#include "swe.h"

Calc::Calc(Coord coord):swe{coord} {}

std::optional<Swe_Time> Calc::find_next_ekadashi_sunrise(Swe_Time after) const
{
    int max_days_left = 16;
    std::optional<Swe_Time> sunrise = after;
    while ((sunrise = swe.get_sunrise(*sunrise)).has_value() && max_days_left--) {
        if (swe.get_tithi(*sunrise).is_ekadashi()) {
            return sunrise;
        }
        sunrise = Swe_Time{sunrise->as_julian_days()+0.1};
    }
    return {};
}

std::optional<Vrata> Calc::find_next_vrata(Date after) const
{
    auto sunrise = find_next_ekadashi_sunrise(Swe_Time{after});
    if (sunrise) {
        auto arunodaya_info = get_arunodaya(*sunrise);
        if (!arunodaya_info.has_value()) { return{}; }
        auto [arunodaya, ardha_ghatika_before_arundaya] = *arunodaya_info;
        auto tithi_arunodaya = swe.get_tithi(arunodaya);
        Swe_Time back_24hrs{sunrise->as_julian_days()-1};
        auto prev_sunset = swe.get_sunset(back_24hrs);
        Vrata_Type type = Vrata_Type::Ekadashi;
        if ((tithi_arunodaya.is_dashami())) {
            // purva-viddha Ekadashi, get next sunrise
            sunrise = swe.get_sunrise(Swe_Time{sunrise->as_julian_days()+0.1});
            if (!sunrise) { return {}; }
        } else {
            Tithi tithi_ardha_ghatika_before_arunodaya = swe.get_tithi(ardha_ghatika_before_arundaya);
            if (tithi_ardha_ghatika_before_arunodaya.is_dashami()) {
                type = Vrata_Type::Sandigdha_Ekadashi;
                // Sandigdha (almost purva-viddha Ekadashi), get next sunrise
                sunrise = swe.get_sunrise(Swe_Time{sunrise->as_julian_days()+0.1});
                if (!sunrise) { return {}; }
            }
        }


        // Adjust to make sure that Vrata data is correct in local timezone
        // (as opposed to *sunrise which is in UTC). We do this by adding an hour
        // for every 30 degrees of eastern latitude (24 hours for 360 degrees).
        // This could give wrong date if actual local timezone is quite
        // different from the "natural" timezone. But until we support proper
        // timezone, this should work for most cases.
        double adjustment_in_days = swe.coord.longitude * (1.0/360);
        Swe_Time local_sunrise{sunrise->as_julian_days()+adjustment_in_days};

        Tithi sunrise_tithi = swe.get_tithi(*sunrise);

        Date d{local_sunrise.as_date()};
        return Vrata{type,
                    d, *sunrise, sunrise_tithi, *prev_sunset, arunodaya, tithi_arunodaya};
    }
    return {};
}

std::optional<Swe_Time> Calc::get_prev_sunset(Swe_Time const sunrise) const {
    Swe_Time back_24hrs{sunrise.as_julian_days()-1};
    return swe.get_sunset(back_24hrs);
}

Swe_Time proportional_time(Swe_Time const t1, Swe_Time const t2, double const proportion) {
    double distance = t2.as_julian_days() - t1.as_julian_days();
    return Swe_Time{t1.as_julian_days() + distance * proportion};
}

std::optional<std::pair<Swe_Time, Swe_Time>> Calc::get_arunodaya(Swe_Time const sunrise) const
{
    auto const prev_sunset = get_prev_sunset(sunrise);
    if (!prev_sunset.has_value()) { return{}; }
    constexpr double muhurtas_per_night = (12*60) / 48.0;
    constexpr double proportion_arunodaya = 2 / muhurtas_per_night; // 2/15 = 1/7.5
    constexpr double proportion_ardha_ghatika_before = 2.25 / muhurtas_per_night; // 2.25/15
    return std::pair(
        proportional_time(sunrise, *prev_sunset, proportion_arunodaya),
        proportional_time(sunrise, *prev_sunset, proportion_ardha_ghatika_before)
    );
}

#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
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

Swe_Time proportional_time(Swe_Time const t1, Swe_Time const t2, double const proportion) {
    double distance = t2.as_julian_days() - t1.as_julian_days();
    return Swe_Time{t1.as_julian_days() + distance * proportion};
}

Date Calc::get_vrata_date(const Swe_Time &sunrise) const
{
    // Adjust to make sure that Vrata data is correct in local timezone
    // (as opposed to *sunrise which is in UTC). We do this by adding an hour
    // for every 30 degrees of eastern latitude (24 hours for 360 degrees).
    // This could give wrong date if actual local timezone is quite
    // different from the "natural" timezone. But until we support proper
    // timezone, this should work for most cases.
    double adjustment_in_days = swe.coord.longitude * (1.0/360);
    Swe_Time local_sunrise{sunrise.as_julian_days()+adjustment_in_days};
    Date vrata_date{local_sunrise.as_date()};
    return vrata_date;
}

Paran Calc::get_paran(Swe_Time const &last_fasting_sunrise) const
{
    std::optional<Swe_Time> paran_start, paran_end;
    auto paran_sunrise = swe.get_sunrise(last_fasting_sunrise+0.1);
    if (paran_sunrise) {
        auto paran_sunset = swe.get_sunset(*paran_sunrise);
        paran_start = paran_sunrise;
        if (paran_sunset) {
            paran_end = proportional_time(*paran_sunrise, *paran_sunset, 0.2);
        }
    }

    Paran::Type type{Paran::Type::Standard};

    // paran end should never be before Dvadashi's end
    auto dvadashi_end = get_next_tithi_start(last_fasting_sunrise, Tithi{Tithi::Dvadashi_End});
    if (dvadashi_end > paran_start && dvadashi_end < paran_end) {
        paran_end = std::min(paran_end, dvadashi_end);
        type = Paran::Type::Until_Dvadashi_End;
    }

    Paran paran{type, paran_start, paran_end};
    return paran;
}

std::optional<Vrata> Calc::find_next_vrata(Date after) const
{
    auto sunrise = find_next_ekadashi_sunrise(Swe_Time{after});
    if (sunrise) {
        auto arunodaya_info = get_arunodaya(*sunrise);
        if (!arunodaya_info.has_value()) { return{}; }
        auto [arunodaya, ardha_ghatika_before_arunodaya] = *arunodaya_info;
        auto tithi_arunodaya = swe.get_tithi(arunodaya);
        Vrata_Type type = Vrata_Type::Ekadashi;
        if ((tithi_arunodaya.is_dashami())) {
            // purva-viddha Ekadashi, get next sunrise
            sunrise = swe.get_sunrise(Swe_Time{sunrise->as_julian_days()+0.1});
            if (!sunrise) { return {}; }
        } else {
            Tithi tithi_ardha_ghatika_before_arunodaya = swe.get_tithi(ardha_ghatika_before_arunodaya);
            if (tithi_ardha_ghatika_before_arunodaya.is_dashami()) {
                type = Vrata_Type::Sandigdha_Ekadashi;
                // Sandigdha (almost purva-viddha Ekadashi), get next sunrise
                sunrise = swe.get_sunrise(Swe_Time{sunrise->as_julian_days()+0.1});
                if (!sunrise) { return {}; }
            }
        }

        return Vrata{
                    type,
                    get_vrata_date(*sunrise),
                    get_paran(*sunrise)};
    }
    return {};
}

std::optional<Swe_Time> Calc::get_prev_sunset(Swe_Time const sunrise) const {
    Swe_Time back_24hrs{sunrise.as_julian_days()-1};
    return swe.get_sunset(back_24hrs);
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

std::optional<Swe_Time> Calc::get_next_tithi_start(Swe_Time from, Tithi tithi) const
{
    constexpr double average_tithi_length = 23.0/24 + 37.0/(24*60); // 23h37m
    Tithi cur_tithi = swe.get_tithi(from);
    double delta_tithi = cur_tithi.positive_delta_until_tithi(tithi);
    Swe_Time time{from + delta_tithi * average_tithi_length};
    cur_tithi = swe.get_tithi(time);

    double prev_abs_delta_tithi = std::numeric_limits<double>::max();

    while (cur_tithi != tithi) {
        delta_tithi = cur_tithi.delta_to_nearest_tithi(tithi);
        time += delta_tithi * average_tithi_length;
        cur_tithi = swe.get_tithi(time);

        double const abs_delta_tithi = fabs(delta_tithi);
        // Check for calculations loop: break if delta stopped decreasing (by absolute vbalue).
        if (abs_delta_tithi >= prev_abs_delta_tithi) {
            break;
        }
        prev_abs_delta_tithi = abs_delta_tithi;
    }
    return time;
}

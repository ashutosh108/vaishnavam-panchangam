#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <optional>

#include "calc.h"
#include "swe.h"

namespace vp {

Calc::Calc(Location coord):swe{coord} {}

std::optional<JulDays_UT> Calc::find_next_ekadashi_sunrise(JulDays_UT after) const
{
    int max_days_left = 16;
    std::optional<JulDays_UT> sunrise = after;
    while ((sunrise = swe.get_sunrise(*sunrise)).has_value() && max_days_left--) {
        auto tithi = swe.get_tithi(*sunrise);
        if (tithi.is_ekadashi() || tithi.is_dvadashi()) {
            return sunrise;
        }
        *sunrise += double_days{0.1};
    }
    return {};
}

JulDays_UT Calc::proportional_time(JulDays_UT const t1, JulDays_UT const t2, double const proportion) {
    double_days distance = t2 - t1;
    return t1 + distance * proportion;
}

date::year_month_day Calc::get_vrata_date(const JulDays_UT &sunrise) const
{
    // Adjust to make sure that Vrata data is correct in local timezone
    // (as opposed to *sunrise which is in UTC). We do this by adding an hour
    // for every 30 degrees of eastern latitude (24 hours for 360 degrees).
    // This could give wrong date if actual local timezone is quite
    // different from the "natural" timezone. But until we support proper
    // timezone, this should work for most cases.
    double_days adjustment_in_days{swe.coord.longitude * (1.0/360)};
    JulDays_UT local_sunrise = sunrise + adjustment_in_days;
    return local_sunrise.year_month_day();
}

Paran Calc::get_paran(JulDays_UT const &last_fasting_sunrise) const
{
    std::optional<JulDays_UT> paran_start, paran_end;
    auto paran_sunrise = swe.get_sunrise(last_fasting_sunrise+double_days{0.1});
    if (paran_sunrise) {
        auto paran_sunset = swe.get_sunset(*paran_sunrise);
        paran_start = paran_sunrise;
        if (paran_sunset) {
            paran_end = proportional_time(*paran_sunrise, *paran_sunset, 0.2);
        }
    }

    Paran::Type type{Paran::Type::Standard};

    auto dvadashi_start = get_next_tithi_start(last_fasting_sunrise-double_days{1.0}, Tithi{Tithi::Dvadashi});
    if (dvadashi_start) {
        auto dvadashi_end = get_next_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
        if (dvadashi_end) {
            // paran start should never be before the end of Dvadashi's first quarter
            if (paran_start) {
                auto dvadashi_quarter = proportional_time(*dvadashi_start, *dvadashi_end, 0.25);
                if (paran_start < dvadashi_quarter) {
                    paran_start = dvadashi_quarter;
                    type = Paran::Type::From_Quarter_Dvadashi;
                }
            }

            if (paran_start && paran_end) {
                // paran end should never be before Dvadashi's end
                if (dvadashi_end > paran_start && dvadashi_end < paran_end) {
                    paran_end = std::min(paran_end, dvadashi_end);
                    type = Paran::Type::Until_Dvadashi_End;
                }
            }
        }
    }


    Paran paran{type, paran_start, paran_end};
    return paran;
}

std::optional<Vrata> Calc::find_next_vrata(date::year_month_day after) const
{
    auto sunrise = find_next_ekadashi_sunrise(JulDays_UT{after});
    if (sunrise) {
        auto arunodaya_info = get_arunodaya(*sunrise);
        if (!arunodaya_info.has_value()) { return{}; }
        auto [arunodaya, ardha_ghatika_before_arunodaya] = *arunodaya_info;
        auto tithi_arunodaya = swe.get_tithi(arunodaya);
        Vrata_Type type = Vrata_Type::Ekadashi;
        if ((tithi_arunodaya.is_dashami())) {
            // purva-viddha Ekadashi, get next sunrise
            sunrise = swe.get_sunrise(*sunrise + double_days{0.1});
            if (!sunrise) { return {}; }
        } else {
            Tithi tithi_ardha_ghatika_before_arunodaya = swe.get_tithi(ardha_ghatika_before_arunodaya);
            if (tithi_ardha_ghatika_before_arunodaya.is_dashami()) {
                type = Vrata_Type::Sandigdha_Ekadashi;
                // Sandigdha (almost purva-viddha Ekadashi), get next sunrise
                sunrise = swe.get_sunrise(*sunrise + double_days{0.1});
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

std::optional<JulDays_UT> Calc::get_prev_sunset(JulDays_UT const sunrise) const {
    JulDays_UT back_24hrs{sunrise - double_days{1.0}};
    return swe.get_sunset(back_24hrs);
}

std::optional<std::pair<JulDays_UT, JulDays_UT>> Calc::get_arunodaya(JulDays_UT const sunrise) const
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

std::optional<JulDays_UT> Calc::get_next_tithi_start(JulDays_UT const from, Tithi const tithi) const
{
    using namespace std::literals::chrono_literals;
    constexpr double_hours average_tithi_length = 23h + 37min;
    Tithi cur_tithi = swe.get_tithi(from);

    double initial_delta_tithi = cur_tithi.positive_delta_until_tithi(tithi);
    // If delta_tithi >= 15 then actually there is
    // another target tithi before the presumably target one
    // (which we would miss with such a larget delta_tithi).
    // Since target tithi is always < 15.0, just increase it
    // by 15.0 because this function is supposed to find
    // next nearest Tithi, whether it's Shukla or Krishna.
    Tithi target_tithi = tithi;
    if (initial_delta_tithi >= 15.0) {
        target_tithi += 15.0;
        initial_delta_tithi -= 15.0;
    }

    JulDays_UT time{from + initial_delta_tithi * average_tithi_length};
    cur_tithi = swe.get_tithi(time);

    double prev_abs_delta_tithi = std::numeric_limits<double>::max();

    constexpr int max_iterations = 1'000;
    int iteration = 0;

    while (cur_tithi != target_tithi) {
        double const delta_tithi = cur_tithi.delta_to_nearest_tithi(target_tithi);
        time += delta_tithi * average_tithi_length;
        cur_tithi = swe.get_tithi(time);

        double const abs_delta_tithi = fabs(delta_tithi);
        // Check for calculations loop: break if delta stopped decreasing (by absolute vbalue).
        if (abs_delta_tithi >= prev_abs_delta_tithi) {
            break;
        }
        prev_abs_delta_tithi = abs_delta_tithi;
        if (++iteration >= max_iterations) {
            return {};
        }
    }
    return time;
}

} // namespace vp

#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <optional>

#include "calc.h"
#include "concat.h"
#include "swe.h"

namespace vp {

Calc::Calc(Swe swe_):swe(std::move(swe_)) {}

/* Find sunrise during next ekadashi tithi or right after it.
 */
std::optional<JulDays_UT> Calc::find_ekadashi_sunrise(JulDays_UT after) const
{
    std::optional<JulDays_UT> ekadashi = find_tithi_start(after, Tithi{Tithi::Ekadashi});
    if (!ekadashi.has_value()) return std::nullopt;

    return swe.find_sunrise(*ekadashi);
}

JulDays_UT Calc::proportional_time(JulDays_UT const t1, JulDays_UT const t2, double const proportion) {
    double_days distance = t2 - t1;
    return t1 + distance * proportion;
}

date::year_month_day Calc::get_vrata_date(const JulDays_UT sunrise) const
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

Paran Calc::get_paran(const JulDays_UT last_fasting_sunrise) const
{
    std::optional<JulDays_UT> paran_start, paran_end;
    auto paran_sunrise = next_sunrise(last_fasting_sunrise);
    if (paran_sunrise) {
        auto paran_sunset = swe.find_sunset(*paran_sunrise);
        paran_start = paran_sunrise;
        if (paran_sunset) {
            paran_end = proportional_time(*paran_sunrise, *paran_sunset, 0.2);
        }
    }

    Paran::Type type{Paran::Type::Standard};

    auto dvadashi_start = find_tithi_start(last_fasting_sunrise-double_days{1.0}, Tithi{Tithi::Dvadashi});
    if (dvadashi_start) {
        auto dvadashi_end = find_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
        if (dvadashi_end) {
            // paran start should never be before the end of Dvadashi's first quarter
            if (paran_start) {
                auto dvadashi_quarter = proportional_time(*dvadashi_start, *dvadashi_end, 0.25);
                if (paran_start < dvadashi_quarter) {
                    paran_start = dvadashi_quarter;
                    if (paran_end <= paran_start) {
                        paran_end = std::nullopt;
                    }
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

Paran Calc::atirikta_paran(const JulDays_UT first_fasting_sunrise) const
{
    auto second_fasting_sunrise = next_sunrise_v(first_fasting_sunrise);
    auto paran_sunrise = next_sunrise_v(second_fasting_sunrise);
    auto paran_sunset = swe.find_sunset_v(paran_sunrise);
    auto fifth_of_paran_daytime = proportional_time(paran_sunrise, paran_sunset, 0.2);
    auto dvadashi_end = find_tithi_start(paran_sunrise, Tithi{Tithi::Dvadashi_End});
    if (fifth_of_paran_daytime < dvadashi_end) {
        return {Paran::Type::Standard, paran_sunrise, fifth_of_paran_daytime};
    }
    return {Paran::Type::Puccha_Dvadashi, paran_sunrise, dvadashi_end};
}

std::optional<JulDays_UT> Calc::next_sunrise(JulDays_UT sunrise) const {
    return swe.find_sunrise(sunrise + double_days{0.001});
}

JulDays_UT Calc::next_sunrise_v(JulDays_UT sunrise) const {
    auto sunrise_or_nullopt = next_sunrise(sunrise);
    if (!sunrise_or_nullopt) {
        std::stringstream s;
        s << "can't get next sunrise after " << sunrise;
        throw std::runtime_error(s.str());
    }

    return *sunrise_or_nullopt;
}

/* Find out if we have "atiriktA ekAdashI" situation (shuddha ekAdashI encompasses two sunrises).
 * This function assumes that if first sunrise is ekAdashI, then it's shuddhA-ekAdashI.
 * Otherwise the sunrise under consideration would have been "next sunrise" already.
 */
bool Calc::got_atirikta_ekadashi(const JulDays_UT sunrise_on_shuddha_ekadashi_or_next_one) const
{
    auto second_sunrise = next_sunrise(sunrise_on_shuddha_ekadashi_or_next_one);
    if (!second_sunrise.has_value()) return false;

    Tithi tithi_on_first_sunrise = swe.get_tithi(sunrise_on_shuddha_ekadashi_or_next_one);
    Tithi tithi_on_second_sunrise = swe.get_tithi(*second_sunrise);
    return tithi_on_first_sunrise.is_ekadashi() && tithi_on_second_sunrise.is_ekadashi();
}

/* Find out if we have "atiriktA dvAdashI" situation (dvadashI encompasses two sunrises).
 * If yes, then adjust the sunrise to be next day's sunrise
 * (because it has to be the sunrise of last fastiung day).
 */
bool Calc::got_atirikta_dvadashi(const JulDays_UT sunrise_on_shuddha_ekadashi_or_next_one) const
{
    auto second_sunrise = next_sunrise(sunrise_on_shuddha_ekadashi_or_next_one);
    if (!second_sunrise.has_value()) return false;
    auto third_sunrise = next_sunrise(*second_sunrise);
    if (!second_sunrise.has_value()) return false;

    Tithi tithi_on_second_sunrise = swe.get_tithi(*second_sunrise);
    Tithi tithi_on_third_sunrise = swe.get_tithi(*third_sunrise);
    return tithi_on_second_sunrise.is_dvadashi() && tithi_on_third_sunrise.is_dvadashi();
}

/* Main calculation: return next vrata on a given date or after.
 * Determine type of vrata (Ekadashi, either of two Atiriktas, Sandigdha),
 * paran time.
 *
 * TODO: Sometimes we miss ekadashi when it falls on the starting day.
 *       Should probably adjust "after" for local midnight before going on.
 *       Need to find an example and add test.*/
std::optional<Vrata> Calc::find_next_vrata(date::year_month_day after) const
{
    auto midnight = calc_astronomical_midnight(after);
    auto start_time = midnight - double_days{3.0};
    int run_number = 0;
repeat_with_fixed_start_time:
    if (++run_number > 2) {
        std::stringstream s;
        s << swe.coord.name << " after " << after << " (" << start_time << "): potential eternal loop detected";
        throw std::runtime_error(s.str());
    }
    auto sunrise = find_ekadashi_sunrise(start_time);
    if (!sunrise) return std::nullopt;

    // jump here can opnly happen once per call, when we got vrata from yesterday.
    auto arunodaya_info = arunodaya_for_sunrise(*sunrise);
    if (!arunodaya_info.has_value()) { return{}; }

    auto [arunodaya, ardha_ghatika_before_arunodaya] = *arunodaya_info;
    auto tithi_arunodaya = swe.get_tithi(arunodaya);
    Vrata_Type type = Vrata_Type::Ekadashi;
    if ((tithi_arunodaya.is_dashami())) {
        // purva-viddha Ekadashi, get next sunrise
        sunrise = next_sunrise(*sunrise);
        if (!sunrise) { return {}; }
    } else {
        Tithi tithi_ardha_ghatika_before_arunodaya = swe.get_tithi(ardha_ghatika_before_arunodaya);
        if (tithi_ardha_ghatika_before_arunodaya.is_dashami()) {
            type = Vrata_Type::Sandigdha_Ekadashi;
            // Sandigdha (almost purva-viddha Ekadashi), get next sunrise
            sunrise = next_sunrise(*sunrise);
            if (!sunrise) { return {}; }
        }
    }

    auto vrata_date = get_vrata_date(*sunrise);

    // if we found vrata before the requested date, then those -3days in the beginning were too much of an adjustment.
    // so we restart without that 3 days offset.
    if (vrata_date < after) {
        start_time = midnight;
        goto repeat_with_fixed_start_time;
    }

    if (got_atirikta_ekadashi(*sunrise)) {
        if (type == Vrata_Type::Sandigdha_Ekadashi) {
            type = Vrata_Type::Sandigdha_Atirikta_Ekadashi;
        } else {
            type = Vrata_Type::Atirikta_Ekadashi;
        }

        return Vrata{
                    type,
                    vrata_date,
                    atirikta_paran(*sunrise)};
    }

    if (got_atirikta_dvadashi(*sunrise)) {
        if (type == Vrata_Type::Sandigdha_Ekadashi) {
            type = Vrata_Type::Sandigdha_With_Atirikta_Dvadashi;
        } else {
            type = Vrata_Type::With_Atirikta_Dvadashi;
        }

        return Vrata{
                    type,
                    vrata_date,
                    atirikta_paran(*sunrise)};
    }

    return Vrata{
                type,
                vrata_date,
                get_paran(*sunrise)};
}

std::optional<JulDays_UT> Calc::sunset_before_sunrise(JulDays_UT const sunrise) const {
    JulDays_UT back_24hrs{sunrise - double_days{1.0}};
    return swe.find_sunset(back_24hrs);
}

std::optional<std::pair<JulDays_UT, JulDays_UT>> Calc::arunodaya_for_sunrise(JulDays_UT const sunrise) const
{
    auto const prev_sunset = sunset_before_sunrise(sunrise);
    if (!prev_sunset.has_value()) { return{}; }
    constexpr double muhurtas_per_night = (12*60) / 48.0;
    constexpr double proportion_arunodaya = 2 / muhurtas_per_night; // 2/15 = 1/7.5
    constexpr double proportion_ardha_ghatika_before = 2.25 / muhurtas_per_night; // 2.25/15
    return std::pair(
        proportional_time(sunrise, *prev_sunset, proportion_arunodaya),
        proportional_time(sunrise, *prev_sunset, proportion_ardha_ghatika_before)
    );
}

std::optional<JulDays_UT> Calc::find_tithi_start(JulDays_UT const from, Tithi const tithi) const
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

JulDays_UT Calc::find_tithi_start_v(const JulDays_UT from, const Tithi tithi) const
{
    auto tithi_start_or_nullopt = find_tithi_start(from, tithi);
    if (!tithi_start_or_nullopt) {
        throw std::runtime_error(concat("can't find ", tithi, " tithi after ", from));
    }
    return *tithi_start_or_nullopt;
}

JulDays_UT Calc::calc_astronomical_midnight(date::year_month_day date) const {
    double_days adjustment{swe.coord.longitude * (1.0/360.0)};
    return JulDays_UT{JulDays_UT{date} - adjustment};
}

} // namespace vp

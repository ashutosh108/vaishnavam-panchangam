#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <optional>
#include <tl/expected.hpp>

#include "calc.h"
#include "concat.h"
#include "swe.h"

// VP_TRY_AUTO: declare auto var, assign given value to it.
// Assuming it's tl::expected<something, Error>,
// check if we have got an error and return that error,
// if necessary. Otherwise, continue.
#define VP_TRY_AUTO(var_name, assign_to)                \
    const auto var_name = assign_to;                    \
    if (!var_name.has_value()) {                        \
        return tl::unexpected{var_name.error()};        \
    }

#define VP_TRY(existing_var_name, assign_to) {          \
    auto tmp = assign_to;                               \
    if (!tmp.has_value()) {                             \
        return tl::unexpected{tmp.error()};             \
    }                                                   \
    existing_var_name = *tmp;                           \
}

namespace vp {

Calc::Calc(Swe swe_):swe(std::move(swe_)) {}

/* Find sunrise during next ekadashi tithi or right after it.
 */
tl::expected<JulDays_UT, CalcError> Calc::find_ekadashi_sunrise(JulDays_UT after) const
{
    const auto ekadashi = find_tithi_start(after, Tithi{Tithi::Ekadashi});

    return swe.find_sunrise(ekadashi);
}

JulDays_UT Calc::proportional_time(JulDays_UT const t1, JulDays_UT const t2, double const proportion) {
    double_days distance = t2 - t1;
    return t1 + distance * proportion;
}

/* get_vrata_date():
 * Returns the formal date for the vrata i.e. date for the vrata sunrise
 * in local timezone.
 */
date::year_month_day Calc::get_vrata_date(const JulDays_UT sunrise) const
{
    auto timezone = date::locate_zone(swe.location.timezone_name);
    auto zoned = sunrise.as_zoned_time(timezone);
    return date::year_month_day{date::floor<date::days>(zoned.get_local_time())};
}

Paran Calc::get_paran(const Vrata & vrata) const
{
    std::optional<JulDays_UT> paran_start, paran_end;
    paran_start = vrata.sunrise2;
    paran_end = proportional_time(vrata.sunrise2, vrata.sunset2, 0.2);

    Paran::Type type{Paran::Type::Standard};

    auto dvadashi_start = find_tithi_start(vrata.sunrise1-double_days{1.0}, Tithi{Tithi::Dvadashi});
    auto dvadashi_end = find_tithi_start(dvadashi_start, Tithi{Tithi::Dvadashi_End});
    // paran start should never be before the end of Dvadashi's first quarter
    auto dvadashi_quarter = proportional_time(dvadashi_start, dvadashi_end, 0.25);
    if (paran_start < dvadashi_quarter) {
        paran_start = dvadashi_quarter;
        if (paran_end <= paran_start) {
            paran_end = std::nullopt;
        }
        type = Paran::Type::From_Quarter_Dvadashi;
    }

    if (paran_end) {
        // paran end should never be before Dvadashi's end
        if (dvadashi_end > *paran_start && dvadashi_end < *paran_end) {
            paran_end = std::min(*paran_end, dvadashi_end);
            type = Paran::Type::Until_Dvadashi_End;
        }
    }

    Paran paran{type, paran_start, paran_end};
    return paran;
}

Paran Calc::atirikta_paran(const Vrata & vrata) const
{
    auto fifth_of_paran_daytime = proportional_time(vrata.sunrise3, vrata.sunset3, 0.2);
    auto dvadashi_end = find_tithi_start(vrata.sunrise3, Tithi{Tithi::Dvadashi_End});
    if (fifth_of_paran_daytime < dvadashi_end) {
        return Paran{Paran::Type::Standard, vrata.sunrise3, fifth_of_paran_daytime};
    }
    return Paran{Paran::Type::Puccha_Dvadashi, vrata.sunrise3, dvadashi_end};
}

tl::expected<JulDays_UT, CalcError> Calc::next_sunrise(JulDays_UT sunrise) const {
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
bool Calc::got_atirikta_ekadashi(const Vrata & vrata) const
{
    Tithi tithi_on_first_sunrise = swe.get_tithi(vrata.sunrise1);
    Tithi tithi_on_second_sunrise = swe.get_tithi(vrata.sunrise2);
    return tithi_on_first_sunrise.is_ekadashi() && tithi_on_second_sunrise.is_ekadashi();
}

/* Find out if we have "atiriktA dvAdashI" situation (dvadashI encompasses two sunrises).
 * If yes, then adjust the sunrise to be next day's sunrise
 * (because it has to be the sunrise of last fastiung day).
 */
bool Calc::got_atirikta_dvadashi(const Vrata & vrata) const
{
    Tithi tithi_on_second_sunrise = swe.get_tithi(vrata.sunrise2);
    Tithi tithi_on_third_sunrise = swe.get_tithi(vrata.sunrise3);
    return tithi_on_second_sunrise.is_dvadashi() && tithi_on_third_sunrise.is_dvadashi();
}

Vrata_Type Calc::calc_vrata_type(const Vrata &vrata) const
{
    if (got_atirikta_ekadashi(vrata)) {
        return Vrata_Type::Atirikta_Ekadashi;
    }

    if (got_atirikta_dvadashi(vrata)) {
        return Vrata_Type::With_Atirikta_Dvadashi;
    }

    return Vrata_Type::Ekadashi;
}

/* Main calculation: return next vrata on a given date or after.
 * Determine type of vrata (Ekadashi, or either of two Atiriktas),
 * paran time.
 *
 * Can also return a CalcError when we can't get necessary sunrise/sunset.
 * It happens e.g. mid-summer and mid-winter in ~68+ degrees latitudes,
 * like Murmank on 2020-06-05 (no sunset) or 2017-11-27 (no sunrise).
 */
tl::expected<Vrata, CalcError> Calc::find_next_vrata(date::year_month_day after) const
{
    auto midnight = calc_astronomical_midnight(after);
    auto start_time = midnight - double_days{3.0};
    int run_number = 0;
    Vrata vrata{};
repeat_with_fixed_start_time:
    if (++run_number > 2) {
        std::stringstream s;
        s << swe.location.name << " after " << after << " (" << start_time << "): potential eternal loop detected";
        throw std::runtime_error(s.str());
    }
    VP_TRY(vrata.sunrise1, find_ekadashi_sunrise(start_time));
    vrata.ekadashi_sunrise = vrata.sunrise1;
    VP_TRY(vrata.sunrise2, next_sunrise(vrata.sunrise1));
    VP_TRY(vrata.sunset0, sunset_before_sunrise(vrata.sunrise1));

    vrata.ativrddhatvam = calc_ativrddhatvam_for_sunset_and_sunrise(vrata.sunset0, vrata.sunrise1);
    auto tithi_that_must_not_be_dashamI = swe.get_tithi(vrata.ativrddhatvam.relevant_timepoint());
    if (tithi_that_must_not_be_dashamI.is_dashami()) {
        vrata.sunrise1 = vrata.sunrise2;
        VP_TRY(vrata.sunrise2, next_sunrise(vrata.sunrise1));
    }

    auto vrata_date = get_vrata_date(vrata.sunrise1);

    // if we found vrata before the requested date, then those -3days in the beginning were too much of an adjustment.
    // so we restart without that 3 days offset.
    if (vrata_date < after) {
        start_time = midnight;
        goto repeat_with_fixed_start_time;
    }

    vrata.date = vrata_date;
    vrata.location = swe.location;

    VP_TRY(vrata.sunset2, swe.find_sunset(vrata.sunrise2));
    VP_TRY(vrata.sunrise3, next_sunrise(vrata.sunrise2));
    VP_TRY(vrata.sunset3, swe.find_sunset(vrata.sunrise3));

    vrata.type = calc_vrata_type(vrata);
    vrata.paran = is_atirikta(vrata.type) ? atirikta_paran(vrata) : get_paran(vrata);

    return vrata;
}

tl::expected<JulDays_UT, CalcError> Calc::sunset_before_sunrise(JulDays_UT const sunrise) const {
    JulDays_UT back_24hrs{sunrise - double_days{1.0}};
    return swe.find_sunset(back_24hrs);
}

tl::expected<JulDays_UT, CalcError> Calc::arunodaya_for_sunrise(JulDays_UT const sunrise) const
{
    VP_TRY_AUTO(prev_sunset, sunset_before_sunrise(sunrise));
    constexpr double muhurtas_per_night = (12*60) / 48.0;
    constexpr double proportion_arunodaya = 2 / muhurtas_per_night; // 2/15 = 1/7.5
    return proportional_time(sunrise, *prev_sunset, proportion_arunodaya);
}

JulDays_UT Calc::find_tithi_start(JulDays_UT const from, Tithi const tithi) const
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
            throw CantFindTithiAfter{tithi, from};
        }
    }
    return time;
}

JulDays_UT Calc::calc_astronomical_midnight(date::year_month_day date) const {
    const double_days adjustment{swe.location.longitude.longitude * (1.0/360.0)};
    return JulDays_UT{date} - adjustment;
}

Ativrddhatvam Calc::calc_ativrddhatvam_for_sunset_and_sunrise(JulDays_UT sunset0, JulDays_UT sunrise1) const
{
    auto ekadashi_start = find_tithi_start(sunrise1 - double_hours{25.0}, Tithi{Tithi::Ekadashi});
    auto dashami_start = find_tithi_start(ekadashi_start - double_hours{27.0}, Tithi{Tithi::Dashami});
    auto dvadashi_start = find_tithi_start(ekadashi_start + double_hours{1.0}, Tithi{Tithi::Dvadashi});
    auto trayodashi_start = find_tithi_start(dvadashi_start + double_hours{1.0}, Tithi{Tithi::Trayodashi});

    auto night_length = sunrise1 - sunset0;
    double_days ghatika = night_length / 30.0;
    double_days vighatika = ghatika / 60.0;
    // Sunrise is 60 ghatikas after last sunrise. So 54gh 40vigh is 60:00-54:40 = 5:20 (5gh20vigh before sunrise).
    // Same for other three time points.
    auto time_point_ativrddha_54gh_40vigh = sunrise1 - 5 * ghatika - 20 * vighatika;
    auto time_point_vrddha_55gh = sunrise1 - 5 * ghatika;
    auto time_point_samyam_55gh_50vigh = sunrise1 - 4 * ghatika - 10 * vighatika;
    auto time_point_hrasva_55gh_55vigh = sunrise1 - 4 * ghatika - 5 * vighatika;
    const auto time_point_arunodaya = sunrise1 - 4 * ghatika;
    return Ativrddhatvam{
        sunset0, sunrise1,
        time_point_ativrddha_54gh_40vigh, time_point_vrddha_55gh, time_point_samyam_55gh_50vigh, time_point_hrasva_55gh_55vigh,
        time_point_arunodaya,
        dashami_start, ekadashi_start, dvadashi_start, trayodashi_start
    };
}

} // namespace vp

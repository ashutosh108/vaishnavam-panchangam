#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <optional>
#include <tl/expected.hpp>

#include "calc.h"
#include "calc-flags.h"
#include "swe.h"

// VP_TRY_AUTO: declare auto var, assign given value to it.
// Assuming it's tl::expected<something, Error>,
// check if we have got an error and return that error,
// if necessary. Otherwise, continue.
#define VP_TRY_AUTO(var_name, assign_to)                \
    const auto var_name = assign_to;                    \
    if (!(var_name).has_value()) {                      \
        return tl::make_unexpected((var_name).error()); \
    }

#define VP_TRY(existing_var_name, assign_to) {          \
    auto tmp = assign_to;                               \
    if (!tmp.has_value()) {                             \
        return tl::make_unexpected(tmp.error());        \
    }                                                   \
    (existing_var_name) = *tmp;                         \
}

namespace vp {

static constexpr auto small_enough_delta = double_days{0.001};


Calc::Calc(Swe swe_):swe(std::move(swe_)) {}

/* Main calculation: return next vrata on a given date or after.
 * Determine type of vrata (Ekadashi, or either of two Atiriktas),
 * paran time.
 *
 * Can also return a CalcError when we can't get necessary sunrise/sunset.
 * It happens e.g. mid-summer and mid-winter in ~68+ degrees latitudes,
 * like Murmank on 2020-06-05 (no sunset) or 2017-11-27 (no sunrise).
 */
tl::expected<Vrata, CalcError> Calc::find_next_vrata(date::local_days after) const
{
    auto midnight = calc_astronomical_midnight(after);
    auto start_time = midnight - double_days{3.0};
    int run_number = 0;
    Vrata vrata{};
repeat_with_fixed_start_time:
    if (++run_number > 2) {
        throw std::runtime_error(fmt::format("find_next_vrata @{} after {} ({}): potential eternal loop detected", swe.location.name, after, start_time));
    }
    VP_TRY(vrata.sunrise1, find_ekadashi_sunrise(start_time))
    VP_TRY(vrata.sunset0, sunset_before_sunrise(vrata.sunrise1))
    VP_TRY(vrata.sunrise2, next_sunrise(vrata.sunrise1))

    vrata.times = calc_key_times_from_sunset_and_sunrise(vrata.sunset0, vrata.sunrise1);
    auto tithi_that_must_not_be_dashamI = swe.tithi(vrata.times.ativrddhaditvam_timepoint());
    if (tithi_that_must_not_be_dashamI.is_dashami()) {
        vrata.sunrise0 = vrata.sunrise1;
        vrata.sunrise1 = vrata.sunrise2;
        VP_TRY(vrata.sunset0, sunset_before_sunrise(vrata.sunrise1))
        VP_TRY(vrata.sunrise2, next_sunrise(vrata.sunrise1))
    }

    vrata.date = get_vrata_date(vrata.sunrise1);

    // if we found vrata before the requested date, then those -3days in the beginning were too much of an adjustment.
    // so we restart without that 3 days offset.
    if (vrata.date < after) {
        start_time = midnight;
        goto repeat_with_fixed_start_time;
    }

    vrata.location = swe.location;

    VP_TRY(vrata.sunset2, swe.next_sunset(vrata.sunrise2))
    VP_TRY(vrata.sunrise3, next_sunrise(vrata.sunrise2))
    VP_TRY(vrata.sunset3, swe.next_sunset(vrata.sunrise3))

    vrata.type = calc_vrata_type(vrata);
    vrata.paran = is_atirikta(vrata.type) ?
        atirikta_paran(vrata.sunrise3, vrata.sunset3, vrata.times.trayodashi_start)
    :
        get_paran(vrata.sunrise2, vrata.sunset2, vrata.times.dvadashi_start, vrata.times.trayodashi_start);
    vrata.masa = chandra_masa_amanta(vrata.sunrise1);
    vrata.paksha = tithi_that_must_not_be_dashamI.get_paksha();

    return vrata;
}

JulDays_UT Calc::calc_astronomical_midnight(date::local_days date) const {
    const double_days adjustment{swe.location.longitude.longitude * (1.0/360.0)};
    return JulDays_UT{date} - adjustment;
}

tl::expected<JulDays_UT, CalcError> Calc::first_midnight_after(JulDays_UT after)
{
    const auto next_sunset = swe.next_sunset(after);
    if (!next_sunset) { return tl::make_unexpected(next_sunset.error()); }
    const auto next_sunset_sunrise = swe.next_sunrise(*next_sunset);
    if (!next_sunset_sunrise) { return tl::make_unexpected(next_sunset_sunrise.error()); }

    const auto prev_sunset = swe.next_sunset(*next_sunset - Swe::max_interval_between_sunrises);
    if (!prev_sunset) { return tl::make_unexpected(prev_sunset.error()); }
    const auto prev_sunset_sunrise = swe.next_sunrise(*prev_sunset);
    if (!prev_sunset_sunrise) { return tl::make_unexpected(prev_sunset_sunrise.error()); }

    const auto midnight1 = proportional_time(*prev_sunset, *prev_sunset_sunrise, 0.5);
    if (midnight1 >= after) { return midnight1; }

    const auto midnight2 = proportional_time(*next_sunset, *next_sunset_sunrise, 0.5);
    assert(midnight2 >= after);
    return midnight2;
}

/* Find sunrise during next ekadashi tithi or right after it.
 */
tl::expected<JulDays_UT, CalcError> Calc::find_ekadashi_sunrise(JulDays_UT after) const
{
    const auto ekadashi = find_either_tithi_start(after, Tithi::Ekadashi());

    return swe.next_sunrise(ekadashi);
}

tl::expected<JulDays_UT, CalcError> Calc::next_sunrise(JulDays_UT sunrise) const {
    return swe.next_sunrise(sunrise + small_enough_delta);
}

JulDays_UT Calc::next_sunrise_v(JulDays_UT sunrise) const {
    auto sunrise_or_nullopt = next_sunrise(sunrise);
    if (!sunrise_or_nullopt) {
        throw std::runtime_error(fmt::format("can't get next sunrise after {}", sunrise));
    }

    return *sunrise_or_nullopt;
}

tl::expected<JulDays_UT, CalcError> Calc::sunset_before_sunrise(JulDays_UT const sunrise) const {
    JulDays_UT back_24hrs{sunrise - double_days{1.0}};
    return swe.next_sunset(back_24hrs);
}

Vrata_Time_Points Calc::calc_key_times_from_sunset_and_sunrise(JulDays_UT sunset0, JulDays_UT sunrise1) const
{
    const auto ekadashi_start = find_either_tithi_start(sunrise1 - Tithi::MaxLength(), Tithi::Ekadashi());
    const auto dashami_start = find_either_tithi_start(ekadashi_start - Tithi::MaxLength(), Tithi::Dashami());
    const auto dvadashi_start = find_either_tithi_start(ekadashi_start + double_hours{1.0}, Tithi::Dvadashi());
    const auto trayodashi_start = find_either_tithi_start(dvadashi_start + double_hours{1.0}, Tithi::Trayodashi());

    const double_days night_length = sunrise1 - sunset0;
    const double_days ghatika = night_length / 30.0;
    const double_days vighatika = ghatika / 60.0;
    // Sunrise is 60 ghatikas after last sunrise. So 54gh 40vigh is 60:00-54:40 = 5:20 (5gh20vigh before sunrise).
    // Same for other three time points.
    const auto time_point_ativrddha_54gh_40vigh = sunrise1 - 5 * ghatika - 20 * vighatika;
    const auto time_point_vrddha_55gh = sunrise1 - 5 * ghatika;
    const auto time_point_samyam_55gh_50vigh = sunrise1 - 4 * ghatika - 10 * vighatika;
    const auto time_point_hrasva_55gh_55vigh = sunrise1 - 4 * ghatika - 5 * vighatika;
    const auto time_point_arunodaya = sunrise1 - 4 * ghatika;
    return Vrata_Time_Points{
        time_point_ativrddha_54gh_40vigh, time_point_vrddha_55gh, time_point_samyam_55gh_50vigh, time_point_hrasva_55gh_55vigh,
        time_point_arunodaya,
        dashami_start, ekadashi_start, dvadashi_start, trayodashi_start
    };
}

/* get_vrata_date():
 * Returns the formal date for the vrata i.e. date for the vrata sunrise
 * in local timezone.
 */
date::local_days Calc::get_vrata_date(const JulDays_UT sunrise) const
{
    auto zoned = sunrise.as_zoned_time(swe.location.time_zone());
    return date::floor<date::days>(zoned.get_local_time());
}

Vrata_Type Calc::calc_vrata_type(const Vrata &vrata) const
{
    if (got_shravana_nakshatra_next_day(vrata)) {
        return Vrata_Type::With_Shravana_Dvadashi_Next_Day;
    }

    if (got_shravana_nakshatra_same_day(vrata)) {
        return Vrata_Type::With_Shravana_Dvadashi_Same_Day;
    }

    if (got_atirikta_ekadashi(vrata)) {
        return Vrata_Type::With_Atirikta_Ekadashi;
    }

    if (got_atirikta_dvadashi(vrata)) {
        return Vrata_Type::With_Atirikta_Dvadashi;
    }

    return Vrata_Type::Ekadashi;
}

namespace {
bool got_shravana_for_sunrise_sunset(JulDays_UT sunrise, JulDays_UT sunset, JulDays_UT next_sunrise, const vp::Swe & swe) {
    using ghatikas = std::chrono::duration<double, std::ratio_multiply<std::chrono::minutes::period, std::ratio<24>>>;
    using namespace std::chrono_literals;

    DiscreteNakshatra nakshatra_on_sunrise = swe.nakshatra(sunrise);
    if (nakshatra_on_sunrise != DiscreteNakshatra::Shravana()) { return false; }
    Tithi tithi_on_sunrise = swe.tithi(sunrise);
    if (!tithi_on_sunrise.is_dvadashi()) { return false; }

    // limit until which both Dvādaśī and Śravaṇa must hold. 12 or 14 ghaṭikas
    // from sunrise, depending on which Kṛṣṇāmṛta-mahārṇava commentaries we rely upon.
    constexpr auto ratio_for_12gh = ghatikas{12} / 12h;
    constexpr auto ratio_for_14gh = ghatikas{14} / 12h;
    static_assert (ratio_for_12gh > 0.0, "12 or 14 ghatikas do not get rounded down to zero");
    static_assert (ratio_for_12gh == 2.0/5, "12ghatikas must be 2/5th of daytime (30 ghatikas)");
    static_assert (ratio_for_14gh == 7.0/15, "14 ghatikas must be 7/15th of daytime (30 ghatikas)");
    const bool require_14gh = ((swe.calc_flags & CalcFlags::ShravanaDvadashiMask) == CalcFlags::ShravanaDvadashi14ghPlus);
    const auto madhyahna_limit_ratio_from_daytime = require_14gh ? ratio_for_14gh : ratio_for_12gh;
    const auto madhyahna_limit = proportional_time(sunrise, sunset, madhyahna_limit_ratio_from_daytime);
    DiscreteNakshatra nakshantra_at_limit = swe.nakshatra(madhyahna_limit);
    if (nakshantra_at_limit != DiscreteNakshatra::Shravana()) { return false; }
    Tithi tithi_at_limit = swe.tithi(madhyahna_limit);
    if (!tithi_at_limit.is_dvadashi()) { return false; }

    // If Śravaṇa nakṣatra extends till another sunrise, then Śravaṇa dvādaśī condition is not fulfilled.
    // Since nakshatras cannot reach 72 ghatikas (60 for full ekādaśī day + 12 to reach madhyahnam on dvādaśī),
    // we don't have to check the previous sunrise (aka "sunrise1") for Śravaṇa.
    // So we only need to check for next sunrise.
    const DiscreteNakshatra nakshatra_at_next_sunrise = swe.nakshatra(next_sunrise);
    if (nakshatra_at_next_sunrise == DiscreteNakshatra::Shravana()) {
        return false;
    }
    return true;
}
}

bool Calc::got_shravana_nakshatra_next_day(const Vrata &vrata) const
{
    return got_shravana_for_sunrise_sunset(vrata.sunrise2, vrata.sunset2, vrata.sunrise3, swe);
}

bool Calc::got_shravana_nakshatra_same_day(const Vrata &vrata) const
{
    auto sunset1 = swe.next_sunset(vrata.sunrise1);
    if (!sunset1) return false;
    return got_shravana_for_sunrise_sunset(vrata.sunrise1, *sunset1, vrata.sunrise2, swe);
}

/* Find out if we have "atiriktA ekAdashI" situation (shuddha ekAdashI encompasses two sunrises).
 * This function assumes that if first sunrise is ekAdashI, then it's shuddhA-ekAdashI.
 * Otherwise the sunrise under consideration would have been "next sunrise" already.
 */
bool Calc::got_atirikta_ekadashi(const Vrata & vrata) const
{
    Tithi tithi_on_first_sunrise = swe.tithi(vrata.sunrise1);
    Tithi tithi_on_second_sunrise = swe.tithi(vrata.sunrise2);
    return tithi_on_first_sunrise.is_ekadashi() && tithi_on_second_sunrise.is_ekadashi();
}

/* Find out if we have "atiriktA dvAdashI" situation (dvadashI encompasses two sunrises).
 * If yes, then adjust the sunrise to be next day's sunrise
 * (because it has to be the sunrise of last fastiung day).
 */
bool Calc::got_atirikta_dvadashi(const Vrata & vrata) const
{
    Tithi tithi_on_second_sunrise = swe.tithi(vrata.sunrise2);
    Tithi tithi_on_third_sunrise = swe.tithi(vrata.sunrise3);
    return tithi_on_second_sunrise.is_dvadashi() && tithi_on_third_sunrise.is_dvadashi();
}

Paran Calc::get_paran(const JulDays_UT sunrise2, const JulDays_UT sunset2, const JulDays_UT dvadashi_start, const JulDays_UT dvadashi_end) const
{
    std::optional<JulDays_UT> paran_start, paran_end;
    paran_start = sunrise2;
    paran_end = proportional_time(sunrise2, sunset2, 0.2);

    Paran::Type type{Paran::Type::Standard};

    // paran start should never be before the end of Dvadashi's first quarter
    auto dvadashi_quarter = proportional_time(dvadashi_start, dvadashi_end, 0.25);
    if (paran_start < dvadashi_quarter) {
        paran_start = dvadashi_quarter;
        paran_end = std::nullopt;
        type = Paran::Type::From_Quarter_Dvadashi;
    }

    if (paran_end) {
        // paran end should never be before Dvadashi's end
        if (dvadashi_end > *paran_start && dvadashi_end < *paran_end) {
            paran_end = dvadashi_end;
            type = Paran::Type::Puccha_Dvadashi;
        }
    }

    Paran paran{type, paran_start, paran_end, swe.location.time_zone()};
    return paran;
}

Paran Calc::atirikta_paran(const JulDays_UT sunrise3, const JulDays_UT sunset3, const JulDays_UT dvadashi_end) const
{
    auto fifth_of_paran_daytime = proportional_time(sunrise3, sunset3, 0.2);
    if (dvadashi_end < sunrise3) {
        return Paran{Paran::Type::Standard, sunrise3, fifth_of_paran_daytime, swe.location.time_zone()};
    } else if (dvadashi_end < fifth_of_paran_daytime) {
        return Paran{Paran::Type::Puccha_Dvadashi, sunrise3, dvadashi_end, swe.location.time_zone()};
    }
    // Standard Pāraṇam with viśeṣa: we specify paran_limit, absolute latest time for pāraṇam, which is after usual standard pāraṇam end.
    return Paran{Paran::Type::Standard, sunrise3, fifth_of_paran_daytime, dvadashi_end, swe.location.time_zone()};
}

tl::expected<JulDays_UT, CalcError> Calc::arunodaya_for_sunrise(JulDays_UT const sunrise) const
{
    VP_TRY_AUTO(prev_sunset, sunset_before_sunrise(sunrise))
    constexpr double muhurtas_per_night = (12*60) / 48.0;
    constexpr double proportion_arunodaya = 2 / muhurtas_per_night; // 2/15 = 1/7.5
    return proportional_time(sunrise, *prev_sunset, proportion_arunodaya);
}

namespace {
template<class Value, class ValueGetter, class PosDeltaCalculator, class MinDeltaCalculator, class ExceptionThrower, class InitialTargetFixer>
JulDays_UT find_time_with_given_value(
    const JulDays_UT from,
    Value target_value,
    double_hours average_length,
    ValueGetter getter,
    PosDeltaCalculator pos_delta_calc,
    MinDeltaCalculator min_delta_calc,
    ExceptionThrower exception_thrower,
    InitialTargetFixer initial_target_fixer)
{
    Value cur_value = getter(from);

    double initial_delta = pos_delta_calc(cur_value, target_value);

    initial_target_fixer(target_value, initial_delta);

    JulDays_UT time{from + initial_delta * average_length};
    cur_value = getter(time);

    double prev_abs_delta = std::numeric_limits<double>::max();

    constexpr int max_iterations = 1'000;
    int iteration = 0;

    while (cur_value != target_value) {
        double const delta = min_delta_calc(cur_value, target_value);
        time += delta * average_length;
        cur_value = getter(time);

        double const abs_delta = fabs(delta);
        // Check for calculations loop: break if delta stopped decreasing (by absolute vbalue).
        if (abs_delta >= prev_abs_delta) {
            break;
        }
        prev_abs_delta = abs_delta;
        if (++iteration >= max_iterations) {
            exception_thrower(target_value, from);
        }
    }
    return time;
}

}

JulDays_UT Calc::find_exact_tithi_start(JulDays_UT from, Tithi tithi) const {
    return find_time_with_given_value(
        from,
        tithi,
        Tithi::AverageLength(),
        [this](JulDays_UT time) { return swe.tithi(time); },
        [](Tithi t1, Tithi t2) { return t1.positive_delta_until_tithi(t2); },
        [](Tithi t1, Tithi t2) { return t1.delta_to_nearest_tithi(t2); },
        [](Tithi target, JulDays_UT from) { throw CantFindTithiAfter{target, from}; },
        [](Tithi & /*target*/, double & /*delta*/) {} );
}

tl::expected<date::local_days, CalcError> Calc::find_exact_tithi_date(const JulDays_UT from, const DiscreteTithi tithi, const date::time_zone * tz) const
{
    const auto tithi_start = find_exact_tithi_start(from, Tithi{tithi});
    const auto sunrise = swe.next_sunrise(tithi_start);
    if (!sunrise) {
        return tl::make_unexpected(sunrise.error());
    }
    const auto date = date::floor<date::days>(sunrise->as_zoned_time(tz).get_local_time());
    return date;
}

JulDays_UT Calc::find_either_tithi_start(JulDays_UT from, Tithi tithi) const
{
    return find_time_with_given_value(
        from,
        tithi,
        Tithi::AverageLength(),
        [this](JulDays_UT time) { return swe.tithi(time); },
        [](Tithi t1, Tithi t2) { return t1.positive_delta_until_tithi(t2); },
        [](Tithi t1, Tithi t2) { return t1.delta_to_nearest_tithi(t2); },
        [](Tithi target, JulDays_UT from) { throw CantFindTithiAfter{target, from}; },
        [](Tithi & target, double & delta) {
            if (delta >= 15.0) {
                target += 15.0;
                delta -= 15.0;
            }
        }
        );
}

JulDays_UT Calc::find_nakshatra_start(const JulDays_UT from, const Nakshatra target_nakshatra) const
{
    return find_time_with_given_value(
        from,
        target_nakshatra,
        Nakshatra::AverageLength(),
        [this](JulDays_UT time) { return swe.nakshatra(time); },
        positive_delta_between_nakshatras,
        minimal_delta_between_nakshatras,
        [](Nakshatra target, JulDays_UT from) { throw CantFindNakshatraAfter{target, from}; },
        [](Nakshatra & /*target*/, double & /*delta*/) {}
    );
}

JulDays_UT Calc::find_sankranti(JulDays_UT after, Saura_Masa masa) const
{
    auto target_longitude = starting_longitude(masa);
    constexpr auto average_saura_masa_length_per_degree = date::years{1} / 360.0;
    return find_time_with_given_value(
        after,
        target_longitude,
        average_saura_masa_length_per_degree,
        [this](JulDays_UT time) { return swe.surya_nirayana_longitude(time); },
        positive_delta_between_longitudes,
        minimal_delta_between_longitudes,
        [&](Nirayana_Longitude target, JulDays_UT from) { throw CantFindSankrantiAfter{masa, target, from}; },
        [](Nirayana_Longitude & /*target*/, double & /*delta*/) {}
        );
}

Saura_Masa Calc::saura_masa(JulDays_UT time) const
{
    auto lng = swe.surya_nirayana_longitude(time);
    return Saura_Masa{1 + static_cast<int>(lng.longitude * (12.0/360.0))};
}

Chandra_Masa Calc::chandra_masa_amanta(JulDays_UT time, std::optional<JulDays_UT> *end_time) const
{
    auto amavasya2 = find_exact_tithi_start(time, Tithi::Amavasya_End()); // end of amavasya is start of shukla pratipat
    if (end_time) {
        *end_time = amavasya2;
    }
    constexpr auto saura_masa_max_length{double_days{32}};
    auto amavasya1 = find_exact_tithi_start(amavasya2 - saura_masa_max_length, Tithi::Amavasya_End()); // end of purnima is start of pratipat
    auto saura_masa1 = saura_masa(amavasya1);
    auto saura_masa2 = saura_masa(amavasya2);
    int delta = saura_masa2 - saura_masa1;
    if (delta == 1) {
        return Chandra_Masa{static_cast<std::underlying_type_t<vp::Saura_Masa>>(saura_masa2)};
    }
    if (delta == 0) {
        return Chandra_Masa::Adhika;
    }
    if (delta == 2) {
        return Chandra_Masa::Kshaya;
    }
    return Chandra_Masa{0};
}

Nirayana_Longitude starting_longitude(Saura_Masa m)
{
    using T = std::underlying_type_t<Saura_Masa>;
    return Nirayana_Longitude{(static_cast<T>(m)-1) * (360.0/12.0)};
}

} // namespace vp

#include "jayanti.h"

namespace vp {

tl::expected<std::vector<RohiniBahulashtamiYoga>, CalcError> rohini_bahulashtami_yogas_in_year(Calc &c, date::year year) {
    std::vector<RohiniBahulashtamiYoga> yogas;
    for (auto timepoint = JulDays_UT{year/1/1}; timepoint.year_month_day().year() == year;) {
        const auto rohini_start = c.find_nakshatra_start(timepoint, Nakshatra::ROHINI_START());
        const auto rohini_end = c.find_nakshatra_start(rohini_start, Nakshatra::ROHINI_END());
        const auto last_sunrise_before_rohini = c.prev_sunrise(rohini_start);
        if (!last_sunrise_before_rohini) return tl::make_unexpected(last_sunrise_before_rohini.error());
        const auto first_sunrise_after_rohini = c.swe.next_sunrise(rohini_end);
        if (!first_sunrise_after_rohini) return tl::make_unexpected(first_sunrise_after_rohini.error());

        for (auto sunrise = last_sunrise_before_rohini; sunrise && *sunrise < *first_sunrise_after_rohini; ) {
            const auto min_tithi = c.swe.tithi(*sunrise);
            const auto next_sunrise = c.swe.next_sunrise(*sunrise + double_days{0.001});
            if (!next_sunrise) return tl::make_unexpected(next_sunrise.error());
            const auto max_tithi = c.swe.tithi(*next_sunrise);
            if (min_tithi < Tithi::Krishna_Ashtami_End() && max_tithi >= Tithi::Krishna_Ashtami()) {
                const auto sunset = c.swe.next_sunset(*sunrise);
                if (!sunset) return tl::make_unexpected(sunset.error());
                const auto midnight = proportional_time(*sunset, *next_sunrise, 0.5);

                const auto k8_end = c.find_exact_tithi_start(*sunrise, Tithi::Krishna_Ashtami_End());
                const auto k8_start = c.find_exact_tithi_start(k8_end - Tithi::MaxLength(), Tithi::Krishna_Ashtami());
                const auto saura_masa_at_midnight = c.saura_masa(midnight);

                yogas.push_back({Interval{*sunrise, *next_sunrise},
                                 midnight, saura_masa_at_midnight,
                                 Interval{rohini_start, rohini_end},
                                 Interval{k8_start, k8_end}});
            }
            sunrise = next_sunrise;
        }
        timepoint = rohini_end;
    }
    std::sort(yogas.begin(), yogas.end(), [](const RohiniBahulashtamiYoga &a, const RohiniBahulashtamiYoga &b) {
        // simha_masa_on_midnight == true goes first. When equal, sort by a.kalpa descending (Kalpa1 is highest numerical value)
        bool result;
        if (a.simha_masa_at_midnight > b.simha_masa_at_midnight)
            result = true;
        else if (b.simha_masa_at_midnight > a.simha_masa_at_midnight)
            result = false;
        else
            result = a.kalpa() > b.kalpa();
        return result;
    });
    return yogas;
}

RoK8YogaKalpa RohiniBahulashtamiYoga::kalpa() const
{
//    if (!simha_masa_on_midnight)
//        return RoK8YogaKalpa::None;
    if (rohini_at_midnight && bahulashtami_at_midnight)
        return RoK8YogaKalpa::Kalpa1;
    else if (rohini_at_midnight)
        return RoK8YogaKalpa::Kalpa2;
    else if (bahulashtami_at_midnight)
        return RoK8YogaKalpa::Kalpa3;
    else if (bahulashtami_at_suryodaya && rohini_at_suryodaya)
        return RoK8YogaKalpa::Kalpa4_2;
    else if (bahulashtami_at_suryodaya)
        return RoK8YogaKalpa::Kalpa4_1;
    return RoK8YogaKalpa::None;
}

Interval intersect(const Interval &interval1, const Interval &interval2) {
    if (interval1.is_empty()) return interval1;
    if (interval2.is_empty()) return interval2;
    const auto start = std::max(interval1.start, interval2.start);
    const auto end = std::min(interval1.end, interval2.end);
    return Interval(start, end);
}

auto distance(const Interval &interval1, const Interval &interval2) {
    if (interval1.is_empty() || interval2.is_empty()) return double_days{std::numeric_limits<double>::infinity()};
    if (interval1.start > interval2.end) {
        return interval1.start - interval2.end;
    }
    if (interval2.start > interval1.end) {
        return interval2.start - interval1.end;
    }
    return double_days{0};
}

bool Interval::contains(JulDays_UT t) const
{
    return t >= start && t < end;
}

} // namespace vp

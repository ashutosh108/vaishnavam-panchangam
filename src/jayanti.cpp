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
                bool simha_masa_status = NoSimhaMasaOnMidnight;
                const auto sunset = c.swe.next_sunset(*sunrise);
                if (!sunset) return tl::make_unexpected(sunset.error());
                const auto midnight = proportional_time(*sunset, *next_sunrise, 0.5);
                const auto saura_masa = c.saura_masa(midnight);
                if (saura_masa == Saura_Masa::Simha) {
#ifdef DEBUG
//                  fmt::print("min_tithi({}) = {}; max_tithi({}) = {}\n", *sunrise, min_tithi, *next_sunrise, max_tithi);
#endif
                    simha_masa_status = SimhaMasaOnMidnight;
                }
                yogas.push_back({*sunrise, midnight, simha_masa_status});
            }
            sunrise = next_sunrise;
        }
        timepoint = rohini_end;
    }
    return yogas;
}

} // namespace vp

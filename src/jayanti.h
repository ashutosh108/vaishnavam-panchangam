#ifndef JAYANTI_H
#define JAYANTI_H

#include "calc.h"

namespace vp {

// Four kalpas for Rohini-bahulashtami-yoga (AKA Rohini-krishnashtami-yoga).
// Ordered by ascending priority: Kalpa1 is the best etc.
enum class RoK8YogaKalpa {
    None,
    Kalpa4_1,
    Kalpa4_2,
    Kalpa3,
    Kalpa2,
    Kalpa1,
};

struct Interval {
    JulDays_UT start{JulDays_UT{double_days{std::numeric_limits<double>::infinity()}}};
    JulDays_UT end{JulDays_UT{double_days{-std::numeric_limits<double>::infinity()}}};
    Interval(JulDays_UT start_, JulDays_UT end_) : start{start_}, end{end_} {}
    Interval()=default;
    bool is_empty() const {
        return start > end;
    }
    bool contains(JulDays_UT t) const;
};

Interval intersect(const Interval & interval1, const Interval & interval2);

auto distance(const Interval & interval1, const Interval & interval2);

/**
 * ‘Rohiṇī-bahulāṣṭamī-yoga’ is defined as a combination of Rohiṇī-nakṣatra
 * and Bahulāṣṭamī in the period of time from a sunrise to a next sunrise
 * (a definition of a ‘day’). In the majority of cases this combination
 * is in the form of an overlap, but in some rare years nakṣatra and tithi
 * start or end during the same day without overlapping.
 *
 * Current 2022 CE is one of such years, while the previous one was 2003 CE.
 *
 *
 * 1st Kalpa
 *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra both Rohiṇī
 *      and Bahulāṣṭamī are present. These are the ideal astronomical
 *      circumstances, coinsiding with those at the actual Appearance
 *      of Bhagavān Śrī-Kṛṣṇa, ‘sampūrṇo jayantī-kalpaḥ’.
 * 2nd Kalpa
 *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra only Rohiṇī
 *      is present.
 * 3rd Kalpa
 *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra only Bahulāṣṭamī
 *      is present.
 * 4th Kalpa
 *      Rohiṇī-bahulāṣṭamī-yoga, neither Rohiṇī, nor Bahulāṣṭamī is present
 *      at the time of ardharātra. This day is Jayantī nevertheless,
 *      provided Kalpas 1-3 are not to be found.
 */
struct RohiniBahulashtamiYoga {
    Interval sunrises;
    JulDays_UT midnight;
    bool simha_masa_at_midnight;
    bool rohini_at_midnight;
    bool bahulashtami_at_midnight;
    bool rohini_at_suryodaya;
    bool bahulashtami_at_suryodaya;
    bool real_intersection; // true if Ro & k8 actually intersect, covering non-empty time period on this solar day

    RohiniBahulashtamiYoga(
        Interval sunrises_,
        JulDays_UT midnight_,
        Saura_Masa saura_masa_on_midnight,
        Interval rohini,
        Interval k8)
        :
        sunrises(sunrises_),
        midnight(midnight_),
        simha_masa_at_midnight(saura_masa_on_midnight == Saura_Masa::Simha),
        rohini_at_midnight(rohini.contains(midnight_)),
        bahulashtami_at_midnight(k8.contains(midnight_)),
        rohini_at_suryodaya(rohini.contains(sunrises_.start)),
        bahulashtami_at_suryodaya(k8.contains(sunrises_.start))
    {
        real_intersection = !intersect(intersect(sunrises, rohini), k8).is_empty();
    }

    RoK8YogaKalpa kalpa() const;
};

tl::expected<std::vector<RohiniBahulashtamiYoga>, CalcError> rohini_bahulashtami_yogas_in_year(Calc &c, date::year year);

} // namespace vp

template<>
struct fmt::formatter<vp::RoK8YogaKalpa> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const vp::RoK8YogaKalpa & kalpa, FormatCtx & ctx) {
        const char * str = NULL;
        switch (kalpa) {
        case vp::RoK8YogaKalpa::None: str = "no kalpa"; break;
        case vp::RoK8YogaKalpa::Kalpa1: str = "kalpa 1"; break;
        case vp::RoK8YogaKalpa::Kalpa2: str = "kalpa 2"; break;
        case vp::RoK8YogaKalpa::Kalpa3: str = "kalpa 3"; break;
        case vp::RoK8YogaKalpa::Kalpa4_1: str = "kalpa 4.1"; break;
        case vp::RoK8YogaKalpa::Kalpa4_2: str = "kalpa 4.2"; break;
        }

        if (str)
            fmt::format_to(ctx.out(), FMT_STRING("{}"), str);
        else
            fmt::format_to(ctx.out(), FMT_STRING("(kalpa with code %d)"), static_cast<int>(kalpa));
        return ctx.out();
    }
};

template<>
struct fmt::formatter<vp::RohiniBahulashtamiYoga> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const vp::RohiniBahulashtamiYoga & yoga, FormatCtx & ctx) {
        fmt::format_to(
            ctx.out(),
            FMT_STRING("[{}: {}, {}, {}, {}, {}, {}, {}]"),
            yoga.kalpa(),
            yoga.sunrises.start,
            yoga.simha_masa_at_midnight ? "Simha" : "no Simha",
            yoga.rohini_at_midnight ? "Rohini" : "no Rohini",
            yoga.bahulashtami_at_midnight ? "K8" : "no K8",
            yoga.rohini_at_suryodaya ? "Rohini@SU" : "no Rohini@SU",
            yoga.bahulashtami_at_suryodaya ? "K8@SU" : "no K8@SU",
            yoga.real_intersection ? "X" : "-"
            );
        return ctx.out();
    }
};

template<>
struct fmt::formatter<std::vector<vp::RohiniBahulashtamiYoga>> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const std::vector<vp::RohiniBahulashtamiYoga> & yogas, FormatCtx & ctx) {
        fmt::format_to(ctx.out(), FMT_STRING("RohiniBahulashtamiYogas{{\n"));
        for (const auto & yoga : yogas)
            fmt::format_to(ctx.out(), FMT_STRING("{},\n"), yoga);
        fmt::format_to(ctx.out(), FMT_STRING("}}"));
        return ctx.out();
    }
};

#endif // JAYANTI_H

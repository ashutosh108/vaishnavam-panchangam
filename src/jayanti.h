#ifndef JAYANTI_H
#define JAYANTI_H

#include "calc.h"

namespace vp {

enum SimhaMasaStatus:bool {
    NoSimhaMasaOnMidnight = false,
    SimhaMasaOnMidnight = true
};

struct RohiniBahulashtamiYoga {
    JulDays_UT sunrise;
    JulDays_UT midnight;
    bool simha_masa_on_midnight;
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

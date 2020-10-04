#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"
#include "fmt-format-fixed.h"
#include <string_view>

namespace vp {

struct Vrata_Detail_Printer {
    Vrata vrata;
    Vrata_Detail_Printer(Vrata _vrata);
    struct NamedTimePoint {
        std::string name;
        JulDays_UT time_point;
    };

    std::vector<NamedTimePoint> events;
};

} // namespace vp

template<>
struct fmt::formatter<vp::Vrata_Detail_Printer> : fmt::formatter<std::string_view>{
    bool use_compact_form = false;
    template<typename ParseCtx>
    auto parse(ParseCtx & ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it == 'c') {
            ++it;
            use_compact_form = true;
        }
        return it;
    }
    static void merge_consequent_events_with_same_time(std::vector<vp::Vrata_Detail_Printer::NamedTimePoint> & events) {
        for (std::size_t i = 1; i < events.size(); ++i) {
            auto time1 = events[i-1].time_point;
            auto time2 = events[i].time_point;
            // same time for both events. Merge them by appending descriptions via comma.
            if (time1 == time2) {
                events[i-1].name += ", " + events[i].name;
                events.erase(events.begin() + static_cast<int>(i));
            }
        }
    }

    template<typename FormatCtx>
    auto format(const vp::Vrata_Detail_Printer & vd, FormatCtx & ctx) {
        if (!use_compact_form) {
            fmt::format_to(
                ctx.out(),
                "# {0}\n"
                "{1}:\n"
                "{2}\n",
                vd.vrata.location_name(),
                vd.vrata,
                vd.vrata.paran.type);
        }
        fmt::format_to(ctx.out(), "Coordinates: {} {}\n", vd.vrata.location.latitude, vd.vrata.location.longitude);
        fmt::format_to(ctx.out(), "Time zone: {}\n", vd.vrata.location.time_zone_name);
        auto events = vd.events;
        // stable sort to keep "pAraNam start/end" after corresponding sunrise+ events.
        std::stable_sort(events.begin(), events.end(), [](const vp::Vrata_Detail_Printer::NamedTimePoint & left, const vp::Vrata_Detail_Printer::NamedTimePoint & right) {
            return left.time_point < right.time_point;
        });
        merge_consequent_events_with_same_time(events);
        for (const auto & e : events) {
            fmt::format_to(ctx.out(), "{} {}\n", vp::JulDays_Zoned{vd.vrata.location.time_zone(), e.time_point}, e.name);
        }
        return ctx.out();
    }
};

#endif // VRATA_DETAIL_H

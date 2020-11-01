#ifndef VP_DATE_FIXED_H
#define VP_DATE_FIXED_H
#if defined(WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#undef min
#undef max
#endif

#include "disable-warning.h"
DISABLE_WARNING_PUSH
DISABLE_WARNING_USELESS_CAST
#include "date/date.h"
DISABLE_WARNING_POP

#include "fmt-format-fixed.h"

template<>
struct fmt::formatter<date::year_month_day> {
    bool as_local = false;
    bool use_nbsp = false;
    bool print_year = false;
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) {
        for (auto it = ctx.begin(), end = ctx.end(); it != end; ++it) {
            if (*it == 'l') {
                as_local = true;
            } else if (*it == 'h') {
                use_nbsp = true;
            } else if (*it == 'y') {
                print_year = true;
            } else {
                return it;
            }
        }
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const date::year_month_day & ymd, FormatContext & ctx) {
        static constexpr const char * month_name[] = {
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December",
        };
        const int year = static_cast<int>(ymd.year());
        const unsigned month = static_cast<unsigned>(ymd.month());
        const unsigned day = static_cast<unsigned>(ymd.day());
        if (as_local && ymd.ok()) {
            if (print_year) {
                return fmt::format_to(ctx.out(), "{}{}{}, {}", month_name[month-1], (use_nbsp ? "&nbsp;" : " "), day, year);
            } else {
                return fmt::format_to(ctx.out(), "{}{}{}", month_name[month-1], (use_nbsp ? "&nbsp;" : " "), day);
            }

        } else {
            return fmt::format_to(ctx.out(), "{:04}-{:02}-{:02}", year, month, day);
        }
    }
};

template<typename Duration>
struct fmt::formatter<date::hh_mm_ss<Duration>> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const date::hh_mm_ss<Duration> & hms, FormatContext & ctx) {
        const long long ms = date::round<std::chrono::microseconds>(hms.to_duration()).count();
        const int hours = static_cast<int>(ms / (1'000'000LL * 3'600LL));
        const int minutes = static_cast<int>((ms / (1'000'000LL * 60LL)) % 60);
        const int seconds = static_cast<int>((ms / 1'000'000LL) % 60);
        const int ms_rem = static_cast<int>(ms % 1'000'000LL);
        return fmt::format_to(ctx.out(), "{:02}:{:02}:{:02}.{:06}", hours, minutes, seconds, ms_rem);
    }
};

template<typename Duration>
struct fmt::formatter<date::local_time<Duration>> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const date::local_time<Duration> & t, FormatContext & ctx) {
        const auto daypoint = date::floor<date::days>(t);
        const auto ymd = date::year_month_day{daypoint};
        const auto tod = date::hh_mm_ss<Duration>{t - daypoint};
        return fmt::format_to(ctx.out(), "{} {}", ymd, tod);
    }
};

namespace std {
template<> struct hash<date::year_month_day> {
    std::size_t operator() (const date::year_month_day & date) const {
        return (std::hash<int>{}(date.year().operator int()) << 2)
               ^ (std::hash<unsigned>{}(date.month().operator unsigned int()) << 1 )
               ^ (std::hash<unsigned>{}(date.day().operator unsigned int()));
    }
};
}

#endif // #ifndef VP_DATE_FIXED_H

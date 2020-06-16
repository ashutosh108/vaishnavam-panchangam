#ifndef VP_DATE_FIXED_H
#define VP_DATE_FIXED_H
#if defined(WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#undef min
#undef max
#endif

#include "date/date.h"

#include <fmt/format.h>

template<>
struct fmt::formatter<date::year_month_day> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const date::year_month_day & ymd, FormatContext & ctx) {
        const int year = (int)ymd.year();
        const unsigned month = (unsigned)ymd.month();
        const unsigned day = (unsigned)ymd.day();
        return fmt::format_to(ctx.out(), "{:04}-{:02}-{:02}", year, month, day);
    }
};

#endif // #ifndef VP_DATE_FIXED_H

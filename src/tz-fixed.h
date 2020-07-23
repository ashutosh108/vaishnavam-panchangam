/* Since we need to include tz from few places and we need to disable warning every time,
 * we define our own include. */

#ifndef TZ_FIXED_H
#define TZ_FIXED_H

#include "date-fixed.h"
#include "disable-warning.h"

#include "fmt-format-fixed.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_SHADOW
#include "date/tz.h"
DISABLE_WARNING_POP

template<typename Duration>
struct fmt::formatter<date::zoned_time<Duration>> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const date::zoned_time<Duration> & t, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), "{} {}", t.get_local_time(), t.get_info().abbrev);
    }
};

#endif

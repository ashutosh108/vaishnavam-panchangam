#ifndef CALCERROR_H
#define CALCERROR_H

#include "juldays_ut.h"
#include "tithi.h"

#include "fmt-format-fixed.h"
#include <string>
#include <string_view>
#include <variant>

namespace vp {

struct CantFindSunriseAfter {
    JulDays_UT after;
};

struct CantFindSunriseBefore {
    JulDays_UT before;
};

struct CantFindSunsetAfter {
    JulDays_UT after;
};

struct CantFindLocation {
    std::string location_name;
};

struct NoRohiniAshtamiIntersectionForJayanti {
};

using CalcError = std::variant<CantFindSunriseAfter, CantFindSunriseBefore, CantFindSunsetAfter, CantFindLocation, NoRohiniAshtamiIntersectionForJayanti>;

} // namespace vp

template<>
struct fmt::formatter<vp::CalcError> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const vp::CalcError & err, FormatCtx & ctx) {
        if (std::holds_alternative<vp::CantFindSunriseAfter>(err)) {
            return fmt::format_to(ctx.out(), "Can't find sunrise after {}", std::get<vp::CantFindSunriseAfter>(err).after);
        } else if (std::holds_alternative<vp::CantFindSunriseBefore>(err)) {
            return fmt::format_to(ctx.out(), "Can't find sunrise after {}", std::get<vp::CantFindSunriseBefore>(err).before);
        } else if (std::holds_alternative<vp::CantFindSunsetAfter>(err)) {
            return fmt::format_to(ctx.out(), "Can't find sunset after {}", std::get<vp::CantFindSunsetAfter>(err).after);
        } else if (std::holds_alternative<vp::CantFindLocation>(err)) {
            return fmt::format_to(ctx.out(), "Can't find given location \"{}\" by name", std::get<vp::CantFindLocation>(err).location_name);
        }
        throw std::runtime_error("Internal error: unknown variant in CalcError");
    }
};

#endif // CALCERROR_H

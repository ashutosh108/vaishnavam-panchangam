#ifndef CATCHFORMATTERS_H
#define CATCHFORMATTERS_H

#include "juldays_ut.h"

#include "catch.hpp"
#include <optional>

namespace Catch {
template<>
struct StringMaker<vp::JulDays_UT> {
    static std::string convert(const vp::JulDays_UT & t) {
        return fmt::to_string(t);
    }
};
}

namespace Catch {
template<>
struct StringMaker<std::optional<vp::JulDays_UT>> {
    static std::string convert(const std::optional<vp::JulDays_UT> & t) {
        if (t) {
            return fmt::to_string(*t);
        }
        return "[nullopt]";
    }
};
}

#endif // CATCHFORMATTERS_H

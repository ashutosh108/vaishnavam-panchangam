#ifndef CATCHFORMATTERS_H
#define CATCHFORMATTERS_H

#include "juldays_ut.h"

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include "catch2/catch.hpp"
#include <optional>

namespace Catch {
template<>
struct StringMaker<vp::JulDays_UT> {
    static std::string convert(const vp::JulDays_UT & t) {
        return fmt::to_string(t);
    }
};
}

#endif // CATCHFORMATTERS_H

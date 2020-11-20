#ifndef CATCHFORMATTERS_H
#define CATCHFORMATTERS_H

#include "juldays_ut.h"
#include "nakshatra.h"

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

template<>
struct StringMaker<vp::Nakshatra> {
    static std::string convert(const vp::Nakshatra & n) {
        return fmt::to_string(n);
    }
};
}

#endif // CATCHFORMATTERS_H

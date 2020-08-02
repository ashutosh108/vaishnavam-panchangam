#ifndef CATCHFORMATTERS_H
#define CATCHFORMATTERS_H

#include "catch.hpp"

#include "juldays_ut.h"

namespace Catch {
template<>
struct StringMaker<vp::JulDays_UT> {
    static std::string convert(const vp::JulDays_UT & t) {
        return fmt::to_string(t);
    }
};
}

#endif // CATCHFORMATTERS_H

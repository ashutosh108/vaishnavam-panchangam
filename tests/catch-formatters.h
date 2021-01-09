#ifndef CATCHFORMATTERS_H
#define CATCHFORMATTERS_H

#include "juldays_ut.h"
#include "nakshatra.h"
#include "tithi.h"
#include "vrata.h"

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

template<>
struct StringMaker<vp::Tithi> {
    static std::string convert(const vp::Tithi & t) {
        return fmt::format(FMT_STRING("Tithi({})"), t.tithi);
    }
};

template<>
struct StringMaker<vp::Nirayana_Longitude> {
    static std::string convert(const vp::Nirayana_Longitude & l) {
        return fmt::format(FMT_STRING("Nirayana_Longitude({})"), l.longitude);
    }
};

template<>
struct StringMaker<vp::Vrata> {
    static std::string convert(const vp::Vrata & v) {
        return fmt::format(FMT_STRING("{}"), v);
    }
};

template<>
struct StringMaker<vp::DiscreteTithi> {
    static std::string convert(const vp::DiscreteTithi t) {
        return fmt::format(FMT_STRING("{}"), t);
    }
};
}

#endif // CATCHFORMATTERS_H

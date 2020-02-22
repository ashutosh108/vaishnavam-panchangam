#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

namespace vp {

struct Vrata_Detail {
    Vrata vrata;
    Location location;
    Calc calc;
    Vrata_Detail(Vrata _vrata, Location _location);
    struct NamedTimePoint {
        std::string name;
        std::optional<JulDays_UT> time_point;
    };

    std::optional<JulDays_UT> ekadashi_start;
    std::vector<NamedTimePoint> events;
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail const &vd);

} // namespace vp

#endif // VRATA_DETAIL_H

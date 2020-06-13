#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

namespace vp {

struct Vrata_Detail_Printer {
    Vrata vrata;
    Vrata_Detail_Printer(Vrata _vrata, Swe swe);
    struct NamedTimePoint {
        std::string name;
        JulDays_UT time_point;
    };

    std::vector<NamedTimePoint> events;
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail_Printer const &vd);

} // namespace vp

#endif // VRATA_DETAIL_H

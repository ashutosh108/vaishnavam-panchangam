#ifndef CALCERROR_H
#define CALCERROR_H

#include "juldays_ut.h"
#include "tithi.h"

#include <string>
#include <variant>

namespace vp {

struct CantFindSunriseAfter {
    JulDays_UT after;
};

struct CantFindSunsetAfter {
    JulDays_UT after;
};

struct CantFindLocation {
    std::string location_name;
};

using CalcError = std::variant<CantFindSunriseAfter, CantFindSunsetAfter, CantFindLocation>;

std::ostream & operator<<(std::ostream & o, const CalcError & err);

} // namespace vp

#endif // CALCERROR_H

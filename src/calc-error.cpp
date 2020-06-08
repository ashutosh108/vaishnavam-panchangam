#include "calc-error.h"

namespace vp {

std::ostream & operator<<(std::ostream & o, const CalcError & err) {
    if (std::holds_alternative<CantFindSunriseAfter>(err)) {
        o << "Can't find sunrise after " << std::get<CantFindSunriseAfter>(err).after;
    } else if (std::holds_alternative<CantFindSunsetAfter>(err)) {
        o << "Can't find sunset after " << std::get<CantFindSunsetAfter>(err).after;
    } else if (std::holds_alternative<CantFindLocation>(err)) {
        o << "Can't find given location \"" << std::get<CantFindLocation>(err).location_name << "\" by name";
    }
    return o;
}

}

#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

struct Vrata_Detail {
    Vrata vrata;
    Coord coord;
    Calc calc;
    Vrata_Detail(Vrata _vrata, Coord _coord):vrata(_vrata), coord(_coord), calc(_coord) {}
    Swe_Time get_approx_local_midnight() const;
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail const &vd);

#endif // VRATA_DETAIL_H

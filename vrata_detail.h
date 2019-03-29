#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

struct Vrata_Detail {
    Vrata vrata;
    Coord coord;
    Vrata_Detail(Vrata _vrata, Coord _coord):vrata(_vrata), coord(_coord) {}
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail const &vd);

#endif // VRATA_DETAIL_H

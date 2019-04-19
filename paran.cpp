#include "paran.h"

std::ostream &Paran::operator<<(std::ostream &o) const
{
    switch (type) {
        case Standard: o << "Paran"; break;
        case Until_Dvadashi_End: o << "Paran_Until_Dvadashi_End"; break;
        case From_Quarter_Dvadashi: o << "Paran_From_Quarter_Dvadashi"; break;
    }
    o << "{";
    if (paran_start.has_value()) {
        o << *paran_start;
    }
    o << "..";
    if (paran_end.has_value()) {
        o << *paran_end;
    }
    o << "}";
    return o;
}

std::ostream &operator<<(std::ostream &o, const Paran &p)
{
    return p.operator<<(o);
}

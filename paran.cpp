#include "paran.h"

std::ostream &Paran::operator<<(std::ostream &o) const
{
    o << "Paran{";
    if (paran_start.has_value()) {
        o << *paran_start;
    }
    o << "..";
    if (paran_end.has_value()) {
        o << *paran_end;
    }
    return o;
}

std::ostream &operator<<(std::ostream &o, const Paran &p)
{
    return p.operator<<(o);
}

#include "paran.h"

namespace vp {

std::ostream &Paran::operator<<(std::ostream &o) const
{
    o << type;
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

std::ostream &operator<<(std::ostream &s, const Paran::Type &t)
{
    switch (t) {
    case Paran::Type::Standard:
        s << "Paran"; break;
    case Paran::Type::Until_Dvadashi_End:
        s << "Paran_Until_Dvadashi_End"; break;
    case Paran::Type::From_Quarter_Dvadashi:
        s << "Paran_From_Quarter_Dvadashi"; break;
    }
    return s;
}

} // namespace vp

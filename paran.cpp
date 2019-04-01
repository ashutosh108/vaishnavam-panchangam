#include "paran.h"

std::ostream &Paran::operator<<(std::ostream &o) const
{
    o << "Paran{";
    if (type == Type::before) {
        o << "before ";
        if (time1) {
            o << *time1;
        } else {
            o << "(nullopt)";
        }
    }
    o << '}';
    return o;
}

std::ostream &operator<<(std::ostream &o, const Paran &p)
{
    return p.operator<<(o);
}

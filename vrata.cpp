#include "vrata.h"

bool operator==(const Vrata &v1, const Vrata &v2)
{
    return v1.type == v2.type && v1.date == v2.date && v1.paran.type == v2.paran.type;
}

bool operator!=(const Vrata &v1, const Vrata &v2)
{
    return !(v1 == v2);
}

std::ostream &operator<<(std::ostream &o, const Vrata &v)
{
    o << v.type << "{" << v.date;
    o << ", " << v.paran;
    return o << "}";
}

std::ostream &operator<<(std::ostream &o, const Vrata_Type &v)
{
    switch (v) {
    case Vrata_Type::Ekadashi:
        o << "Ekadashi"; break;
    case Vrata_Type::Sandigdha_Ekadashi:
        o << "Sandigdha_Ekadashi"; break;
//    default:
//        o << "Vrata#" << static_cast<int>(v);
    }
    return o;
}

#include "vrata.h"

namespace vp {

bool operator==(const Vrata &v1, const Vrata &v2)
{
    // non-symmetrical compare for more elegant tests:
    // if v1's paran_start/end is nullopt, then v2's paran_start_end can be anything.
    // But if v1's paran_start/end is set, then v2's one must match.
    return v1.type == v2.type && v1.date == v2.date && v1.paran.type == v2.paran.type &&
            (
                (!v1.paran.paran_start.has_value()) ||
                (v1.paran.paran_start == v2.paran.paran_start)
            ) &&
            (
                (!v1.paran.paran_end.has_value()) ||
                (v1.paran.paran_end == v2.paran.paran_end)
            );
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
    case Vrata_Type::With_Atirikta_Dvadashi:
        o << "With_Atirikta_Dvadashi"; break;
    case Vrata_Type::Sandigdha_With_Atirikta_Dvadashi:
        o << "Sandigdha_With_Atirikta_Dvadashi"; break;
//    default:
//        o << "Vrata#" << static_cast<int>(v);
    }
    return o;
}

} // namespace vp

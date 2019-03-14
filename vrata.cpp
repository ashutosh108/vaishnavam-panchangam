#include "vrata.h"

bool operator==(const Vrata &v1, const Vrata &v2)
{
    return v1.type == v2.type && v1.date == v2.date;
}

bool operator!=(const Vrata &v1, const Vrata &v2)
{
    return !(v1 == v2);
}

std::ostream &operator<<(std::ostream &o, const Vrata &v)
{
    o << "Vrata{" << v.date;
    if (v.sunrise) {
        o << ", sunrise=" << v.sunrise.value();
        if (v.tithi_sunrise) {
            o << "(" << *v.tithi_sunrise << ")";
        }
    }
    if (v.prev_sunset) {
        o << ", prev_sunset=" << *v.prev_sunset;
    }
    if (v.arunodaya) {
        o << ", arunodaya=" << *v.arunodaya;
        if (v.tithi_arunodaya) {
            o << "(" << *v.tithi_arunodaya << ")";
        }
    }
    return o << "}";
}

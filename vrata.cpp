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
    return o << "Vrata{" << v.date << "}";
}

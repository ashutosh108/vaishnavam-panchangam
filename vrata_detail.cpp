#include "vrata_detail.h"


std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    s << vd.vrata;
    return s;
}

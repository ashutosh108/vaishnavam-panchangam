#include "vrata_detail.h"

#include "swe.h"
#include "swe_time.h"


std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    Swe swe;
    s << vd.vrata;


    if (vd.vrata.sunrise) {
        std::optional<Swe_Time> sunrise_plus1 = swe.get_sunrise(Swe_Time{vd.vrata.sunrise->as_julian_days()+0.1}, vd.coord);
        std::optional<Swe_Time> sunrise_plus2 = swe.get_sunrise(Swe_Time{vd.vrata.sunrise->as_julian_days()+1.1}, vd.coord);
        Tithi t1{swe.get_tithi(*sunrise_plus1)};
        Tithi t2{swe.get_tithi(*sunrise_plus2)};
        s << ", sunrise+1=" << *sunrise_plus1 << "(" << t1 << ")";
        s << ", sunrise+2=" << *sunrise_plus2 << "(" << t2 << ")";
    }
    return s;
}

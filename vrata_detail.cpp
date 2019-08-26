#include "vrata_detail.h"

#include "calc.h"

Swe_Time Vrata_Detail::get_approx_local_midnight() const {
    double adjustment = coord.longitude * (1.0/360.0);
    return Swe_Time{Swe_Time{vrata.date} - adjustment};
}


std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    s << vd.vrata << ":\n";
    Swe_Time local_midnight = vd.get_approx_local_midnight();
    auto sunrise = vd.calc.swe.get_sunrise(local_midnight);
    if (sunrise) {
        auto arunodaya_pair = vd.calc.get_arunodaya(*sunrise);
        if (arunodaya_pair) {
            s << "    Arddha-ghatika before arunodaya: " << arunodaya_pair->second << '\n';
            s << "    Arunodaya:                       " << arunodaya_pair->first << '\n';
        }
        s << "    Sunrise:                         " << *sunrise << '\n';
        auto ekadashi_start = vd.calc.get_next_tithi_start(local_midnight-1.0, Tithi{Tithi::Ekadashi});
        if (ekadashi_start) {
            s << "    Ekadashi start:                  " << *ekadashi_start << '\n';
            auto dvadashi_start = vd.calc.get_next_tithi_start(*ekadashi_start, Tithi{Tithi::Dvadashi});
            if (dvadashi_start) {
                s << "    Dvadashi start:                  " << *dvadashi_start << '\n';
                auto dvadashi_end = vd.calc.get_next_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
                if (dvadashi_end) {
                    s << "    Dvadashi end:                    " << *dvadashi_end << '\n';
                }
            }
        }
    }
    s << "    Paran type:                      " << vd.vrata.paran.type << '\n';
    if (vd.vrata.paran.paran_start) {
        s << "    Paran start:                     " << *vd.vrata.paran.paran_start << '\n';
    }
    if (vd.vrata.paran.paran_end) {
        s << "    Paran end:                       " << *vd.vrata.paran.paran_end << '\n';
    }
    return s;
}

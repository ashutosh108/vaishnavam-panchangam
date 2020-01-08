#include "vrata_detail.h"

#include "calc.h"

namespace vp {

Vrata_Detail::Vrata_Detail(Vrata _vrata, Location _coord):vrata(_vrata), coord(_coord), calc(_coord) {
    JulDays_UT local_midnight = get_approx_local_midnight();
    sunrise = calc.swe.get_sunrise(local_midnight);
    if (sunrise) {
        auto arunodaya_pair = calc.get_arunodaya(*sunrise);
        if (arunodaya_pair) {
            arddha_ghatika_before_arunodaya = arunodaya_pair->second;
            arunodaya = arunodaya_pair->first;
        }
        // -1.0 to ensure we actually select start time before Ekadashi.
        // Not 100% sure it's enought, but it's working for all test cases so far.
        ekadashi_start = calc.get_next_tithi_start(local_midnight-double_days{1.0}, Tithi{Tithi::Ekadashi});
        if (ekadashi_start) {
            dvadashi_start = calc.get_next_tithi_start(*ekadashi_start, Tithi{Tithi::Dvadashi});
            if (dvadashi_start) {
                dvadashi_end = calc.get_next_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
                dvadashi_quarter = Calc::proportional_time(*dvadashi_start, *dvadashi_end, 0.25);
            }
        }
    }
}

JulDays_UT Vrata_Detail::get_approx_local_midnight() const {
    double_days adjustment{coord.longitude * (1.0/360.0)};
    return JulDays_UT{JulDays_UT{vrata.date} - adjustment};
}


std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    auto z = [&vd](JulDays_UT t) {
        return JulDays_Zoned{vd.coord.timezone_name, t};
    };
    s << vd.vrata << ":\n";
    if (vd.arddha_ghatika_before_arunodaya) {
        s << "    Arddha-ghatika before arunodaya: " << z(*vd.arddha_ghatika_before_arunodaya) << '\n';
    }

    if (vd.arunodaya) {
        s << "    Arunodaya:                       " << z(*vd.arunodaya) << '\n';
    }

    if (vd.sunrise) {
        s << "    Sunrise:                         " << z(*vd.sunrise) << '\n';
    }

    if (vd.ekadashi_start) {
        s << "    Ekadashi start:                  " << z(*vd.ekadashi_start) << '\n';
    }

    if (vd.dvadashi_start) {
        s << "    Dvadashi start:                  " << z(*vd.dvadashi_start) << '\n';
    }

    if (vd.dvadashi_quarter) {
        s << "    Dvadashi quarter:                " << z(*vd.dvadashi_quarter) << '\n';
    }

    if (vd.dvadashi_end) {
        s << "    Dvadashi end:                    " << z(*vd.dvadashi_end) << '\n';
    }

    s << "    Paran type:                      " << vd.vrata.paran.type << '\n';

    if (vd.vrata.paran.paran_start) {
        s << "    Paran start:                     " << z(*vd.vrata.paran.paran_start) << '\n';
    }

    if (vd.vrata.paran.paran_end) {
        s << "    Paran end:                       " << z(*vd.vrata.paran.paran_end) << '\n';
    }

    return s;
}

} // namespace vp

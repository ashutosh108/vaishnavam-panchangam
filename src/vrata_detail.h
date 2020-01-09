#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

namespace vp {

struct Vrata_Detail {
    Vrata vrata;
    Location coord;
    Calc calc;
    Vrata_Detail(Vrata _vrata, Location _coord);
    JulDays_UT get_approx_local_midnight() const;
    std::optional<JulDays_UT> arddha_ghatika_before_arunodaya;
    std::optional<JulDays_UT> arunodaya;
    std::optional<JulDays_UT> sunrise;
    std::optional<JulDays_UT> ekadashi_start;
    std::optional<JulDays_UT> dvadashi_start;
    std::optional<JulDays_UT> dvadashi_end;
    std::optional<JulDays_UT> dvadashi_quarter;
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail const &vd);

} // namespace vp

#endif // VRATA_DETAIL_H

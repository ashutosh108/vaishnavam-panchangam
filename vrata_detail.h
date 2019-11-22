#ifndef VRATA_DETAIL_H
#define VRATA_DETAIL_H

#include "calc.h"

struct Vrata_Detail {
    Vrata vrata;
    Location coord;
    Calc calc;
    Vrata_Detail(Vrata _vrata, Location _coord);
    Swe_Time get_approx_local_midnight() const;
    std::optional<Swe_Time> arddha_ghatika_before_arunodaya;
    std::optional<Swe_Time> arunodaya;
    std::optional<Swe_Time> sunrise;
    std::optional<Swe_Time> ekadashi_start;
    std::optional<Swe_Time> dvadashi_start;
    std::optional<Swe_Time> dvadashi_end;
    std::optional<Swe_Time> dvadashi_quarter;
};

std::ostream &operator<<(std::ostream &s, Vrata_Detail const &vd);

#endif // VRATA_DETAIL_H

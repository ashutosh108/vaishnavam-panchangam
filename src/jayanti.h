#ifndef JAYANTI_H
#define JAYANTI_H

#include "calc.h"

namespace vp {

enum SimhaMasaStatus:bool {
    NoSimhaMasaOnMidnight = false,
    SimhaMasaOnMidnight = true
};

struct RohiniBahulashtamiYoga {
    JulDays_UT sunrise;
    JulDays_UT midnight;
    bool simha_masa_on_midnight;
};

tl::expected<std::vector<RohiniBahulashtamiYoga>, CalcError> rohini_bahulashtami_yogas_in_year(Calc &c, date::year year);

} // namespace vp

#endif // JAYANTI_H

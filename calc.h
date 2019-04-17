#ifndef CALC_H
#define CALC_H

#include <optional>

#include "coord.h"
#include "date.h"
#include "swe.h"
#include "swe_time.h"
#include "tithi.h"
#include "vrata.h"

class Calc
{
public:
    Calc(Coord coord);
    // main interface: get info for nearest future Vrata after given date
    std::optional<Vrata> find_next_vrata(Date after) const;

    // Helper functions. They are public for easier testing,
    // but should be considered private otherwise.

    // find next sunrise which happens to be within Ekadashi tithi
    std::optional<Swe_Time> find_next_ekadashi_sunrise(Swe_Time after) const;

    // get arunodaya(= sunrise - night_length/8) for given sunrise
    std::optional<std::pair<Swe_Time, Swe_Time>> get_arunodaya(Swe_Time sunrise) const;

private:
    Swe swe;
    std::optional<Swe_Time> get_prev_sunset(Swe_Time const sunrise) const;
};

#endif // CALC_H

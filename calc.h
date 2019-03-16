#ifndef CALC_H
#define CALC_H

#include <optional>

#include "coord.h"
#include "date.h"
#include "swe_time.h"
#include "tithi.h"
#include "vrata.h"

class Calc
{
public:
    std::optional<Swe_Time> find_next_ekadashi_sunrise(Swe_Time after, Coord coord);
    std::optional<Vrata> find_next_vrata(Date after, Coord coord);
    std::optional<std::pair<Swe_Time, Swe_Time>> get_arunodaya(Swe_Time sunrise, Coord coord);
};

#endif // CALC_H

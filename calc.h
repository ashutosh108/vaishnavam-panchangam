#ifndef CALC_H
#define CALC_H

#include <optional>

#include "swe_time.h"


class Calc
{
public:
    std::optional<Swe_Time> find_next_ekadashi_sunrise(Swe_Time after, double latitude, double longitude);
};

#endif // CALC_H

#ifndef CALC_H
#define CALC_H

#include <optional>

#include "coord.h"
#include "date.h"
#include "swe_time.h"
#include "tithi.h"

enum class Vrata_Type {
    Shuddha_Ekadashi,
//    Atirikta,
};

struct Vrata {
    Vrata_Type type;
    Date date;
//    Swe_Time arunodaya;
//    Tithi tithi_sunrise;
//    Tithi tithi_arunodaya;
//    Swe_Time paran_sunrise;
//    Swe_Time paran_sunset;
//    Tithi tithi_paran_start;
//    Swe_Time paran_start;
//    Tithi tithi_paran_end;
//    Swe_Time paran_end;
};

class Calc
{
public:
    std::optional<Swe_Time> find_next_ekadashi_sunrise(Swe_Time after, Coord coord);
    std::optional<Vrata> find_next_vrata(Date after, Coord coord);
    std::optional<Swe_Time> get_arunodaya(Swe_Time sunrise, Coord coord);
};

#endif // CALC_H

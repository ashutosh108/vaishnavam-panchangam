#ifndef CALC_H
#define CALC_H

#include <optional>

#include "swe_time.h"

enum class Paksha {
    Shukla,
    Krishna
};

enum class Vrata_Type {
    Shuddha_Ekadashi,
//    Atirikta,
};

struct Vrata {
    Paksha paksha;
    Vrata_Type type;
//    Swe_Time prev_sunrise;
//    Tithi tithi_arunodaya;
//    Swe_Time arunodaya;
//    Tithi tithi_sunrise;
    Swe_Time sunrise;
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
    std::optional<Swe_Time> find_next_ekadashi_sunrise(Swe_Time after, double latitude, double longitude);
    std::optional<Vrata> find_next_vrata(Swe_Time after, double latitude, double longitude);
};

#endif // CALC_H

#ifndef VRATA_H
#define VRATA_H

#include <optional>
#include <ostream>

#include "date.h"
#include "swe_time.h"

enum class Vrata_Type {
    Ekadashi,
//    Atirikta,
};

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    Date date;
    std::optional<Swe_Time> vrata_sunrise;
    Vrata(Date _date) : date(_date){}
    Vrata(Date _date, Swe_Time _vrata_sunrise) : date(_date), vrata_sunrise(_vrata_sunrise){}
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

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);
std::ostream &operator<<(std::ostream &o, Vrata const &v);

#endif // VRATA_H

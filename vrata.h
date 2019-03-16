#ifndef VRATA_H
#define VRATA_H

#include <optional>
#include <ostream>

#include "date.h"
#include "swe_time.h"
#include "tithi.h"

enum class Vrata_Type {
    Ekadashi,
    Sandigdha_Ekadashi, // when Ekadashi Tithi starts before Arunodaya, but less than 0.25 muhurtas before.
//    Atirikta,
};

std::ostream &operator<<(std::ostream &o, Vrata_Type const &v);

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    Date date;
    std::optional<Swe_Time> sunrise;
    std::optional<Tithi> tithi_sunrise;
    std::optional<Swe_Time> prev_sunset;
    std::optional<Swe_Time> arunodaya;
    std::optional<Tithi> tithi_arunodaya;

    Vrata(Date _date) : date(_date){}
    Vrata(Vrata_Type _type, Date _date)
        : type(_type),
          date(_date){}
    Vrata(Vrata_Type _type, Date _date, Swe_Time _vrata_sunrise, Tithi _tithi_sunrise, Swe_Time _prev_sunset, Swe_Time _arunodaya, Tithi _tithi_arunodaya)
        : type(_type),
          date(_date),
          sunrise(_vrata_sunrise),
          tithi_sunrise(_tithi_sunrise),
          prev_sunset(_prev_sunset),
          arunodaya(_arunodaya),
          tithi_arunodaya(_tithi_arunodaya)
    {}
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

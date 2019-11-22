#ifndef VRATA_H
#define VRATA_H

#include <optional>
#include <ostream>

#include "date/date.h"
#include "paran.h"
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
    date::year_month_day date;
    Paran paran;

    Vrata(date::year_month_day _date) : date(_date){}
    Vrata(Vrata_Type _type, date::year_month_day _date)
        : type(_type),
          date(_date){}
    Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran)
        : type(_type),
          date(_date),
          paran(_paran){}
};

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);
std::ostream &operator<<(std::ostream &o, Vrata const &v);

#endif // VRATA_H

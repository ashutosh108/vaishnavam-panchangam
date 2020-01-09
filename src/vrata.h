#ifndef VRATA_H
#define VRATA_H

#include "date-fixed.h"
#include "paran.h"
#include "juldays_ut.h"
#include "tithi.h"

#include <optional>
#include <ostream>

namespace vp {

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

} // namespace vp

#endif // VRATA_H

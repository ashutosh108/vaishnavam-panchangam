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
    With_Atirikta_Dvadashi,
    Sandigdha_With_Atirikta_Dvadashi,
    Atirikta_Ekadashi,
    Sandigdha_Atirikta_Ekadashi
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
    bool is_two_days() const;
};

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);
std::ostream &operator<<(std::ostream &o, Vrata const &v);

// ref to static vector of all known ekAdashI names in English, capitalized. E.g. "Varūthinī"
const std::vector<std::string> & ekadashi_names();
const std::vector<std::string> & ekadashi_names_rus();

} // namespace vp

#endif // VRATA_H

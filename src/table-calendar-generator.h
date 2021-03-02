#ifndef TABLE_CALENDAR_GENERATOR_H
#define TABLE_CALENDAR_GENERATOR_H

#include "calc-error.h"
#include "table.h"
#include "vrata.h"

#include <ostream>
#include <unordered_map>

namespace vp {

using Custom_Dates = std::unordered_map<date::local_days, std::string>;

class Table_Calendar_Generator
{
public:
    static vp::Table generate(const VratasForDate & vratas, date::year default_year=date::year::min(), const Custom_Dates & custom_dates={});
};

} // namespace vp

#endif // TABLE_CALENDAR_GENERATOR_H

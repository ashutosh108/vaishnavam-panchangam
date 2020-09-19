#ifndef TABLE_CALENDAR_GENERATOR_H
#define TABLE_CALENDAR_GENERATOR_H

#include "calc-error.h"
#include "table.h"
#include "vrata.h"

#include <ostream>

namespace vp {

class Table_Calendar_Generator
{
public:
    static vp::Table generate(const VratasForDate & vratas);
};

} // namespace vp

#endif // TABLE_CALENDAR_GENERATOR_H

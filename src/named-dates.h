#ifndef NAMEDDATES_H
#define NAMEDDATES_H

#include "date-fixed.h"
#include <map>
#include <string>

namespace vp{
struct NamedDate {
    std::string name;
};

using NamedDates = std::multimap<date::local_days, NamedDate>;
}

#endif // NAMEDDATES_H

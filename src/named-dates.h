#ifndef NAMEDDATES_H
#define NAMEDDATES_H

#include "date-fixed.h"
#include <map>
#include <string>

namespace vp{
struct NamedDate {
    std::string name;
    std::string title;
    std::string css_classes;
    NamedDate(std::string name_, std::string title_="", std::string css_classes_="") :
          name(name_), title(title_), css_classes(css_classes_) {}
};

using NamedDates = std::multimap<date::local_days, NamedDate>;
}

#endif // NAMEDDATES_H

#include "date.h"

#include <iomanip>
#include <ostream>

std::ostream &operator <<(std::ostream &o, const Date &d)
{
    o << d.year;
    o << '-' << std::setw(2) << std::setfill('0') << d.month;
    o << '-' << std::setw(2) << std::setfill('0') << d.day;
    return o;
}

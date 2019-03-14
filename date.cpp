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

bool operator ==(const Date &d1, const Date &d2)
{
    return d1.year == d2.year && d1.month == d2.month && d1.day == d2.day;
}

bool operator !=(const Date &d1, const Date &d2)
{
    return !(d1 == d2);
}

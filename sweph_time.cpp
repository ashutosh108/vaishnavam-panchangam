#include "sweph_time.h"
#include <swephexp.h>

Sweph_Time::Sweph_Time(double jd) : jd_(jd)
{

}

std::ostream &operator<<(std::ostream &os, Sweph_Time const &t) {
    int year, month, day;
    double hours_double;
    swe_revjul(t.jd_, SE_GREG_CAL, &year, &month, &day, &hours_double);
    int hours = static_cast<int>(hours_double);
    double minutes_remain = (hours_double - hours) * 60;
    int minutes = static_cast<int>(minutes_remain);
    double seconds = (minutes_remain - minutes) * 60;

    os.width(4);
    os.fill('0');
    os << year << '-';
    os.width(2);
    os << month << '-' << day << ' ';
    os.width(2);
    os << hours << ':' << minutes << ':' << seconds;
    os.width(1);
    return os;
}

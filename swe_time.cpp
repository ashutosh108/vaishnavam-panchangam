#include <iomanip>

#include "swe_time.h"
#include <swephexp.h>

Swe_Time::Swe_Time(double jd) : jd_(jd)
{

}

Swe_Time::Swe_Time(int year, int month, int day) {
    jd_ = swe_julday(year, month, day, 0, SE_GREG_CAL);
}

std::ostream &operator<<(std::ostream &os, Swe_Time const &t) {
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
    os << hours << ':';
    os.width(2);
    os << minutes << ':' << std::fixed << std::setprecision(6) << std::setw(9) << seconds << " UTC";
    return os;
}

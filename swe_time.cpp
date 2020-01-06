#include <cmath>
#include <iomanip>

#include "tz-fixed.h"

#include "swe_time.h"
#include <swephexp.h>

Swe_Time::Swe_Time(double_days jd) : jd_(jd)
{
}

Swe_Time::Swe_Time(date::year year, date::month month, date::day day, double_hours hours)
{
    jd_ = double_days{swe_julday(static_cast<int>(year),
                                 static_cast<int>(static_cast<unsigned>(month)),
                                 static_cast<int>(static_cast<unsigned>(day)),
                                 hours.count(),
                                 SE_GREG_CAL)};
}

Swe_Time::Swe_Time(date::year year, date::month month, date::day day, int hours, int minutes, double seconds)
    : Swe_Time(year, month, day, double_hours{hours+minutes/60.0+seconds/3600.0}) {
}

bool Swe_Time::operator==(const Swe_Time &to) const
{
    const double epsilon = 1e-6;
    return std::fabs(jd_.count() - to.jd_.count()) <= epsilon;
}

date::year_month_day Swe_Time::as_date() const
{
    int year, month, day;
    double hours;
    swe_revjul(jd_.count(), SE_GREG_CAL, &year, &month, &day, &hours);
    return date::year{year}/month/day;
}

double_hours Swe_Time::hours() const
{
    int year, month, day;
    double hours;
    swe_revjul(jd_.count(), SE_GREG_CAL, &year, &month, &day, &hours);
    return double_hours{hours};
}

std::ostream &operator<<(std::ostream &os, Swe_Time const &t) {
    os << t.as_date() << ' ';
    os << date::hh_mm_ss(t.hours()) << " UTC";
    return os;
}

std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t) {
    date::sys_days utc_days{t.t.as_date()};
    auto utc = utc_days + t.t.hours();
    auto timezone = date::locate_zone(t.timezone_name);
    auto z = date::make_zoned(timezone, utc);
    return os << z;
}

Swe_Time operator +(const Swe_Time &t, double_days delta)
{
    return Swe_Time{t.as_julian_days() + delta};
}

Swe_Time operator -(const Swe_Time &t, double_days delta)
{
    return t + (-delta);
}

Swe_Time Swe_Time::operator +=(double_days delta)
{
    *this = *this+delta;
    return *this;
}

Swe_Time Swe_Time::operator -=(double_days delta)
{
    *this = *this-delta;
    return *this;
}

bool Swe_Time::operator <(const Swe_Time &other) const
{
    return as_julian_days() < other.as_julian_days();
}

bool Swe_Time::operator >(const Swe_Time &other) const
{
    return as_julian_days() > other.as_julian_days();
}

double_days operator -(const Swe_Time &t1, const Swe_Time &t2)
{
    return t1.as_julian_days() - t2.as_julian_days();
}

#include "juldays_ut.h"

#include <cmath>
#include <iomanip>
#include "swephexp.h"
#include "tz-fixed.h"

namespace vp {

JulDays_UT::JulDays_UT(double_days juldays_ut) : juldays_ut_(juldays_ut)
{
}

JulDays_UT::JulDays_UT(date::year_month_day d, double_hours hours)
{
    juldays_ut_ = double_days{swe_julday(static_cast<int>(d.year()),
                                 static_cast<int>(static_cast<unsigned>(d.month())),
                                 static_cast<int>(static_cast<unsigned>(d.day())),
                                 hours.count(),
                                 SE_GREG_CAL)};
}

bool JulDays_UT::operator==(const JulDays_UT &to) const
{
    const double epsilon = 1e-6;
    return std::fabs(juldays_ut_.count() - to.juldays_ut_.count()) <= epsilon;
}

date::year_month_day JulDays_UT::as_date() const
{
    int year, month, day;
    double hours;
    swe_revjul(juldays_ut_.count(), SE_GREG_CAL, &year, &month, &day, &hours);
    return date::year{year}/month/day;
}

double_hours JulDays_UT::hours() const
{
    int year, month, day;
    double hours;
    swe_revjul(juldays_ut_.count(), SE_GREG_CAL, &year, &month, &day, &hours);
    return double_hours{hours};
}

std::ostream &operator<<(std::ostream &os, JulDays_UT const &t) {
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

JulDays_UT operator +(const JulDays_UT &t, double_days delta)
{
    return JulDays_UT{t.raw_julian_days_ut() + delta};
}

JulDays_UT operator -(const JulDays_UT &t, double_days delta)
{
    return t + (-delta);
}

JulDays_UT JulDays_UT::operator +=(double_days delta)
{
    *this = *this+delta;
    return *this;
}

JulDays_UT JulDays_UT::operator -=(double_days delta)
{
    *this = *this-delta;
    return *this;
}

bool JulDays_UT::operator <(const JulDays_UT &other) const
{
    return raw_julian_days_ut() < other.raw_julian_days_ut();
}

bool JulDays_UT::operator >(const JulDays_UT &other) const
{
    return raw_julian_days_ut() > other.raw_julian_days_ut();
}

double_days operator -(const JulDays_UT &t1, const JulDays_UT &t2)
{
    return t1.raw_julian_days_ut() - t2.raw_julian_days_ut();
}

} // namespace vp

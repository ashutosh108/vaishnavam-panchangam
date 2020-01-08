#ifndef SWE_TIME_H
#define SWE_TIME_H

#include "date-fixed.h"
#include "tz-fixed.h"

#include <ostream>

namespace vp {

using double_hours = std::chrono::duration<double, std::ratio<3600>>;
using double_days = std::chrono::duration<double, std::ratio<86400>>;
using double_utc = date::utc_time<double_hours>;

class Swe_Zoned_Time;

class Swe_Time
{
public:
    explicit Swe_Time(double_days jd);
    explicit Swe_Time(date::year year, date::month month, date::day day, double_hours hours=double_hours{});
    explicit Swe_Time(date::year year, date::month month, date::day day, int hours, int minutes, double seconds);
    explicit Swe_Time(date::year_month_day d) : Swe_Time(d.year(), d.month(), d.day()) {}

    double_days as_julian_days() const { return jd_; }
    bool operator==(Swe_Time const &to) const;
    date::year_month_day as_date() const;
    double_hours hours() const;
    Swe_Time operator +=(double_days);
    Swe_Time operator -=(double_days);
    bool operator <(Swe_Time const &other) const;
    bool operator >(Swe_Time const &other) const;
private:
    double_days jd_;
    friend std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
    friend std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t);
};

class Swe_Zoned_Time {
public:
    Swe_Time t;
    const char *timezone_name;
    Swe_Zoned_Time(const char *_timezone_name, date::year_month_day d, double_hours hours=double_hours{}) :
        t(d.year(), d.month(), d.day(), hours),
        timezone_name(_timezone_name){}
    Swe_Zoned_Time(const char *_timezone_name, Swe_Time _t) :
        t(_t), timezone_name(_timezone_name) {}
};

std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t);
Swe_Time operator +(const Swe_Time &, double_days);
Swe_Time operator -(const Swe_Time &, double_days);

// returns number of days between two Swe_Time-s (like as_julian_days())
double_days operator -(const Swe_Time &, const Swe_Time &);

} // namespace vp

#endif // SWE_TIME_H

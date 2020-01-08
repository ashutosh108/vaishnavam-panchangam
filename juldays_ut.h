#ifndef JulDays_UT_H_INCLUDED
#define JulDays_UT_H_INCLUDED

#include "date-fixed.h"
#include "tz-fixed.h"

#include <ostream>

namespace vp {

using double_hours = std::chrono::duration<double, std::ratio<3600>>;
using double_days = std::chrono::duration<double, std::ratio<86400>>;
using double_utc = date::utc_time<double_hours>;

class Swe_Zoned_Time;

/* Wrapper class for jd (julian days) as it is understood in the swiss ephemeris library.
 * Very compact class: only one double in size, no virtual functions.
 * Provides value semantics, though maybe not all operations implemented for the lack of necessity.
 * NOTE: Julian days is *not* related to Julian calendar. It's named after monk Julius
 * and it is number of days since noon UT Jan 1, 4713BC.
 * See https://en.wikipedia.org/wiki/Julian_day
*/
class JulDays_UT
{
public:
    explicit JulDays_UT(double_days juldays_ut);
    explicit JulDays_UT(date::year year, date::month month, date::day day, double_hours hours=double_hours{});
    explicit JulDays_UT(date::year year, date::month month, date::day day, int hours, int minutes, double seconds);
    explicit JulDays_UT(date::year_month_day d) : JulDays_UT(d.year(), d.month(), d.day()) {}

    double_days raw_julian_days_ut() const { return juldays_ut_; }
    bool operator==(JulDays_UT const &to) const;
    date::year_month_day as_date() const;
    double_hours hours() const;
    JulDays_UT operator +=(double_days);
    JulDays_UT operator -=(double_days);
    bool operator <(JulDays_UT const &other) const;
    bool operator >(JulDays_UT const &other) const;
private:
    double_days juldays_ut_;
    friend std::ostream &operator<<(std::ostream &os, JulDays_UT const &t);
    friend std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t);
};

class Swe_Zoned_Time {
public:
    JulDays_UT t;
    const char *timezone_name;
    Swe_Zoned_Time(const char *_timezone_name, date::year_month_day d, double_hours hours=double_hours{}) :
        t(d.year(), d.month(), d.day(), hours),
        timezone_name(_timezone_name){}
    Swe_Zoned_Time(const char *_timezone_name, JulDays_UT _t) :
        t(_t), timezone_name(_timezone_name) {}
};

std::ostream &operator<<(std::ostream &os, JulDays_UT const &t);
std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t);
JulDays_UT operator +(const JulDays_UT &, double_days);
JulDays_UT operator -(const JulDays_UT &, double_days);

// returns number of days between two Swe_Time-s (like as_julian_days())
double_days operator -(const JulDays_UT &, const JulDays_UT &);

} // namespace vp

#endif // JulDays_UT_H_INCLUDED

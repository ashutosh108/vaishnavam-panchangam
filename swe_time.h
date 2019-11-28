#ifndef SWE_TIME_H
#define SWE_TIME_H

#include <ostream>

#include "date/date.h"

class Swe_Time
{
public:
    explicit Swe_Time(double jd);
    explicit Swe_Time(date::year year, date::month month, date::day day, double hours=0.0);
    explicit Swe_Time(date::year year, date::month month, date::day day, int hours, int minutes, double seconds);
    explicit Swe_Time(date::year_month_day d) : Swe_Time(d.year(), d.month(), d.day()) {}

    double as_julian_days() const { return jd_; }
    date::year year() const;
    date::month month() const;
    date::day day() const;
    double hours() const;
    bool operator==(Swe_Time const &to) const;
    date::year_month_day as_date() const;
    Swe_Time operator +=(double);
    Swe_Time operator -=(double);
    bool operator <(Swe_Time const &other) const;
    bool operator >(Swe_Time const &other) const;
private:
    double jd_;
    date::year year_;
    date::month month_;
    date::day day_;
    double hours_;
    friend std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
};

class Swe_Zoned_Time {
public:
    Swe_Time t;
    const char *timezone_name;
    Swe_Zoned_Time(const char *_timezone_name, date::year_month_day d, double hours=0.0) :
        t(d.year(), d.month(), d.day(), hours),
        timezone_name(_timezone_name){}
    Swe_Zoned_Time(const char *_timezone_name, Swe_Time _t) :
        t(_t), timezone_name(_timezone_name) {}
};

std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t);
Swe_Time operator +(const Swe_Time &, double);
Swe_Time operator -(const Swe_Time &, double);

// returns number of days between two Swe_Time-s (like as_julian_days())
double operator -(const Swe_Time &, const Swe_Time &);

#endif // SWE_TIME_H

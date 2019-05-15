#ifndef SWE_TIME_H
#define SWE_TIME_H

#include <ostream>

#include "date.h"

class Swe_Time
{
public:
    explicit Swe_Time(double jd);
    explicit Swe_Time(int year, int month, int day, double hours=0.0);
    explicit Swe_Time(int year, int month, int day, int hours, int minutes, double seconds);
    explicit Swe_Time(Date d) : Swe_Time(d.year, d.month, d.day) {}

    double as_julian_days() const { return jd_; }
    int year();
    int month();
    int day();
    double hours();
    bool operator==(Swe_Time const &to) const;
    Date as_date();
    Swe_Time operator +=(double);
    Swe_Time operator -=(double);
    bool operator <(Swe_Time const &other) const;
    bool operator >(Swe_Time const &other) const;
private:
    double jd_;
    int year_, month_, day_;
    double hours_;
    friend std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
};

std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
Swe_Time operator +(const Swe_Time &, double);
Swe_Time operator -(const Swe_Time &, double);

// returns number of days between two Swe_Time-s (like as_julian_days())
double operator -(const Swe_Time &, const Swe_Time &);

#endif // SWE_TIME_H

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
private:
    double jd_;
    int year_, month_, day_;
    double hours_;
    friend std::ostream &operator<<(std::ostream &os, Swe_Time const &t);
};

std::ostream &operator<<(std::ostream &os, Swe_Time const &t);

#endif // SWE_TIME_H

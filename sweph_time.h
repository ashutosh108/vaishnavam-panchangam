#ifndef SWEPH_TIME_H
#define SWEPH_TIME_H

#include <ostream>


class Sweph_Time
{
public:
    explicit Sweph_Time(double jd);
    explicit Sweph_Time(int year, int month, int day);
    double as_julian_days() const { return jd_; }
private:
    double jd_;
    friend std::ostream &operator<<(std::ostream &os, Sweph_Time const &t);
};

std::ostream &operator<<(std::ostream &os, Sweph_Time const &t);

#endif // SWEPH_TIME_H

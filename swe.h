#ifndef SWE_H
#define SWE_H

#include "swe_time.h"


class Swe
{
public:
    Swe();
    Swe_Time get_sunrise(Swe_Time after, double latitude, double longitude);
    double get_sun_longitude(Swe_Time time);
    double get_moon_longitude(Swe_Time time);
    double get_tithi(Swe_Time time);
private:
    void do_calc_ut(double jd, int planet, int flags, double *res);
};

#endif // SWE_H

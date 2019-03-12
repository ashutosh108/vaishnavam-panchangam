#ifndef SWE_H
#define SWE_H

#include <optional>

#include "swe_time.h"


class Swe
{
public:
    std::optional<Swe_Time> get_sunrise(Swe_Time after, double latitude, double longitude);
    double get_sun_longitude(Swe_Time time);
    double get_moon_longitude(Swe_Time time);
    /** Get tithi as double [0..30) */
    double get_tithi(Swe_Time time);
    Swe_Time find_tithi_start(Swe_Time after, double tithi);
private:
    void check_flags(int out_flags, int in_flags, char *serr);
    void do_calc_ut(double jd, int planet, int flags, double *res);
};

#endif // SWE_H

#ifndef SWE_H
#define SWE_H

#include <optional>

#include "swe_time.h"
#include "tithi.h"


class Swe
{
public:
    std::optional<Swe_Time> get_sunrise(Swe_Time after, double latitude, double longitude);
    std::optional<Swe_Time> get_sunset(Swe_Time after, double latitude, double longitude);
    double get_sun_longitude(Swe_Time time);
    double get_moon_longitude(Swe_Time time);
    /** Get tithi as double [0..30) */
    Tithi get_tithi(Swe_Time time);
    Swe_Time find_tithi_start(Swe_Time after, double tithi);
private:
    [[noreturn]] void throw_on_wrong_flags(int out_flags, int in_flags, char *serr);
    void do_calc_ut(double jd, int planet, int flags, double *res);
    std::optional<Swe_Time> do_rise_trans(int rise_or_set, Swe_Time after, double latitude, double longitude);
};

#endif // SWE_H

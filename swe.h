#ifndef SWE_H
#define SWE_H

#include <optional>

#include "coord.h"
#include "swe_time.h"
#include "tithi.h"


class Swe
{
public:
    Coord coord;

    Swe(Coord coord_);
    ~Swe();
    // Swe is kind of hanlde for sweph and thus we can't really copy it.
    // Copying it would allow for muiltiple swe_close() calls.
    Swe(const Swe &) = delete;
    Swe& operator=(const Swe &) = delete;
    std::optional<Swe_Time> get_sunrise(Swe_Time after) const;
    std::optional<Swe_Time> get_sunset(Swe_Time after) const;
    double get_sun_longitude(Swe_Time time) const;
    double get_moon_longitude(Swe_Time time) const;
    /** Get tithi as double [0..30) */
    Tithi get_tithi(Swe_Time time) const;
//    Swe_Time find_tithi_start(Swe_Time after, double tithi);
private:
    [[noreturn]] void throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const;
    void do_calc_ut(double jd, int planet, int flags, double *res) const;
    std::optional<Swe_Time> do_rise_trans(int rise_or_set, Swe_Time after) const;
};

#endif // SWE_H

#ifndef SWE_H
#define SWE_H

#include "calc-error.h"
#include "juldays_ut.h"
#include "location.h"
#include "tithi.h"

#include <cstdint> // for int32_t
#include <tl/expected.hpp>

namespace vp {

struct Longitude_sidereal {
    double longitude;
};

class Swe
{
public:
    // bitmask
    enum class Flag {
        SunriseByDiscCenter = 0,    // both sunrise and sunset
        SunriseByDiscEdge = 1,      // both sunrise and sunset
        SunriseByDiscMask = 1,
        RefractionOff = 0,
        RefractionOn = 2,
        RefractionMask = 2,
        Default = SunriseByDiscCenter | RefractionOff,
    };
    Location location{};

    Swe(Location coord_, Flag flags=Flag::Default);
    ~Swe();
    // Swe is kind of hanlde for sweph and thus we can't really copy it.
    // Copying it would allow for muiltiple swe_close() calls.
    Swe(const Swe &) = delete;
    Swe& operator=(const Swe &) = delete;
    Swe(Swe &&) noexcept;
    Swe& operator=(Swe &&) noexcept;
    tl::expected<JulDays_UT, CalcError> find_sunrise(JulDays_UT after) const;
    JulDays_UT find_sunrise_v(JulDays_UT after) const;
    tl::expected<JulDays_UT, CalcError> find_sunset(JulDays_UT after) const;
    JulDays_UT find_sunset_v(JulDays_UT after) const;
    double get_sun_longitude(JulDays_UT time) const;
    double get_moon_longitude(JulDays_UT time) const;
    /** Get tithi as double [0..30) */
    Tithi get_tithi(JulDays_UT time) const;
    Longitude_sidereal get_moon_longitude_sidereal(JulDays_UT time) const;
private:
    bool need_to_close = true;
    int32_t rise_flags;
    [[noreturn]] void throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const;
    void do_calc_ut(double jd, int planet, int flags, double *res) const;
    tl::expected<JulDays_UT, CalcError> do_rise_trans(int rise_or_set, JulDays_UT after) const;
    int32_t get_rise_flags(Flag flags);
};

Swe::Flag operator&(Swe::Flag lhs, Swe::Flag rhs);

} // namespace vp

#endif // SWE_H

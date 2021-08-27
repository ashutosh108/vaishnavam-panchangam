#ifndef SWE_H
#define SWE_H

#include "calc-error.h"
#include "calc-flags.h"
#include "juldays_ut.h"
#include "location.h"
#include "nakshatra.h"
#include "tithi.h"

#include <cstdint> // for int32_t
#include <tl/expected.hpp>

namespace vp {

class Swe
{
public:
    Location location{};
    CalcFlags calc_flags = CalcFlags::Invalid;

    Swe(const Location & coord_, CalcFlags flags=CalcFlags::Default);
    ~Swe();
    // Swe is kind of hanlde for sweph and thus we can't really copy it.
    // Copying it would allow for muiltiple swe_close() calls.
    Swe(const Swe &) = delete;
    Swe& operator=(const Swe &) = delete;
    Swe(Swe &&) noexcept;
    // We shouldn't need move-assignment for this class (can't see any well-defined use case).
    // Swe& operator=(Swe &&) noexcept;
    tl::expected<JulDays_UT, CalcError> find_sunrise(JulDays_UT after) const;
    JulDays_UT find_sunrise_v(JulDays_UT after) const;
    tl::expected<JulDays_UT, CalcError> find_sunset(JulDays_UT after) const;
    JulDays_UT find_sunset_v(JulDays_UT after) const;
    double get_sun_longitude(JulDays_UT time) const;
    double get_moon_longitude(JulDays_UT time) const;
    /** Get tithi as double [0..30) */
    Tithi get_tithi(JulDays_UT time) const;
    Nirayana_Longitude get_moon_longitude_sidereal(JulDays_UT time) const;
    Nakshatra get_nakshatra(JulDays_UT time) const;
    Nirayana_Longitude surya_nirayana_longitude(JulDays_UT time) const;
    // find next astronomical midnight (mid-point in time between sunset and sunrise)
    tl::expected<JulDays_UT, CalcError> first_midnight_after(vp::JulDays_UT after);
private:
    // remember to update move-contructor and and move-assigment when adding/changing fields
    bool need_to_close = true;
    int32_t rise_flags;
    int32_t ephemeris_flags;
    [[noreturn]] void throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const;
    void do_calc_ut(double jd, int planet, int flags, double *res) const;
    tl::expected<JulDays_UT, CalcError> do_rise_trans(int rise_or_set, JulDays_UT after) const;
    int32_t get_rise_flags(CalcFlags flags) const noexcept;
    int32_t calc_ephemeris_flags(CalcFlags flags) const noexcept;
};

} // namespace vp

#endif // SWE_H

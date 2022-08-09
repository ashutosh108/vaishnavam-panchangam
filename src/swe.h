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
    tl::expected<JulDays_UT, CalcError> next_sunrise(JulDays_UT after) const;
    JulDays_UT next_sunrise_v(JulDays_UT after) const;
    tl::expected<JulDays_UT, CalcError> next_sunset(JulDays_UT after) const;
    JulDays_UT next_sunset_v(JulDays_UT after) const;
    double sun_longitude(JulDays_UT time) const;
    double moon_longitude(JulDays_UT time) const;
    /** Get tithi as double [0..30) */
    Tithi tithi(JulDays_UT time) const;
    Nirayana_Longitude moon_longitude_sidereal(JulDays_UT time) const;
    Nakshatra nakshatra(JulDays_UT time) const;
    Nirayana_Longitude surya_nirayana_longitude(JulDays_UT time) const;

    static constexpr double_hours max_interval_between_sunrises{27.0};
private:
    // remember to update move-contructor and and move-assigment when adding/changing fields
    bool need_to_close = true;
    int32_t rise_flags;
    int32_t ephemeris_flags;
    tl::expected<JulDays_UT, CalcError> do_rise_trans(int rise_or_set, JulDays_UT after) const;
    int32_t get_rise_flags(CalcFlags flags) const noexcept;
    int32_t calc_ephemeris_flags(CalcFlags flags) const noexcept;
};

} // namespace vp

#endif // SWE_H

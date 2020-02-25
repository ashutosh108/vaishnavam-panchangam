#ifndef SWE_H
#define SWE_H

#include <optional>

#include "location.h"
#include "juldays_ut.h"
#include "tithi.h"

namespace vp {

class Swe
{
public:
    Location coord{};

    Swe(Location coord_);
    ~Swe();
    // Swe is kind of hanlde for sweph and thus we can't really copy it.
    // Copying it would allow for muiltiple swe_close() calls.
    Swe(const Swe &) = delete;
    Swe& operator=(const Swe &) = delete;
    Swe(Swe &&) noexcept;
    Swe& operator=(Swe &&) noexcept;
    std::optional<JulDays_UT> find_sunrise(JulDays_UT after) const;
    JulDays_UT find_sunrise_v(JulDays_UT after) const;
    std::optional<JulDays_UT> find_sunset(JulDays_UT after) const;
    JulDays_UT find_sunset_v(JulDays_UT after) const;
    double get_sun_longitude(JulDays_UT time) const;
    double get_moon_longitude(JulDays_UT time) const;
    /** Get tithi as double [0..30) */
    Tithi get_tithi(JulDays_UT time) const;
//    Swe_Time find_tithi_start(Swe_Time after, double tithi);
private:
    bool need_to_close = true;
    [[noreturn]] void throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const;
    void do_calc_ut(double jd, int planet, int flags, double *res) const;
    std::optional<JulDays_UT> do_rise_trans(int rise_or_set, JulDays_UT after) const;
};

} // namespace vp

#endif // SWE_H

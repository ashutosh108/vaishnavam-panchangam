#include "swe.h"

#include "concat.h"
#include "location.h"

#include <exception>
#include <sstream>
#include "swephexp.h"

namespace vp {

namespace detail {
constexpr int32 ephemeris_flags = SEFLG_SWIEPH;

constexpr double atmospheric_pressure = 1013.25;
constexpr double atmospheric_temperature = 15;
}

tl::expected<JulDays_UT, CalcError> Swe::do_rise_trans(int rise_or_set, JulDays_UT after) const {
    int32 rsmi = rise_or_set | rise_flags;
    double geopos[3] = {location.longitude, location.latitude, 0};
    double trise;
    char serr[AS_MAXCH];
    int32 flags = detail::ephemeris_flags;
    int res_flag = swe_rise_trans(after.raw_julian_days_ut().count(),
                                  SE_SUN,
                                  nullptr,
                                  flags,
                                  rsmi,
                                  geopos,
                                  detail::atmospheric_pressure,
                                  detail::atmospheric_temperature,
                                  &trise, serr);
    if (res_flag == -1) {
        throw_on_wrong_flags(-1, flags, serr);
    }

    if (res_flag == -2) {
        if (rise_or_set == SE_CALC_SET) {
            return tl::unexpected{CantFindSunsetAfter{after}};
        }
        return tl::unexpected{CantFindSunriseAfter{after}};
    } else {
        return JulDays_UT{double_days{trise}};
    }
}

// Get proper Sweph rise/set flags from intialization Swe flags.
// Returned value is to be or-red with SE_CALC_RISE or SE_CALC_SET.
// Possible flags: SE_BIT_DISC_CENTER, SE_BIT_NO_REFRACTION, SE_BIT_GEOCTR_NO_ECL_LAT
int32_t Swe::get_rise_flags(Flag flags) {
    int32_t res = SE_BIT_NO_REFRACTION;
    if ((flags & Flag::SunriseByDiscMask) == Flag::SunriseByDiscCenter) {
        res |= SE_BIT_DISC_CENTER;
    }
    return res;
}

Swe::Swe(Location coord_, Flag flags):location(coord_)
{
    rise_flags = get_rise_flags(flags);

    // have to use (non-const) char array due to swe_set_ephe_path() strang signature: char * instead of const char *.
    char ephepath[] = "eph";
    swe_set_ephe_path(ephepath);
    swe_set_topo(location.longitude, location.latitude, 0);
}

Swe::~Swe()
{
    if (need_to_close) {
        swe_close();
    }
}

Swe::Swe(Swe && other) noexcept
{
    need_to_close = false;
    std::swap(need_to_close, other.need_to_close);
    std::swap(location, other.location);
    std::swap(rise_flags, other.rise_flags);
}

Swe &Swe::operator=(Swe && other) noexcept
{
    need_to_close = false;
    std::swap(need_to_close, other.need_to_close);
    std::swap(location, other.location);
    std::swap(rise_flags, other.rise_flags);
    return *this;
}

tl::expected<JulDays_UT, CalcError> Swe::find_sunrise(JulDays_UT after) const
{
    return do_rise_trans(SE_CALC_RISE, after);
}

JulDays_UT Swe::find_sunrise_v(JulDays_UT after) const
{
    auto sunrise_or_error = find_sunrise(after);
    if (!sunrise_or_error) {
        throw std::runtime_error(concat("can't find next sunrise after ", after));
    }
    return *sunrise_or_error;
}

tl::expected<JulDays_UT, CalcError> Swe::find_sunset(JulDays_UT after) const
{
    return do_rise_trans(SE_CALC_SET, after);
}

JulDays_UT Swe::find_sunset_v(JulDays_UT after) const
{
    auto sunset_or_error = find_sunset(after);
    if (!sunset_or_error) {
        throw std::runtime_error(concat("can't find sunset after ", after));
    }
    return *sunset_or_error;
}

[[noreturn]] void Swe::throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const {
    if (out_flags == ERR) {
        throw std::runtime_error(serr);
    } else {
        std::stringstream s;
        s << "return flags != input flags (" << out_flags << "!=" << in_flags << ")";
        throw std::runtime_error(s.str());
    }
}

void Swe::do_calc_ut(double jd, int planet, int flags, double *res) const {
    char serr[AS_MAXCH];
    int32 res_flags = swe_calc_ut(jd, planet, flags, res, serr);
    if (res_flags == flags) {
        return;
    }
    throw_on_wrong_flags(res_flags, flags, serr);
}

double Swe::get_sun_longitude(JulDays_UT time) const
{
    double res[6];
    do_calc_ut(time.raw_julian_days_ut().count(), SE_SUN, detail::ephemeris_flags, res);
    return res[0];
}

double Swe::get_moon_longitude(JulDays_UT time) const
{
    double res[6];
    do_calc_ut(time.raw_julian_days_ut().count(), SE_MOON, detail::ephemeris_flags, res);
    return res[0];
}

/** Get tithi as double [0..30) */
Tithi Swe::get_tithi(JulDays_UT time) const
{
    double sun = get_sun_longitude(time);
    double moon = get_moon_longitude(time);
    double diff = moon - sun;
    if (diff < 0) diff += 360.0;
    return Tithi{diff / (360.0/30)};
}

Swe::Flag operator&(Swe::Flag lhs, Swe::Flag rhs) {
    using underlying = std::underlying_type_t<Swe::Flag>;
    return static_cast<Swe::Flag>(
                static_cast<underlying>(lhs) &
                static_cast<underlying>(rhs));
}

} // namespace swe

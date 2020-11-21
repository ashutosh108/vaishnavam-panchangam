#include "swe.h"

#include "location.h"

#include <array>
#include <exception>
#include "swephexp.h"

namespace vp {

namespace detail {
constexpr int32 ayanamsha = SE_SIDM_LAHIRI;

constexpr double atmospheric_pressure = 1013.25;
constexpr double atmospheric_temperature = 15;
}

tl::expected<JulDays_UT, CalcError> Swe::do_rise_trans(int rise_or_set, JulDays_UT after) const {
    int32 rsmi = rise_or_set | rise_flags;
    std::array<double, 3> geopos{location.longitude.longitude, location.latitude.latitude, 0.0};
    double trise;
    std::array<char, AS_MAXCH> serr;
    int res_flag = swe_rise_trans(after.raw_julian_days_ut().count(),
                                  SE_SUN,
                                  nullptr,
                                  ephemeris_flags,
                                  rsmi,
                                  geopos.data(),
                                  detail::atmospheric_pressure,
                                  detail::atmospheric_temperature,
                                  &trise, serr.data());
    if (res_flag == -1) {
        throw_on_wrong_flags(-1, ephemeris_flags, serr.data());
    }

    if (res_flag == -2) {
        if (rise_or_set == SE_CALC_SET) {
            return tl::make_unexpected(CantFindSunsetAfter{after});
        }
        return tl::make_unexpected(CantFindSunriseAfter{after});
    } else {
        return JulDays_UT{double_days{trise}};
    }
}

// Get proper Sweph rise/set flags from intialization Swe flags.
// Returned value is to be or-red with SE_CALC_RISE or SE_CALC_SET.
// Possible flags: SE_BIT_DISC_CENTER, SE_BIT_NO_REFRACTION, SE_BIT_GEOCTR_NO_ECL_LAT
int32_t Swe::get_rise_flags(CalcFlags flags) const noexcept {
    int32_t res = 0;
    if ((flags & CalcFlags::RefractionMask) == CalcFlags::RefractionOff) {
        res |= SE_BIT_NO_REFRACTION;
    }
    if ((flags & CalcFlags::SunriseByDiscMask) == CalcFlags::SunriseByDiscCenter) {
        res |= SE_BIT_DISC_CENTER;
    }
    if ((flags & CalcFlags::RiseSetGeocentricMask) == CalcFlags::RiseSetGeocentricOn)  {
        res |= SE_BIT_GEOCTR_NO_ECL_LAT;
    }
    return res;
}

int32_t Swe::calc_ephemeris_flags(CalcFlags flags) const noexcept
{
    if ((flags & vp::CalcFlags::EphemerisMask) == vp::CalcFlags::EphemerisSwiss) {
        return SEFLG_SWIEPH;
    } else {
        return SEFLG_MOSEPH;
    }
}

Swe::Swe(Location coord_, CalcFlags flags):location(coord_)
{
    rise_flags = get_rise_flags(flags);
    ephemeris_flags = calc_ephemeris_flags(flags);

    // have to use (non-const) char array due to swe_set_ephe_path() strang signature: char * instead of const char *.
    char ephepath[] = "eph";
    swe_set_ephe_path(ephepath);
    swe_set_topo(location.longitude.longitude, location.latitude.latitude, 0);
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
    std::swap(ephemeris_flags, other.ephemeris_flags);
}

Swe &Swe::operator=(Swe && other) noexcept
{
    need_to_close = false;
    std::swap(need_to_close, other.need_to_close);
    std::swap(location, other.location);
    std::swap(rise_flags, other.rise_flags);
    std::swap(ephemeris_flags, other.ephemeris_flags);
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
        throw std::runtime_error(fmt::format("can't find next sunrise after {}", after));
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
        throw std::runtime_error(fmt::format("can't find sunset after ", after));
    }
    return *sunset_or_error;
}

namespace {
std::string se_flag_to_string(uint_fast32_t flag) {
    struct known_flag {
        uint_fast32_t flag;
        std::string name;
        std::string description;
    };

    static const std::array<known_flag, 20> known_flags{{
        {SEFLG_JPLEPH, "SEFLG_JPLEPH", "use JPL ephemeris"},    // 1
        {SEFLG_SWIEPH, "SEFLG_SWIEPH", "use SWISSEPH ephemeris"},   // 2
        {SEFLG_MOSEPH, "SEFLG_MOSEPH", "use Moshier ephemeris"}, //4
        {SEFLG_HELCTR, "SEFLG_HELCTR", "heliocentric position"}, //8
        {SEFLG_TRUEPOS, "SEFLG_TRUEPOS", "true/geometric position, not apparent position"}, //16
        {SEFLG_J2000, "SEFLG_J2000", "no precession, i.e. give J2000 equinox"}, //32
        {SEFLG_NONUT, "SEFLG_NONUT", "no nutation, i.e. mean equinox of date"}, //64
        {SEFLG_SPEED3, "SEFLG_SPEED3", "speed from 3 positions"}, //128
        {SEFLG_SPEED, "SEFLG_SPEED", "high precision speed"}, //256
        {SEFLG_NOGDEFL, "SEFLG_NOGDEFL", "turn off gravitational deflection"}, //512
        {SEFLG_NOABERR, "SEFLG_NOABERR", "turn off 'annual' aberration of light"}, //1024
        {SEFLG_EQUATORIAL, "SEFLG_EQUATORIAL", "equatorial positions are wanted"}, //(2*1024)
        {SEFLG_XYZ, "SEFLG_XYZ", "cartesian, not polar, coordinates"}, //(4*1024)
        {SEFLG_RADIANS, "SEFLG_RADIANS", "coordinates in radians, not degrees"}, //(8*1024)
        {SEFLG_BARYCTR, "SEFLG_BARYCTR", "barycentric position"}, //(16*1024)
        {SEFLG_TOPOCTR, "SEFLG_TOPOCTR", "topocentric position"}, //(32*1024)
        {SEFLG_SIDEREAL, "SEFLG_SIDEREAL", "sidereal position"}, //(64*1024)
        {SEFLG_ICRS, "SEFLG_ICRS", "ICRS (DE406 reference frame)"}, //(128*1024)
        {SEFLG_DPSIDEPS_1980, "SEFLG_DPSIDEPS_1980", "reproduce JPL Horizons 1962 - today to 0.002 arcsec."}, //(256*1024)
        {SEFLG_JPLHOR_APPROX, "SEFLG_JPLHOR_APPROX", "approximate JPL Horizons 1962 - today"}, //(512*1024)
    }};
    fmt::memory_buffer buf;
    fmt::format_to(buf, "{}\n", flag);
    bool first = true;
    for (const auto & cur_flag : known_flags) {
        if (cur_flag.flag & flag) {
            flag &= ~(cur_flag.flag);
            if (!first) fmt::format_to(buf, "\n| ");
            first = false;
            fmt::format_to(buf, "{:x} {} ({})", cur_flag.flag, cur_flag.name, cur_flag.description);
        }
    }
    if (flag != 0) {
        fmt::format_to(buf, "\n| {:x}", flag);

    }
    return fmt::to_string(buf);
}
}

[[noreturn]] void Swe::throw_on_wrong_flags(int out_flags, int in_flags, char *serr) const {
    if (out_flags == ERR) {
        throw std::runtime_error(serr);
    } else {
        throw std::runtime_error(fmt::format(
            "input flags != return flags ({}!={})\nin: {}\nout: {}", in_flags, out_flags, se_flag_to_string(in_flags), se_flag_to_string(out_flags)));
    }
}

void Swe::do_calc_ut(double jd, int planet, int flags, double *res) const {
    char serr[AS_MAXCH];
    int32 res_flags = swe_calc_ut(jd, planet, flags, res, serr);
    if (res_flags == flags) {
        return;
    }
    // with sidereal calculations, NONUT is added automatically, so ignore change in that bit
    if ((flags & SEFLG_SIDEREAL) && ((flags & ~SEFLG_NONUT) == (res_flags & ~SEFLG_NONUT))) {
        return;
    }

    throw_on_wrong_flags(res_flags, flags, serr);
}

double Swe::get_sun_longitude(JulDays_UT time) const
{
    double res[6];
    do_calc_ut(time.raw_julian_days_ut().count(), SE_SUN, ephemeris_flags, res);
    return res[0];
}

double Swe::get_moon_longitude(JulDays_UT time) const
{
    double res[6];
    do_calc_ut(time.raw_julian_days_ut().count(), SE_MOON, ephemeris_flags, res);
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

Nirayana_Longitude Swe::get_moon_longitude_sidereal(JulDays_UT time) const
{
//    throw std::runtime_error("get_moon_longitude_sidereal is not implemented yet");
    double res[6];
    swe_set_sid_mode(
        detail::ayanamsha,
        0/*t0, unused since predefined mode is given as first argument*/,
        0/*ayan_t0, unused since predefined mode is given as first argument*/);
    do_calc_ut(time.raw_julian_days_ut().count(), SE_MOON, ephemeris_flags | SEFLG_SIDEREAL, res);
    return Nirayana_Longitude{res[0]};
}

Nakshatra Swe::get_nakshatra(JulDays_UT time) const
{
    const auto moon_longitude_sidereal = get_moon_longitude_sidereal(time);
    return Nakshatra{moon_longitude_sidereal.longitude * (27.0/360.0)};
}

} // namespace swe

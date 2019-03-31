#include <exception>
#include <optional>
#include <sstream>

#include "swephexp.h"

#include "coord.h"
#include "swe.h"

std::optional<Swe_Time> Swe::do_rise_trans(int rise_or_set, Swe_Time after) const {
    int rsmi = rise_or_set | SE_BIT_HINDU_RISING;
         // or SE_CALC_RISE | SE_BIT_DISC_CENTER | SE_BIT_NO_REFRACTION | SE_BIT_GEOCTR_NO_ECL_LAT;
    double geopos[3] = {coord.longitude, coord.latitude, 0};
    const double atmospheric_pressure = 1013.25;
    const double atmospheric_temperature = 15;
    double trise;
    char serr[AS_MAXCH];
    int32 flags = SEFLG_MOSEPH;
    int res_flag = swe_rise_trans(after.as_julian_days(), SE_SUN, nullptr, flags, rsmi, geopos,
                                     atmospheric_pressure, atmospheric_temperature, &trise, serr);
    if (res_flag == -1) {
        throw_on_wrong_flags(-1, flags, serr);
    }

    if (res_flag == -2) {
        return{};
    } else {
        return Swe_Time{trise};
    }
}

Swe::Swe(Coord coord_):coord(coord_)
{
    swe_set_ephe_path(nullptr);
    swe_set_topo(coord.longitude, coord.latitude, 0);
}

Swe::~Swe()
{
    swe_close();
}

std::optional<Swe_Time> Swe::get_sunrise(Swe_Time after) const
{
    return do_rise_trans(SE_CALC_RISE, after);
}

std::optional<Swe_Time> Swe::get_sunset(Swe_Time after) const
{
    return do_rise_trans(SE_CALC_SET, after);
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
    };
    throw_on_wrong_flags(res_flags, flags, serr);
}

double Swe::get_sun_longitude(Swe_Time time) const
{
    double res[6];
    do_calc_ut(time.as_julian_days(), SE_SUN, SEFLG_MOSEPH, res);
    return res[0];
}

double Swe::get_moon_longitude(Swe_Time time) const
{
    double res[6];
    do_calc_ut(time.as_julian_days(), SE_MOON, SEFLG_MOSEPH, res);
    return res[0];
}

/** Get tithi as double [0..30) */
Tithi Swe::get_tithi(Swe_Time time) const
{
    double sun = get_sun_longitude(time);
    double moon = get_moon_longitude(time);
    double diff = moon - sun;
    if (diff < 0) diff += 360.0;
    return Tithi{diff / (360.0/30)};
}

// TODO: implement (this function seems to be nice to have for debugging
// but not essential for actual Ekadashi calculations
//Swe_Time Swe::find_tithi_start(Swe_Time after, double/* tithi*/)
//{
//    return after;
//}

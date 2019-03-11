#include <exception>
#include <sstream>

#include "swe.h"

#include "swephexp.h"

Swe::Swe()
{

}

Swe_Time Swe::get_sunrise(Swe_Time after, double latitude, double longitude)
{
    int rsmi = SE_CALC_RISE | SE_BIT_HINDU_RISING;
         // or SE_CALC_RISE | SE_BIT_DISC_CENTER | SE_BIT_NO_REFRACTION | SE_BIT_GEOCTR_NO_ECL_LAT;
    double geopos[3] = {longitude, latitude, 0};
    const double atmospheric_pressure = 1013.25;
    const double atmospheric_temperature = 15;
    double trise;
    char serr[AS_MAXCH];
    int return_code = swe_rise_trans(after.as_julian_days(), SE_SUN, nullptr, SEFLG_SWIEPH, rsmi, geopos,
                                     atmospheric_pressure, atmospheric_temperature, &trise, serr);
    return Swe_Time{trise};
}

void Swe::do_calc_ut(double jd, int planet, int flags, double *res) {
    char serr[AS_MAXCH];
    int32 res_flags = swe_calc_ut(jd, planet, flags, res, serr);
    if (res_flags != flags) {
        if (res_flags == ERR) {
            throw std::runtime_error(serr);
        } else {
            std::stringstream s;
            s << "return flags != inpur flags (" << res_flags << "!=" << flags << ")";
            throw std::runtime_error(s.str());
        }
    }
}

double Swe::get_sun_longitude(Swe_Time time)
{
    double res[6];
    do_calc_ut(time.as_julian_days(), SE_SUN, SEFLG_MOSEPH, res);
    return res[0];
}

double Swe::get_moon_longitude(Swe_Time time)
{
    double res[6];
    do_calc_ut(time.as_julian_days(), SE_MOON, SEFLG_MOSEPH, res);
    return res[0];
}

/** Get tithi as double [0..30) */
double Swe::get_tithi(Swe_Time time)
{
    double sun = get_sun_longitude(time);
    double moon = get_moon_longitude(time);
    double diff = moon - sun;
    if (diff < 0) diff += 360.0;
    return diff / (360.0/30);
}

// TODO: implement (this function seems to be nice to have for debugging
// but not essential for actual Ekadashi calculations
Swe_Time Swe::find_tithi_start(Swe_Time after, double/* tithi*/)
{
    return after;
}

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

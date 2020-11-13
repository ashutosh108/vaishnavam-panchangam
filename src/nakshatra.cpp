#include "nakshatra.h"

#include "fmt-format-fixed.h"

vp::Nakshatra::Nakshatra(double nakshatra_value) : nakshatra(nakshatra_value)
{
    if (nakshatra < 0.0 || nakshatra >= 27.0) {
        throw std::domain_error(fmt::format(FMT_STRING("nakshatra's value {} is outside of permitted range [0.0..28.0)."), nakshatra));
    }
}

vp::Nakshatra::Nakshatra(vp::Longitude_sidereal longitude) : Nakshatra(longitude.longitude * (27.0 / 360.0))
{
}

double vp::positive_delta_between_nakshatras(vp::Nakshatra n1, vp::Nakshatra n2)
{
    double delta = n2.nakshatra - n1.nakshatra;
    if (delta < 0) delta += 27.0;
    return delta;
}

double vp::minimal_delta_between_nakshatras(Nakshatra n1, Nakshatra n2)
{
    double delta = n2.nakshatra - n1.nakshatra;
    if (delta < -(27.0/2.0)) {
        delta += 27.0;
    } else if (delta > (27.0/2.0)) {
        delta -= 27.0;
    }
    return delta;
}

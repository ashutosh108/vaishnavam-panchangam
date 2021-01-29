#include "nakshatra.h"

#include "fmt-format-fixed.h"

vp::Nakshatra::Nakshatra(vp::Nirayana_Longitude longitude) : Nakshatra(longitude.longitude * (27.0 / 360.0))
{
}

vp::Nakshatra vp::Nakshatra::floor() const
{
    return vp::Nakshatra{std::floor(nakshatra)};
}

vp::Nakshatra vp::Nakshatra::ceil() const
{
    return vp::Nakshatra{normalize(std::ceil(nakshatra))};
}

double vp::Nakshatra::normalize(double raw_nakshatra)
{
    double mod_res = std::fmod(raw_nakshatra, 27.0);
    if (mod_res < 0) { mod_res += 27.0; }
    return mod_res;
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

bool vp::operator ==(const vp::Nakshatra n1, const vp::Nakshatra n2)
{
    // 2e-11 is chosen to give precision better than 0.5E-6 seconds:
    // ~1e5 seconds per average nakshatra, 1e6 millionths per second, *2 to be better than
    // half of millionth of digit.
    constexpr double epsilon = 0.5e-11;
    return ( fabs(n1.nakshatra - n2.nakshatra) < epsilon );
}

bool vp::operator !=(const vp::Nakshatra n1, const vp::Nakshatra n2) {
    return !(n1 == n2);
}

vp::Nakshatra & vp::operator ++(vp::Nakshatra & n)
{
    n.nakshatra = vp::Nakshatra::normalize(n.nakshatra + 1.0);
    return n;
}

vp::Nakshatra vp::operator +(vp::Nakshatra n, double delta)
{
    return vp::Nakshatra{vp::Nakshatra::normalize(n.nakshatra + delta)};
}

vp::Nakshatra vp::operator -(vp::Nakshatra n, double delta)
{
    return vp::Nakshatra{vp::Nakshatra::normalize(n.nakshatra - delta)};
}

double vp::positive_delta_between_longitudes(vp::Nirayana_Longitude l1, vp::Nirayana_Longitude l2) {
    double delta = l2.longitude - l1.longitude;
    if (delta < 0) delta += 360.0;
    return delta;
}

double vp::minimal_delta_between_longitudes(vp::Nirayana_Longitude l1, vp::Nirayana_Longitude l2)
{
    double delta = l2.longitude - l1.longitude;
    if (delta < -(360.0/2.0)) {
        delta += 360.0;
    } else if (delta > (360.0/2.0)) {
        delta -= 360.0;
    }
    return delta;
}

bool vp::operator ==(vp::Nirayana_Longitude l1, vp::Nirayana_Longitude l2)
{
    constexpr double epsilon = 0.5e-11;
    return ( fabs(l2.longitude - l1.longitude) < epsilon );
}

bool vp::operator !=(vp::Nirayana_Longitude l1, vp::Nirayana_Longitude l2) {
    return !(l1 == l2);
}

vp::DiscreteNakshatra::DiscreteNakshatra(vp::Nakshatra n)
{
    num = static_cast<int>(n.nakshatra);
}

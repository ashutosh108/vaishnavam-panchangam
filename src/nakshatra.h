#ifndef NAKSHATRA_H
#define NAKSHATRA_H

#include "swe.h"

namespace vp {

class Nakshatra
{
public:
    double nakshatra;
    Nakshatra(double nakshatra_value);
    Nakshatra(vp::Longitude_sidereal longitude);
};

double positive_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);
double minimal_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);

}

#endif // NAKSHATRA_H

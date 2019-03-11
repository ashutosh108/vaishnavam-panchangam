#ifndef SWE_H
#define SWE_H

#include "swe_time.h"


class Swe
{
public:
    Swe();
    Swe_Time get_sunrise(Swe_Time after, double latitude, double longitude);
};

#endif // SWE_H

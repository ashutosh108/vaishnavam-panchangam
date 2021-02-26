#ifndef CALC_FLAGS_H
#define CALC_FLAGS_H

#include <type_traits>

namespace vp {

// bitmask
enum class CalcFlags {
    SunriseByDiscCenter = 0,    // both sunrise and sunset
    SunriseByDiscEdge = 1,      // both sunrise and sunset
    SunriseByDiscMask = 1,
    RefractionOff = 0,
    RefractionOn = 2,
    RefractionMask = 2,
    EphemerisSwiss = 0,
    EphemerisMoshier = 4,
    EphemerisMask = 4,
    RiseSetGeocentricMask = 8,
    RiseSetGeocentricOff = 0, // default: do not set GEOCTR_NO_ECL_LAT
    RiseSetGeocentricOn = 8, // set GEOCTR_NO_ECL_LAT for rise-set-related calls
    ShravanaDvadashiMask = 16,      // Be default, Śravaṇa-nakṣatra must be present on Dvādaśī for 12+ ghaṭikas
    ShravanaDvadashi12ghPlus = 0,   // after sunrise for Śravaṇa-dvādaśī (i.e. enter Madhyāhna-kāla at least briefly)
    ShravanaDvadashi14ghPlus = 16,  // OR, require 14+ ghaṭikas (i.e. Śr-nakṣatra must not only enter Madhyahna-kāla, but also enter it's middle ghaṭika, 15th ghaṭika from suryodaya)
    Default = 0, // Default must be zero because we are ORing it with flags sometimes
    Invalid = -1,
};

CalcFlags operator&(CalcFlags lhs, CalcFlags rhs);
CalcFlags operator|(CalcFlags lhs, CalcFlags rhs);

}
#endif // CALC_FLAGS_H

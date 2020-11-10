#ifndef CALC_FLAGS_H
#define CALC_FLAGS_H


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
    Default = 0, // Default must be zero because we are ORing it with flags sometimes
};

CalcFlags operator&(CalcFlags lhs, CalcFlags rhs);
CalcFlags operator|(CalcFlags lhs, CalcFlags rhs);

}
#endif // CALC_FLAGS_H

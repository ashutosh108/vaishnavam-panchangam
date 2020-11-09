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
    Default = SunriseByDiscCenter | RefractionOff,
};

CalcFlags operator&(CalcFlags lhs, CalcFlags rhs);

}
#endif // CALC_FLAGS_H

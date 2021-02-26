#include "calc-flags.h"

#include <type_traits>

using underlying = std::underlying_type_t<vp::CalcFlags>;

vp::CalcFlags vp::operator&(vp::CalcFlags lhs, vp::CalcFlags rhs)
{
    return static_cast<vp::CalcFlags>(
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs));
}

vp::CalcFlags vp::operator|(vp::CalcFlags lhs, vp::CalcFlags rhs)
{
    return static_cast<vp::CalcFlags>(
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs));
}

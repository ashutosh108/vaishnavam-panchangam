#include "masa.h"

std::underlying_type_t<vp::Saura_Masa> vp::operator-(vp::Saura_Masa m1, vp::Saura_Masa m2)
{
    using T = std::underlying_type_t<vp::Saura_Masa>;
    std::make_signed_t<T> delta = static_cast<T>(m1) - static_cast<T>(m2);
    if (delta < 0) { delta += 12; }
    return delta;
}

vp::Saura_Masa vp::operator+(vp::Saura_Masa m, int delta)
{
    using T = std::underlying_type_t<vp::Saura_Masa>;
    int mod = (static_cast<T>(m) - 1 + delta) % 12;
    if (mod < 0) { mod += 12; }
    return vp::Saura_Masa{1 + mod};
}

vp::Saura_Masa_Point vp::operator+(vp::Saura_Masa m, double delta)
{
    using T = std::underlying_type_t<vp::Saura_Masa>;
    auto mod = std::fmod((static_cast<T>(m) - 1 + delta), 12);
    if (mod < 0) { mod += 12; }
    return vp::Saura_Masa_Point{mod};
}

vp::Saura_Masa vp::Saura_Masa_Point::floor() const
{
    return Saura_Masa{static_cast<int>(val+1)};
}

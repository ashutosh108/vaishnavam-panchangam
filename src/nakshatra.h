#ifndef NAKSHATRA_H
#define NAKSHATRA_H

#include <cmath>
#include "fmt-format-fixed.h"
#include <string>

namespace vp {

struct Nirayana_Longitude {
    double longitude;
};

class Nakshatra
{
public:
    double nakshatra;
    explicit constexpr Nakshatra(double nakshatra_value) : nakshatra(nakshatra_value) {
        if (nakshatra < 0.0 || nakshatra >= 27.0) {
            throw std::domain_error(fmt::format(FMT_STRING("nakshatra's value {} is outside of permitted range [0.0..28.0)."), nakshatra));
        }
    }
    explicit Nakshatra(vp::Nirayana_Longitude longitude);
    Nakshatra floor() const;
    Nakshatra ceil() const;
    static double normalize(double raw_nakshatra);

    static constexpr Nakshatra ASHVINI_START() { return Nakshatra{0.0}; }
    static constexpr Nakshatra ASHVINI_END() { return Nakshatra{1.0}; }
    static constexpr Nakshatra BHARANI_START() { return Nakshatra{1.0}; }
    static constexpr Nakshatra BHARANI_END() { return Nakshatra{2.0}; }
    static constexpr Nakshatra KRITIKA_START() { return Nakshatra{2.0}; }
    static constexpr Nakshatra KRITIKA_END() { return Nakshatra{3.0}; }
    static constexpr Nakshatra ROHINI_START() { return Nakshatra{3.0}; }
    static constexpr Nakshatra ROHINI_END() { return Nakshatra{4.0}; }
    static constexpr Nakshatra MRGASHIRSHA_START() { return Nakshatra{4.0}; }
    static constexpr Nakshatra MRGASHIRSHA_END() { return Nakshatra{5.0}; }
    static constexpr Nakshatra ARDRA_START() { return Nakshatra{5.0}; }
    static constexpr Nakshatra ARDRA_END() { return Nakshatra{6.0}; }
    static constexpr Nakshatra PUNARVASU_START() { return Nakshatra{6.0}; }
    static constexpr Nakshatra PUNARVASU_END() { return Nakshatra{7.0}; }
    static constexpr Nakshatra PUSHYA_START() { return Nakshatra{7.0}; }
    static constexpr Nakshatra PUSHYA_END() { return Nakshatra{8.0}; }
    static constexpr Nakshatra ASHLESHA_START() { return Nakshatra{8.0}; }
    static constexpr Nakshatra ASHLESHA_END() { return Nakshatra{9.0}; }
    static constexpr Nakshatra MAGHA_START() { return Nakshatra{9.0}; }
    static constexpr Nakshatra MAGHA_END() { return Nakshatra{10.0}; }
    static constexpr Nakshatra PURVA_PHALGUNI_START() { return Nakshatra{10.0}; }
    static constexpr Nakshatra PURVA_PHALGUNI_END() { return Nakshatra{11.0}; }
    static constexpr Nakshatra UTTARA_PHALGUNI_START() { return Nakshatra{11.0}; }
    static constexpr Nakshatra UTTARA_PHALGUNI_END() { return Nakshatra{12.0}; }
    static constexpr Nakshatra HASTA_START() { return Nakshatra{12.0}; }
    static constexpr Nakshatra HASTA_END() { return Nakshatra{13.0}; }
    static constexpr Nakshatra CHITRA_START() { return Nakshatra{13.0}; }
    static constexpr Nakshatra CHITRA_END() { return Nakshatra{14.0}; }
    static constexpr Nakshatra SVATI_START() { return Nakshatra{14.0}; }
    static constexpr Nakshatra SVATI_END() { return Nakshatra{15.0}; }
    static constexpr Nakshatra VISHAKHA_START() { return Nakshatra{15.0}; }
    static constexpr Nakshatra VISHAKHA_END() { return Nakshatra{16.0}; }
    static constexpr Nakshatra ARUNDHATI_START() { return Nakshatra{16.0}; }
    static constexpr Nakshatra ARUNDHATI_END() { return Nakshatra{17.0}; }
    static constexpr Nakshatra JYESHTHA_START() { return Nakshatra{17.0}; }
    static constexpr Nakshatra JYESHTHA_END() { return Nakshatra{18.0}; }
    static constexpr Nakshatra MULA_START() { return Nakshatra{18.0}; }
    static constexpr Nakshatra MULA_END() { return Nakshatra{19.0}; }
    static constexpr Nakshatra PURVA_ASHADHA_START() { return Nakshatra{19.0}; }
    static constexpr Nakshatra PURVA_ASHADHA_END() { return Nakshatra{20.0}; }
    static constexpr Nakshatra UTTARA_ASHADHA_START() { return Nakshatra{20.0}; }
    static constexpr Nakshatra UTTARA_ASHADHA_END() { return Nakshatra{21.0}; }
    static constexpr Nakshatra SHRAVANA_START() { return Nakshatra{21.0}; }
    static constexpr Nakshatra SHRAVANA_END() { return Nakshatra{22.0}; }
    static constexpr Nakshatra DHANISTHA_START() { return Nakshatra{22.0}; }
    static constexpr Nakshatra DHANISTHA_END() { return Nakshatra{23.0}; }
    static constexpr Nakshatra SHATABHISHA_START() { return Nakshatra{23.0}; }
    static constexpr Nakshatra SHATABHISHA_END() { return Nakshatra{24.0}; }
    static constexpr Nakshatra PURVA_BHADRAPADA_START() { return Nakshatra{24.0}; }
    static constexpr Nakshatra PURVA_BHADRAPADA_END() { return Nakshatra{25.0}; }
    static constexpr Nakshatra UTTARA_BHADRAPADA_START() { return Nakshatra{25.0}; }
    static constexpr Nakshatra UTTARA_BHADRAPADA_END() { return Nakshatra{26.0}; }
    static constexpr Nakshatra REVATI_START() { return Nakshatra{26.0}; }
    static constexpr Nakshatra REVATI_END() { return Nakshatra{0.0}; }
};



double positive_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);
double minimal_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);
bool operator == (const Nakshatra n1, const Nakshatra n2);
bool operator != (const Nakshatra n1, const Nakshatra n2);
vp::Nakshatra & operator ++(Nakshatra & n);
vp::Nakshatra operator +(Nakshatra n, double delta);

}

namespace  {
std::string nakshatra_name(int n) {
    static const char * const names[] {
        "Ashvini",
        "Bharani",
        "Kritika",
        "Rohini",
        "Mrgashirsha",
        "Ardra",
        "Punarvasu",
        "Pushya",
        "Ashlesha",
        "Magha",
        "Purva Phalguni",
        "Uttara Phalguni",
        "Hasta",
        "Chitra",
        "Svati",
        "Vishakha",
        "Arundhati",
        "Jyeshtha",
        "Mula",
        "Purva Ashadha",
        "Uttara Ashadha",
        "Shravana",
        "Dhanistha",
        "Shatabhisha",
        "Purva Bhadrapada",
        "Uttara Bhadrapada",
        "Revati"
    };
    if (n < 0 || n > 26) {
        return "Unknown";
    }
    return names[n];
}
}

template<>
struct fmt::formatter<vp::Nakshatra> {
    template<class ParseContext>
    auto parse(ParseContext & ctx) {
        return ctx.begin();
    }
    template<class FormatContext>
    auto format(const vp::Nakshatra n, FormatContext & ctx) {
        long raw_nakshatra_by_thousand = std::lround(n.nakshatra * 1000) % 27000;
        long nakshatra_num = raw_nakshatra_by_thousand / 1000; // 0...26
        long nakshatra_fract = raw_nakshatra_by_thousand % 1000;
        return fmt::format_to(ctx.out(), FMT_STRING("{}(.{:03})"), nakshatra_name(nakshatra_num), nakshatra_fract);
    }
};

#endif // NAKSHATRA_H

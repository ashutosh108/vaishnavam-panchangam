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
    explicit Nakshatra(double nakshatra_value);
    explicit Nakshatra(vp::Nirayana_Longitude longitude);
    Nakshatra floor() const;
    Nakshatra ceil() const;
    static double normalize(double raw_nakshatra);

    static constexpr double ASHVINI_START = 0.0;
    static constexpr double ASHVINI_END = 1.0;
    static constexpr double BHARANI_START = 1.0;
    static constexpr double BHARANI_END = 2.0;
    static constexpr double KRITIKA_START = 2.0;
    static constexpr double KRITIKA_END = 3.0;
    static constexpr double ROHINI_START = 3.0;
    static constexpr double ROHINI_END = 4.0;
    static constexpr double MRGASHIRSHA_START = 4.0;
    static constexpr double MRGASHIRSHA_END = 5.0;
    static constexpr double ARDRA_START = 5.0;
    static constexpr double ARDRA_END = 6.0;
    static constexpr double PUNARVASU_START = 6.0;
    static constexpr double PUNARVASU_END = 7.0;
    static constexpr double PUSHYA_START = 7.0;
    static constexpr double PUSHYA_END = 8.0;
    static constexpr double ASHLESHA_START = 8.0;
    static constexpr double ASHLESHA_END = 9.0;
    static constexpr double MAGHA_START = 9.0;
    static constexpr double MAGHA_END = 10.0;
    static constexpr double PURVA_PHALGUNI_START = 10.0;
    static constexpr double PURVA_PHALGUNI_END = 11.0;
    static constexpr double UTTARA_PHALGUNI_START = 11.0;
    static constexpr double UTTARA_PHALGUNI_END = 12.0;
    static constexpr double HASTA_START = 12.0;
    static constexpr double HASTA_END = 13.0;
    static constexpr double CHITRA_START = 13.0;
    static constexpr double CHITRA_END = 14.0;
    static constexpr double SVATI_START = 14.0;
    static constexpr double SVATI_END = 15.0;
    static constexpr double VISHAKHA_START = 15.0;
    static constexpr double VISHAKHA_END = 16.0;
    static constexpr double ARUNDHATI_START = 16.0;
    static constexpr double ARUNDHATI_END = 17.0;
    static constexpr double JYESHTHA_START = 17.0;
    static constexpr double JYESHTHA_END = 18.0;
    static constexpr double MULA_START = 18.0;
    static constexpr double MULA_END = 19.0;
    static constexpr double PURVA_ASHADHA_START = 19.0;
    static constexpr double PURVA_ASHADHA_END = 20.0;
    static constexpr double UTTARA_ASHADHA_START = 20.0;
    static constexpr double UTTARA_ASHADHA_END = 21.0;
    static constexpr double SHRAVANA_START = 21.0;
    static constexpr double SHRAVANA_END = 22.0;
    static constexpr double DHANISTHA_START = 22.0;
    static constexpr double DHANISTHA_END = 23.0;
    static constexpr double SHATABHISHA_START = 23.0;
    static constexpr double SHATABHISHA_END = 24.0;
    static constexpr double PURVA_BHADRAPADA_START = 24.0;
    static constexpr double PURVA_BHADRAPADA_END = 25.0;
    static constexpr double UTTARA_BHADRAPADA_START = 25.0;
    static constexpr double UTTARA_BHADRAPADA_END = 26.0;
    static constexpr double REVATI_START = 26.0;
    static constexpr double REVATI_END = 0.0;
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

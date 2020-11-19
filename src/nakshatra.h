#ifndef NAKSHATRA_H
#define NAKSHATRA_H

namespace vp {

struct Longitude_sidereal {
    double longitude;
};

class Nakshatra
{
public:
    double nakshatra;
    explicit Nakshatra(double nakshatra_value);
    explicit Nakshatra(vp::Longitude_sidereal longitude);
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

}

#endif // NAKSHATRA_H

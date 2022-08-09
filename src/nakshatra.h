#ifndef NAKSHATRA_H
#define NAKSHATRA_H

#include <cmath>
#include <chrono>
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
    static constexpr Nakshatra MRGASHIRA_START() { return Nakshatra{4.0}; }
    static constexpr Nakshatra MRGASHIRA_END() { return Nakshatra{5.0}; }
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

    static constexpr std::chrono::duration<double, std::ratio<3600>> AverageLength() {
        using namespace std::literals::chrono_literals;
        return 24h + 17min + 9.36s; // from "Basics of Panchangam" by S.Narasimha Rao, p.21
    }

    static constexpr std::chrono::duration<double, std::ratio<3600>> MaxLengthOrMore() {
        using namespace std::literals::chrono_literals;
        // 26h 32m 24s according to "Basics of Panchangam" by S.Narasimha Rao, p.21
        // Since this number has to be greater than max, round up to 27 hours
        return  27h;
    }

    static constexpr std::chrono::duration<double, std::ratio<3600>> MinLengthOrLess() {
        using namespace std::literals::chrono_literals;
        //  22h 22m 24s according to "Basics of Panchangam" by S.Narasimha Rao, p.21
        // Since this number has to be less than min, round down to 22 hours
        return  22h;
    }

    bool is_rohini() const;
    bool is_mrgashira() const;
};

class DiscreteNakshatra {
public:
    DiscreteNakshatra(Nakshatra n);
    static constexpr DiscreteNakshatra Unknown() { return DiscreteNakshatra{-1}; }
    static constexpr DiscreteNakshatra Ashvini() { return DiscreteNakshatra{0}; }
    static constexpr DiscreteNakshatra Bharani() { return DiscreteNakshatra{1}; }
    static constexpr DiscreteNakshatra Kritika() { return DiscreteNakshatra{2}; }
    static constexpr DiscreteNakshatra Rohini() { return DiscreteNakshatra{3}; }
    static constexpr DiscreteNakshatra Mrgashirsha() { return DiscreteNakshatra{4}; }
    static constexpr DiscreteNakshatra Ardra() { return DiscreteNakshatra{5}; }
    static constexpr DiscreteNakshatra Punarvasu() { return DiscreteNakshatra{6}; }
    static constexpr DiscreteNakshatra Pushya() { return DiscreteNakshatra{7}; }
    static constexpr DiscreteNakshatra Ashlesha() { return DiscreteNakshatra{8}; }
    static constexpr DiscreteNakshatra Magha() { return DiscreteNakshatra{9}; }
    static constexpr DiscreteNakshatra PurvaPhalguni() { return DiscreteNakshatra{10}; }
    static constexpr DiscreteNakshatra UttaraPhalguni() { return DiscreteNakshatra{11}; }
    static constexpr DiscreteNakshatra Hasta() { return DiscreteNakshatra{12}; }
    static constexpr DiscreteNakshatra Chitra() { return DiscreteNakshatra{13}; }
    static constexpr DiscreteNakshatra Svati() { return DiscreteNakshatra{14}; }
    static constexpr DiscreteNakshatra Vishakha() { return DiscreteNakshatra{15}; }
    static constexpr DiscreteNakshatra Arundhati() { return DiscreteNakshatra{16}; }
    static constexpr DiscreteNakshatra Jyeshtha() { return DiscreteNakshatra{17}; }
    static constexpr DiscreteNakshatra Mula() { return DiscreteNakshatra{18}; }
    static constexpr DiscreteNakshatra PurvaAshadha() { return DiscreteNakshatra{19}; }
    static constexpr DiscreteNakshatra UttaraAshadha() { return DiscreteNakshatra{20}; }
    static constexpr DiscreteNakshatra Shravana() { return DiscreteNakshatra{21}; }
    static constexpr DiscreteNakshatra Dhanistha() { return DiscreteNakshatra{22}; }
    static constexpr DiscreteNakshatra Shatabhisha() { return DiscreteNakshatra{23}; }
    static constexpr DiscreteNakshatra PurvaBhadrapada() { return DiscreteNakshatra{24}; }
    static constexpr DiscreteNakshatra UttaraBhadrapada() { return DiscreteNakshatra{25}; }
    static constexpr DiscreteNakshatra Revati() { return DiscreteNakshatra{26}; }
    friend bool operator ==(DiscreteNakshatra t1, DiscreteNakshatra t2) {
        return t1.num == t2.num;
    }
    friend bool operator !=(DiscreteNakshatra t1, DiscreteNakshatra t2) {
        return t1.num != t2.num;
    }
    std::string_view name() const;
private:
    int num = Unknown().num;
    explicit constexpr DiscreteNakshatra(int n) : num(n) {}
};


double positive_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);
double minimal_delta_between_nakshatras(Nakshatra n1, Nakshatra n2);
bool operator == (const Nakshatra n1, const Nakshatra n2);
bool operator != (const Nakshatra n1, const Nakshatra n2);
vp::Nakshatra & operator ++(Nakshatra & n);
vp::Nakshatra operator +(Nakshatra n, double delta);
vp::Nakshatra operator -(Nakshatra n, double delta);
double positive_delta_between_longitudes(Nirayana_Longitude l1, Nirayana_Longitude l2);
double minimal_delta_between_longitudes(Nirayana_Longitude l1, Nirayana_Longitude l2);
bool operator ==(Nirayana_Longitude l1, Nirayana_Longitude l2);
bool operator !=(Nirayana_Longitude l1, Nirayana_Longitude l2);

}

namespace  {
std::string_view nakshatra_name(int n) {
    static const char * const names[] {
        "Aśvinī",
        "Bharaṇī",
        "Kṛttikā",
        "Rohiṇī",
        "Mrgaśirā",
        "Ādrā",
        "Punarvasu",
        "Puṣya",
        "Āśleṣā",
        "Maghā",
        "Pūrvā Phalgunī",
        "Uttarā Phalgunī",
        "Hasta",
        "Citrā",
        "Svātī",
        "Viśākhā",
        "Anurādhā",
        "Jyeṣṭhā",
        "Mūla",
        "Pūrvā Aṣāḍha",
        "Uttarā Aṣāḍha",
        "Śravaṇā",
        "Dhaniṣṭhā",
        "Śatabhiṣā",
        "Pūrvā Bhādrapada",
        "Uttarā Bhādrapada",
        "Revatī"
    };
    static_assert (sizeof (names) / sizeof(*names) == 27, "there must be exactly 27 nakshatra names");
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
        int raw_nakshatra_by_thousand = static_cast<int>(std::lround(n.nakshatra * 1000) % 27000);
        int nakshatra_num = raw_nakshatra_by_thousand / 1000; // 0...26
        int nakshatra_fract = raw_nakshatra_by_thousand % 1000;
        return fmt::format_to(ctx.out(), FMT_STRING("{}(.{:03})"), nakshatra_name(nakshatra_num), nakshatra_fract);
    }
};

inline std::string_view vp::DiscreteNakshatra::name() const {
    return nakshatra_name(num);
}

template<>
struct fmt::formatter<vp::DiscreteNakshatra> {
    template<class ParseContext>
    auto parse(ParseContext & ctx) {
        return ctx.begin();
    }
    template<class FormatContext>
    auto format(const vp::DiscreteNakshatra n, FormatContext & ctx) {
        return fmt::format_to(ctx.out(), FMT_STRING("{}"), n.name());
    }
};

#endif // NAKSHATRA_H

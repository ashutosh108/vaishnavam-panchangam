#ifndef VP_MASA_H
#define VP_MASA_H

#include "fmt-format-fixed.h"
#include <string_view>
#include <type_traits>

namespace vp {

enum class Chandra_Masa {
    Unknown = 0,
    Chaitra = 1,
    Vaishakha = 2,
    Jyeshtha = 3,
    Ashadha = 4,
    Shravana = 5,
    Bhadrapada = 6,
    Ashvin = 7,
    Kartika = 8,
    Margashirsha = 9,
    Pausha = 10,
    Magha = 11,
    Phalguna = 12,
    Adhika = 13,
    Kshaya = 14,
};

enum class Saura_Masa {
    Unknown = 0,
    Mesha = 1,
    Vrishabha = 2,
    Mithuna = 3,
    Karkataka = 4,
    Simha = 5,
    Kanya = 6,
    Tula = 7,
    Vrishchika = 8,
    Dhanu = 9,
    Makara = 10,
    Kumbha = 11,
    Mina = 12
};

constexpr auto Saura_Masa_Avg_Length = 365.25 / 12;

/**
 * Saura_Masa_Point: like Saura_Masa, but keeps fractional part.
 * So can be used to see if given point in time close to the end of some
 * particuar masa.
 */
struct Saura_Masa_Point {
    double val; // 0..11.9999999
    Saura_Masa_Point(double val_) : val(val_) {}
    Saura_Masa floor() const;
};

std::underlying_type_t<vp::Saura_Masa> operator-(vp::Saura_Masa m1, vp::Saura_Masa m2);
Saura_Masa operator+(Saura_Masa m, int delta);
Saura_Masa_Point operator+(Saura_Masa m, double delta);
inline Saura_Masa_Point operator-(Saura_Masa m, double delta) {
    return m + -delta;
}
inline bool operator<(Saura_Masa_Point a, Saura_Masa_Point b) {
    return a.val < b.val;
}
inline bool operator<=(Saura_Masa_Point a, Saura_Masa_Point b) {
    return a.val <= b.val;
}
inline bool operator>(Saura_Masa_Point a, Saura_Masa_Point b) {
    return a.val > b.val;
}
inline bool operator>=(Saura_Masa_Point a, Saura_Masa_Point b) {
    return a.val >= b.val;
}

} // namespace vp

template<>
struct fmt::formatter<vp::Saura_Masa> : fmt::formatter<std::string_view> {
    template<class FormatCtx>
    auto format(const vp::Saura_Masa & m, FormatCtx & ctx) {
        const char * name = "Unknown";
        switch (m) {
        case vp::Saura_Masa::Mesha: name = "Meśa"; break;
        case vp::Saura_Masa::Vrishabha: name = "Vṛṣabha"; break;
        case vp::Saura_Masa::Mithuna: name = "Mithuna"; break;
        case vp::Saura_Masa::Karkataka: name = "Karkataka"; break;
        case vp::Saura_Masa::Simha: name = "Siṁha"; break;
        case vp::Saura_Masa::Kanya: name = "Kanyā"; break;
        case vp::Saura_Masa::Tula: name = "Tulā"; break;
        case vp::Saura_Masa::Vrishchika: name = "Vṛścika"; break;
        case vp::Saura_Masa::Dhanu: name = "Dhanu"; break;
        case vp::Saura_Masa::Makara: name = "Makara"; break;
        case vp::Saura_Masa::Kumbha: name = "Kumbha"; break;
        case vp::Saura_Masa::Mina: name = "Mīna"; break;
        case vp::Saura_Masa::Unknown: break;
        }

        return fmt::format_to(ctx.out(), "{}", name);
    }
};

template<>
struct fmt::formatter<vp::Chandra_Masa> : fmt::formatter<std::string_view> {
    template<class FormatCtx>
    auto format(const vp::Chandra_Masa & m, FormatCtx & ctx) {
        const char * name = "Unknown";
        switch (m) {
        case vp::Chandra_Masa::Unknown: name = "Unknown"; break;
        case vp::Chandra_Masa::Chaitra: name = "Chaitra"; break; // (चैत्र)
        case vp::Chandra_Masa::Vaishakha: name = "Vaiśākha"; break; // (वैशाख)
        case vp::Chandra_Masa::Jyeshtha: name = "Jyeṣṭha"; break; // (ज्येष्ठ)
        case vp::Chandra_Masa::Ashadha: name = "Āṣāḍha"; break; // (आषाढ)
        case vp::Chandra_Masa::Shravana: name = "Śṛāvaṇa"; break; // (श्रावण)
        case vp::Chandra_Masa::Bhadrapada: name = "Bhādrapada"; break; // (भाद्रपद)
        case vp::Chandra_Masa::Ashvin: name = "Aśvin"; break; // (अश्विन्)
        case vp::Chandra_Masa::Kartika: name = "Kārtika"; break; // (कार्तिक)
        case vp::Chandra_Masa::Margashirsha: name = "Mārgaśīrṣa"; break; // (मार्गशीर्ष)
        case vp::Chandra_Masa::Pausha: name = "Pauṣa"; break; // (पौष)
        case vp::Chandra_Masa::Magha: name = "Māgha"; break; // (माघ)
        case vp::Chandra_Masa::Phalguna: name = "Phālguna"; break; // (फाल्गुन)
        case vp::Chandra_Masa::Adhika: name = "Adhika"; break;
        case vp::Chandra_Masa::Kshaya: name = "Kṣaya"; break;
        }

        return fmt::format_to(ctx.out(), "{}", name);
    }
};

#endif // VP_MASA_H

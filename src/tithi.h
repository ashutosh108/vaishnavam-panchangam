#ifndef TITHI_H
#define TITHI_H

#include <cassert>
#include <fmt/format.h>
#include <string_view>

namespace vp {

enum class Paksha {
    Shukla,
    Krishna
};

struct Tithi {
    explicit Tithi(double _tithi);
    double tithi;
    Paksha get_paksha();
    bool is_dvadashi();
    bool is_ekadashi();
    bool is_dashami();
    // Pratipat is 0.00...0.99, Dvitiya is 1.00..1.99, etc
    static constexpr double Dashami = 9.0;
    static constexpr double Ekadashi = 10.0;
    static constexpr double Dashami_End = Ekadashi;
    static constexpr double Dvadashi = 11.0;
    static constexpr double Ekadashi_End = Dvadashi;
    static constexpr double Trayodashi = 12.0;
    static constexpr double Dvadashi_End = Trayodashi;

    friend bool operator ==(Tithi const &t1, Tithi const &t2);
    friend bool operator !=(Tithi const &t1, Tithi const &t2);
    friend bool operator <(Tithi const &t1, Tithi const &t2);
    friend bool operator <=(Tithi const &t1, Tithi const &t2);
    friend bool operator >(Tithi const &t1, Tithi const &t2);
    friend bool operator >=(Tithi const &t1, Tithi const &t2);
    Tithi &operator +=(double const);
    double positive_delta_until_tithi(Tithi) const;
    double delta_to_nearest_tithi(Tithi) const;
};

double operator -(Tithi const &, Tithi const &);
Tithi operator +(Tithi const &, double);
Tithi operator -(Tithi const &, double);

} // namespace vp

template<>
struct fmt::formatter<vp::Tithi> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const vp::Tithi & t, FormatContext & ctx) {
        using namespace std::string_view_literals;
        assert(t.tithi >= 0 && t.tithi < 30);
        constexpr const static string_view tithi_names[] = {
            "Pratipat"sv,
            "Dvitiya"sv,
            "Tritiya"sv,
            "Chaturthi"sv,
            "Panchami"sv,
            "Shashthi"sv,
            "Saptami"sv,
            "Ashtami"sv,
            "Navami"sv,
            "Dashami"sv,
            "Ekadashi"sv,
            "Dvadashi"sv,
            "Trayodashi"sv,
            "Chaturdashi"sv
        };
        int int_tithi = static_cast<int>(t.tithi);
        if (int_tithi < 15) {
            if (int_tithi < 14) {
                fmt::format_to(ctx.out(), "Shukla {}", tithi_names[int_tithi]);
            } else {
                fmt::format_to(ctx.out(), "Purnima");
            }
        } else {
            if (int_tithi < 29) {
                fmt::format_to(ctx.out(), "Krishna {}", tithi_names[int_tithi-15]);
            } else {
                fmt::format_to(ctx.out(), "Amavasya");
            }
        }
        return fmt::format_to(ctx.out(), "({:.2g})", t.tithi - int_tithi);
    }
};

#endif // TITHI_H

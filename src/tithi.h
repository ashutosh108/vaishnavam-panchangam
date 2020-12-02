#ifndef TITHI_H
#define TITHI_H

#include <cassert>
#include <chrono>
#include "fmt-format-fixed.h"
#include <string_view>

namespace vp {

enum class Paksha {
    Unknown = 0,
    Shukla = 1,
    Krishna = 2
};

struct Tithi {
    explicit constexpr Tithi(double _tithi) : tithi(_tithi)
    {
        if (tithi < 0 || tithi >= 30) {
            throw std::range_error(fmt::format("Wrong tithi value: {}", tithi));
        }
    }
    double tithi;
    Paksha get_paksha();
    bool is_dvadashi();
    bool is_ekadashi();
    bool is_dashami();
    bool is_shukla_pratipat() const noexcept;
    bool is_krishna_pratipat() const;
    // Pratipat is 0.00...0.99, Dvitiya is 1.00..1.99, etc
    static constexpr Tithi Shukla_Pratipat() { return Tithi{0.0}; }
    static constexpr Tithi Amavasya_End() { return Tithi::Shukla_Pratipat(); }
    static constexpr Tithi Dashami() { return Tithi{9.0}; }
    static constexpr Tithi Ekadashi() { return Tithi{10.0}; }
    static constexpr Tithi Dashami_End() { return Ekadashi(); }
    static constexpr Tithi Dvadashi() { return Tithi{11.0}; }
    static constexpr Tithi Ekadashi_End() { return Dvadashi(); }
    static constexpr Tithi Trayodashi() { return Tithi{12.0}; }
    static constexpr Tithi Dvadashi_End() { return Trayodashi(); }
    static constexpr Tithi Krishna_Pratipat() { return Tithi{15.0}; }
    static constexpr Tithi Purnima_End() { return Tithi::Krishna_Pratipat(); }
    static constexpr Tithi Krishna_Saptami() { return Tithi{15.0 + 6.0}; }

    static constexpr std::chrono::duration<double, std::ratio<3600>> AverageLength() {
        using namespace std::literals::chrono_literals;
        return 23h + 37min + 28.092s; // from "Basics of Panchangam" by S.Narasimha Rao, p.12
    };
    static constexpr std::chrono::duration<double, std::ratio<3600>> MaxLength() {
        using namespace std::literals::chrono_literals;
        // "Basics of Panchangam" by S.Narasimha Rao, p.12 states 26h 6min 24s, but I've found cases of somewhat longer tithis
        // (at least 26h 26m 15.36s long for 2020-05-17 ekadadshi), so 27h it is, to be safe.
        return 27h;
    }

    friend bool operator ==(Tithi const &t1, Tithi const &t2);
    friend bool operator !=(Tithi const &t1, Tithi const &t2);
    friend bool operator <(Tithi const &t1, Tithi const &t2);
    friend bool operator <=(Tithi const &t1, Tithi const &t2);
    friend bool operator >(Tithi const &t1, Tithi const &t2);
    friend bool operator >=(Tithi const &t1, Tithi const &t2);
    Tithi &operator +=(double const);
    double positive_delta_until_tithi(Tithi) const;
    double delta_to_nearest_tithi(Tithi) const;
    static double normalize(double raw_tithi);
    Tithi floor() const;
    Tithi ceil() const;
};

double operator -(Tithi const &, Tithi const &);
Tithi operator +(Tithi const &, double);
Tithi operator -(Tithi const &, double);

} // namespace vp

template<>
struct fmt::formatter<vp::Tithi> : fmt::formatter<std::string_view> {
    bool use_short_form = false;
    bool round_down_to_integer = false;

    /**
     * {}:   Krishna Saptami(0.285)
     * {:s}: K07(0.285)
     * {:d}: Krishna Saptami
     **/
    template<typename ParseContext>
    constexpr auto parse(ParseContext & ctx) {
        auto it = ctx.begin();
        if (it != ctx.end()) {
            if (*it == 's') {
                ++it;
                use_short_form = true;
            } else if (*it == 'd') {
                ++it;
                round_down_to_integer = true;
            }
        }

        return it;
    }

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

        if (use_short_form) {
            if (int_tithi < 15) {
                if (int_tithi < 14) {
                    fmt::format_to(ctx.out(), "Ś{:02d}", int_tithi + 1);
                } else {
                    fmt::format_to(ctx.out(), "Pur");
                }
            } else {
                if (int_tithi < 29) {
                    fmt::format_to(ctx.out(), "K{:02d}", int_tithi - 15 + 1);
                } else {
                    fmt::format_to(ctx.out(), "Ama");
                }
            }
            return fmt::format_to(ctx.out(), "({:.3f})", t.tithi - int_tithi);
        } else {
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
            if (!round_down_to_integer) {
                fmt::format_to(ctx.out(), "({:.2g})", t.tithi - int_tithi);
            }
            return ctx.out();
        }
    }
};

template<>
struct fmt::formatter<vp::Paksha> : fmt::formatter<std::string_view> {
    template<class FormatCtx>
    auto format(vp::Paksha p, FormatCtx & ctx) {
        const char * name = "unknown";
        switch (p) {
        case vp::Paksha::Unknown: name = "unknown"; break;
        case vp::Paksha::Shukla: name = "śukla"; break;
        case vp::Paksha::Krishna: name = "kṛṣṇa"; break;
        }
        return fmt::format_to(ctx.out(), "{}", name);
    }
};

#endif // TITHI_H

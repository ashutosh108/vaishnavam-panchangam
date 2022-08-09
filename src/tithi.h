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

struct DiscreteTithi;

struct Tithi {
    explicit constexpr Tithi(double _tithi) : tithi(_tithi)
    {
        if (tithi < 0 || tithi >= 30) {
            throw std::range_error(fmt::format("Wrong tithi value: {}", tithi));
        }
    }
    explicit Tithi(DiscreteTithi t);
    double tithi;
    Paksha get_paksha();
    bool is_dvadashi();
    bool is_ekadashi();
    bool is_dashami();
    bool is_shukla_pratipat() const noexcept;
    bool is_krishna_pratipat() const;
    bool is_krishna_ashtami() const noexcept;
    bool is_krishna_navami() const noexcept;
    // Pratipat is 0.00...0.99, Dvitiya is 1.00..1.99, etc
    static constexpr Tithi Shukla_Pratipat() { return Tithi{0.0}; }
    static constexpr Tithi Amavasya_End() { return Tithi::Shukla_Pratipat(); }
    static constexpr Tithi Dashami() { return Tithi{9.0}; }
    static constexpr Tithi Dashami_End() { return Ekadashi(); }
    static constexpr Tithi Ekadashi() { return Tithi{10.0}; }
    static constexpr Tithi Ekadashi_End() { return Dvadashi(); }
    static constexpr Tithi Dvadashi() { return Tithi{11.0}; }
    static constexpr Tithi Dvadashi_End() { return Trayodashi(); }
    static constexpr Tithi Trayodashi() { return Tithi{12.0}; }
    static constexpr Tithi Trayodashi_End() { return Tithi::Chaturdashi(); }
    static constexpr Tithi Chaturdashi() { return Tithi{13.0}; }
    static constexpr Tithi Chaturdashi_End() { return Tithi::Purnima(); }
    static constexpr Tithi Purnima() { return Tithi{14.0}; }
    static constexpr Tithi Purnima_End() { return Tithi::Krishna_Pratipat(); }
    static constexpr Tithi Krishna_Pratipat() { return Tithi{15.0}; }
    static constexpr Tithi Krishna_Saptami() { return Tithi{15.0 + 6.0}; }
    static constexpr Tithi Krishna_Saptami_End() { return Tithi::Krishna_Ashtami(); }
    static constexpr Tithi Krishna_Ashtami() { return Tithi{15.0 + 7.0}; }
    static constexpr Tithi Krishna_Ashtami_End() { return Tithi::Krishna_Navami(); }
    static constexpr Tithi Krishna_Navami() { return Tithi{15.0 + 8.0}; }
    static constexpr Tithi Krishna_Navami_End() { return Tithi::Krishna_Dashami(); }
    static constexpr Tithi Krishna_Dashami() { return Tithi{15.0 + 9.0}; }

    static constexpr std::chrono::duration<double, std::ratio<3600>> AverageLength() {
        using namespace std::literals::chrono_literals;
        return 23h + 37min + 28.092s; // from "Basics of Panchangam" by S.Narasimha Rao, p.12
    };
    static constexpr std::chrono::duration<double, std::ratio<3600>> MaxLength() {
        using namespace std::literals::chrono_literals;
        // "Basics of Panchangam" by S.Narasimha Rao, p.12 states 26h 6min 24s, but I've found cases of somewhat longer tithis
        // (at least 26h 26m 15.36s long for 2020-05-17 ekadashi), so 27h it is, to be safe.
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

// DiscreteTithi could have been an enum class but it's a class instead to make uninitialized values impossible.
struct DiscreteTithi {
    explicit DiscreteTithi(Tithi t);
    static constexpr DiscreteTithi Unknown() { return DiscreteTithi{-1}; }
    static constexpr DiscreteTithi Shukla_Pratipat() { return DiscreteTithi{0}; }
    static constexpr DiscreteTithi Shukla_Dvitiya() { return DiscreteTithi{1}; }
    static constexpr DiscreteTithi Shukla_Tritiya() { return DiscreteTithi{2}; }
    static constexpr DiscreteTithi Shukla_Chaturthi() { return DiscreteTithi{3}; }
    static constexpr DiscreteTithi Shukla_Panchami() { return DiscreteTithi{4}; }
    static constexpr DiscreteTithi Shukla_Shashthi() { return DiscreteTithi{5}; }
    static constexpr DiscreteTithi Shukla_Saptami() { return DiscreteTithi{6}; }
    static constexpr DiscreteTithi Shukla_Ashtami() { return DiscreteTithi{7}; }
    static constexpr DiscreteTithi Shukla_Navami() { return DiscreteTithi{8}; }
    static constexpr DiscreteTithi Shukla_Dashami() { return DiscreteTithi{9}; }
    static constexpr DiscreteTithi Shukla_Ekadashi() { return DiscreteTithi{10}; }
    static constexpr DiscreteTithi Shukla_Dvadashi() { return DiscreteTithi{11}; }
    static constexpr DiscreteTithi Shukla_Trayodashi() { return DiscreteTithi{12}; }
    static constexpr DiscreteTithi Shukla_Chaturdashi() { return DiscreteTithi{13}; }
    static constexpr DiscreteTithi Purnima() { return DiscreteTithi{14}; }
    static constexpr DiscreteTithi Krishna_Pratipat() { return DiscreteTithi{15}; }
    static constexpr DiscreteTithi Krishna_Dvitiya() { return DiscreteTithi{16}; }
    static constexpr DiscreteTithi Krishna_Tritiya() { return DiscreteTithi{17}; }
    static constexpr DiscreteTithi Krishna_Chaturthi() { return DiscreteTithi{18}; }
    static constexpr DiscreteTithi Krishna_Panchami() { return DiscreteTithi{19}; }
    static constexpr DiscreteTithi Krishna_Shashthi() { return DiscreteTithi{20}; }
    static constexpr DiscreteTithi Krishna_Saptami() { return DiscreteTithi{21}; }
    static constexpr DiscreteTithi Krishna_Ashtami() { return DiscreteTithi{22}; }
    static constexpr DiscreteTithi Krishna_Navami() { return DiscreteTithi{23}; }
    static constexpr DiscreteTithi Krishna_Dashami() { return DiscreteTithi{24}; }
    static constexpr DiscreteTithi Krishna_Ekadashi() { return DiscreteTithi{25}; }
    static constexpr DiscreteTithi Krishna_Dvadashi() { return DiscreteTithi{26}; }
    static constexpr DiscreteTithi Krishna_Trayodashi() { return DiscreteTithi{27}; }
    static constexpr DiscreteTithi Krishna_Chaturdashi() { return DiscreteTithi{28}; }
    static constexpr DiscreteTithi Amavasya() { return DiscreteTithi{29}; }
    friend bool operator ==(DiscreteTithi t1, DiscreteTithi t2) {
        return t1.num == t2.num;
    }
    friend bool operator !=(DiscreteTithi t1, DiscreteTithi t2) {
        return t1.num != t2.num;
    }
    int num = Unknown().num;
private:
    explicit constexpr DiscreteTithi(int n) : num(n){}
};

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

template<>
struct fmt::formatter<vp::DiscreteTithi> : fmt::formatter<std::string_view> {
    template<class FormatCtx>
    auto format(vp::DiscreteTithi t, FormatCtx & ctx) {
        using namespace std::string_view_literals;

        constexpr const static string_view tithi_names[] = {
            "Pratipat"sv,
            "Dvitīyā"sv,
            "Tṛtīyā"sv,
            "Caturthī"sv,
            "Pañcamī"sv,
            "Ṣaṣṭhī"sv,
            "Saptamī"sv,
            "Aśtamī"sv,
            "Navamī"sv,
            "Daśamī"sv,
            "Ekādaśī"sv,
            "Dvādaśī"sv,
            "Trayodaśī"sv,
            "Caturdaśī"sv
        };
        if (t.num < -1 || t.num >= 30) {
            return fmt::format_to(ctx.out(), FMT_STRING("Invalid tithi"));
        } else if (t.num == -1) {
            return fmt::format_to(ctx.out(), FMT_STRING("Unknown tithi"));
        } else if (t.num >= 0 && t.num <= 13) {
            fmt::format_to(ctx.out(), FMT_STRING("Śukla "));
        } else if (t.num == 14) {
            return fmt::format_to(ctx.out(), FMT_STRING("Pūrṇimā"));
        } else if (t.num >= 15 && t.num <= 28) {
            fmt::format_to(ctx.out(), FMT_STRING("Kṛṣṇa "));
        } else if (t.num == 29) {
            return fmt::format_to(ctx.out(), FMT_STRING("Amāvāsyā"));
        }
        auto tithi_name_index = t.num % 15;
        fmt::format_to(ctx.out(), FMT_STRING("{}"), tithi_names[tithi_name_index]);
        return ctx.out();
    }
};

#endif // TITHI_H

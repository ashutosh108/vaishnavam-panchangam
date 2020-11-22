#ifndef CALC_H
#define CALC_H

#include "location.h"
#include "date-fixed.h"
#include "nakshatra.h"
#include "swe.h"
#include "juldays_ut.h"
#include "tithi.h"
#include "vrata.h"

#include <tl/expected.hpp>

namespace vp {

// for throwing in exceptional cases when we can't find tithi timepoint.
struct CantFindTithiAfter {
    Tithi tithi;
    JulDays_UT after;
};

struct CantFindNakshatraAfter {
    Nakshatra nakshatra;
    JulDays_UT after;
};

enum class Chandra_Masa {
    Chaitra = 1,
    Vaisakha = 2,
    Jyeshta = 3,
    Ashadha = 4,
    Sravana = 5,
    Bhadrapada = 6,
    Aswayuja = 7,
    Kartika = 8,
    Margashira = 9,
    Pushya = 10,
    Magha = 11,
    Phalguna = 12
};

enum class Saura_Masa {
    Mesha = 1,
    Vrishabha = 2,
    Mithuna = 3,
    Karkataka = 4,
    Simha = 5,
    Kanya = 6,
    Tula = 7,
    Vrishchika = 8,
    Dhanus = 9,
    Makara = 10,
    Kumbha = 11,
    Mina = 12
};

class Calc
{
public:
    Calc(Swe swe);
    // main interface: get info for nearest future Vrata after given date
    tl::expected<Vrata, CalcError> find_next_vrata(date::year_month_day after) const;

    // Helper functions. They are public for easier testing,
    // but should be considered private otherwise.

    // find next sunrise which happens to be within Ekadashi tithi
    tl::expected<JulDays_UT, CalcError> find_ekadashi_sunrise(JulDays_UT after) const;

    // get arunodaya(= sunrise - night_length/8) for given sunrise
    tl::expected<JulDays_UT, CalcError> arunodaya_for_sunrise(JulDays_UT sunrise) const;

    // find Shukla- or Krishna- tithi - whichever is closest. Expects Tithi argument to be < 15.0 (shukla paksha)
    JulDays_UT find_either_tithi_start(JulDays_UT, Tithi) const;
    // find exactly given tithi (Shukla- or Krishna-), even if another paksha's tithi is closer.
    JulDays_UT find_exact_tithi_start(JulDays_UT, Tithi) const;
    JulDays_UT find_nakshatra_start(const JulDays_UT, const Nakshatra) const;
    Saura_Masa saura_masa(JulDays_UT time) const;
    Chandra_Masa chandra_masa(JulDays_UT time) const;

    static JulDays_UT proportional_time(JulDays_UT const t1, JulDays_UT const t2, double const proportion);
    JulDays_UT calc_astronomical_midnight(date::year_month_day date) const;

    vp::Swe swe;

private:
    Vrata_Time_Points calc_key_times_from_sunset_and_sunrise(JulDays_UT sunset0, JulDays_UT sunrise1) const;
    tl::expected<JulDays_UT, CalcError> sunset_before_sunrise(JulDays_UT const sunrise) const;
    date::year_month_day get_vrata_date(const JulDays_UT sunrise) const;
    Paran get_paran(const JulDays_UT sunrise2, const JulDays_UT sunset2, const JulDays_UT dvadashi_start, const JulDays_UT dvadashi_end) const;
    Paran atirikta_paran(const JulDays_UT sunrise3, const JulDays_UT sunset3, const JulDays_UT dvadashi_end) const;

    tl::expected<JulDays_UT, CalcError> next_sunrise(JulDays_UT sunrise) const;
    JulDays_UT next_sunrise_v(JulDays_UT sunrise) const;
    bool got_atirikta_ekadashi(const Vrata & vrata) const;
    bool got_atirikta_dvadashi(const Vrata & vrata) const;
    Vrata_Type calc_vrata_type(const Vrata & vrata) const;
};

int operator-(Saura_Masa m1, Saura_Masa m2);

} // namespace vp

namespace {
}

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
        case vp::Saura_Masa::Tula: name = "Tula"; break;
        case vp::Saura_Masa::Vrishchika: name = "Vṛścika"; break;
        case vp::Saura_Masa::Dhanus: name = "Dhanus"; break;
        case vp::Saura_Masa::Makara: name = "Makara"; break;
        case vp::Saura_Masa::Kumbha: name = "Kumbha"; break;
        case vp::Saura_Masa::Mina: name = "Mina"; break;
        }

        return fmt::format_to(ctx.out(), "{}", name);
    }
};

#endif // CALC_H

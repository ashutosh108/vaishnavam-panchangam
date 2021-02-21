#ifndef CALC_H
#define CALC_H

#include "location.h"
#include "date-fixed.h"
#include "masa.h"
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

Nirayana_Longitude starting_longitude(Saura_Masa m);

struct CantFindSankrantiAfter {
    Saura_Masa m;
    Nirayana_Longitude l;
    JulDays_UT after;
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
    tl::expected<date::local_days, CalcError> find_exact_tithi_date(const JulDays_UT, const DiscreteTithi, const date::time_zone *) const;
    JulDays_UT find_nakshatra_start(const JulDays_UT, const Nakshatra) const;
    Saura_Masa saura_masa(JulDays_UT time) const;
    Chandra_Masa chandra_masa_amanta(JulDays_UT time, std::optional<JulDays_UT> * end_time = nullptr) const;
    JulDays_UT find_sankranti(JulDays_UT after, Saura_Masa masa) const;

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

} // namespace vp

#endif // CALC_H

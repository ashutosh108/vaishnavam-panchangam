#ifndef CALC_H
#define CALC_H

#include "location.h"
#include "date-fixed.h"
#include "swe.h"
#include "juldays_ut.h"
#include "tithi.h"
#include "vrata.h"

#include <optional>

namespace vp {

class Calc
{
public:
    Calc(Location coord);
    // main interface: get info for nearest future Vrata after given date
    std::optional<Vrata> find_next_vrata(date::year_month_day after) const;

    // Helper functions. They are public for easier testing,
    // but should be considered private otherwise.

    // find next sunrise which happens to be within Ekadashi tithi
    std::optional<JulDays_UT> find_next_ekadashi_sunrise(JulDays_UT after) const;

    // get arunodaya(= sunrise - night_length/8) for given sunrise
    std::optional<std::pair<JulDays_UT, JulDays_UT>> get_arunodaya(JulDays_UT sunrise) const;

    std::optional<JulDays_UT> get_next_tithi_start(JulDays_UT, Tithi) const;

    static JulDays_UT proportional_time(JulDays_UT const t1, JulDays_UT const t2, double const proportion);

    vp::Swe swe;

private:
    std::optional<JulDays_UT> get_prev_sunset(JulDays_UT const sunrise) const;
    date::year_month_day get_vrata_date(const JulDays_UT &sunrise) const;
    Paran get_paran(JulDays_UT const &last_fasting_sunrise) const;
    Paran atirikta_dvadashi_paran(JulDays_UT const &first_fasting_sunrise) const;

    std::optional<JulDays_UT> next_sunrise(JulDays_UT sunrise) const;
    bool got_atirikta_dvadashi(const JulDays_UT sunrise_on_shuddha_ekadashi_or_next_one) const;
};

} // namespace vp

#endif // CALC_H

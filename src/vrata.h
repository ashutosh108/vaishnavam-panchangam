#ifndef VRATA_H
#define VRATA_H

#include "date-fixed.h"
#include "juldays_ut.h"
#include "location.h"
#include "paran.h"
#include "tithi.h"

#include <fmt/format.h>
#include <limits>
#include <optional>
#include <ostream>

namespace vp {

enum class Vrata_Type {
    Ekadashi,
    With_Atirikta_Ekadashi,
    With_Atirikta_Dvadashi,
};

constexpr inline bool is_atirikta(Vrata_Type type)
{
    return type == Vrata_Type::With_Atirikta_Dvadashi
           ||
           type == Vrata_Type::With_Atirikta_Ekadashi;
}

std::ostream &operator<<(std::ostream &o, Vrata_Type const &v);

/*
 * Helper struct used in Vrata and returned by Calc::calc_ativrddhatvam_for_sunrise():
 * - whether this particular ekAdashI is ativRddhA, vRddhA, samyam or hrasva
 *   (per tithinirNayaH shloka 25: ekA''tidvAdashIvRddhau...)
 * - for each of ativRddhAdi cases: time point which must be clean of dashamI
 *   tithI in correponding ativRddhAdi case for the whole ekAdashI
 *   to be considered uposhya ("fastable").
 */
struct Vrata_Time_Points {
    // Timepoints for uposhya status: if dashamI tithI extends after
    // corresponding time_point, then this ekAdashI is not uposhya
    // and fast must be moved to the next day.
    // Otherwise it's clean ekAdashI (and thus uposhya, "fastable").
    JulDays_UT ativrddha_54gh_40vigh; // for ativRddhA
    JulDays_UT vrddha_55gh;        // vRddhA
    JulDays_UT samyam_55gh_50vigh; // samyam
    JulDays_UT hrasva_55gh_55vigh; // hrasva
    JulDays_UT arunodaya;          // 56gh = 4gh before sūryodaya

    JulDays_UT dashami_start;
    JulDays_UT ekadashi_start;
    JulDays_UT dvadashi_start;
    JulDays_UT trayodashi_start;

    // describes tithI status for some ekAdashI: ativRddhA, vRddhA, samyam, hrasva
    enum class Ativrddhaadi {
        ativrddha, vrddha, samyam, hrasva
    };

    Ativrddhaadi ativrddhaadi() const;

    double_ghatikas dashami_length() const;
    double_ghatikas ekadashi_length() const;
    double_ghatikas dvadashi_length() const;

    JulDays_UT ativrddhaditvam_timepoint() const;
};

// For tests where we need to initialize ativrddhatvam and we don't care about
// it's value, so it's arbitrary.
[[maybe_unused]] constexpr Vrata_Time_Points dummy_vrata_time_points{
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // ativrddha_54gh_40vigh;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // vrddha_55gh;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // samyam_55gh_50vigh;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // hrasva_55gh_55vigh;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // arunodaya;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // dashami_start;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // ekadashi_start;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // dvadashi_start;
    JulDays_UT{double_days{std::numeric_limits<double>::quiet_NaN()}}, // trayodashi_start;
};

std::ostream & operator<<(std::ostream & s, const Vrata_Time_Points::Ativrddhaadi & a);

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    date::year_month_day date;
    Paran paran;
    Location location;
    Vrata_Time_Points times = dummy_vrata_time_points;

    static constexpr double_days nan_days = double_days{std::numeric_limits<double>::quiet_NaN()};
    JulDays_UT sunset_before_ekadashi_sunrise{nan_days};
    JulDays_UT ekadashi_sunrise{nan_days};
    JulDays_UT sunset0{nan_days};
    JulDays_UT sunrise1{nan_days};      // on the first day of vrata (Ekādaśī or Dvādaśī)
    JulDays_UT sunrise2{nan_days};      // on second day after vrata start (≈Dvādaśī)
    JulDays_UT sunset2{nan_days};       // on second day after vrata start (≈Dvādaśī)
    JulDays_UT sunrise3{nan_days};      // on third day after vrata start (≈Trayodaśī)
    JulDays_UT sunset3{nan_days};       // on third day after vrata start (≈Trayodaśī)

    Vrata(){}

    // used only for tests
    Vrata(date::year_month_day _date)
        : date(_date){}

    // used only for tests
    Vrata(Vrata_Type _type, date::year_month_day _date)
        : type(_type),
          date(_date){}

    // we always use this constructor for real calculations
    // "dummy" defaults only used in tests.
    Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran, Location _location = dummy_coord, Vrata_Time_Points _times = dummy_vrata_time_points)
        : type(_type),
          date(_date),
          paran(_paran),
          location(_location),
          times(_times){}
    date::year_month_day local_paran_date();
    std::string location_name() const;
};

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);
std::ostream &operator<<(std::ostream &o, Vrata const &v);

// ref to static vector of all known ekAdashI names in English, capitalized. E.g. "Varūthinī"
const std::vector<std::string> & ekadashi_names();
const std::vector<std::string> & ekadashi_names_rus();
bool ekadashi_name_rus_is_valid(const std::string & name);

} // namespace vp

template<>
struct fmt::formatter<vp::Vrata_Type> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(vp::Vrata_Type vrata, FormatContext & ctx) {
        const char * name = "unknown";
        switch (vrata) {
        case vp::Vrata_Type::Ekadashi:
            name = "Śuddhā Ekādaśī"; break;
        case vp::Vrata_Type::With_Atirikta_Dvadashi:
            name = "Ekādaśī with Atiriktā Dvādaśī (two days fast)"; break;
        case vp::Vrata_Type::With_Atirikta_Ekadashi:
            name = "Ekādaśī with Atiriktā Ekādaśī (two days fast)"; break;
        }
        return fmt::format_to(ctx.out(), "{}", name);
    }
};

template<>
struct fmt::formatter<vp::Vrata> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const vp::Vrata & v, FormatContext & ctx) {
        fmt::format_to(ctx.out(), "{} on {}", v.type, v.date);
        if (is_atirikta(v.type)) {
            date::year_month_day date2 = date::sys_days{v.date} + date::days{1};
            fmt::format_to(ctx.out(), " & {}", date2);
        }
        return ctx.out();
    }
};

#endif // VRATA_H

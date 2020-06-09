#ifndef VRATA_H
#define VRATA_H

#include "date-fixed.h"
#include "juldays_ut.h"
#include "location.h"
#include "paran.h"
#include "tithi.h"

#include <optional>
#include <ostream>

namespace vp {

enum class Vrata_Type {
    Ekadashi,
    With_Atirikta_Dvadashi,
    Atirikta_Ekadashi,
};

bool is_atirikta(Vrata_Type t);

std::ostream &operator<<(std::ostream &o, Vrata_Type const &v);

/*
 * Helper struct used in Vrata and returned by Calc::calc_ativrddhatvam_for_sunrise():
 * - whether this particular ekAdashI is ativRddhA, vRddhA, samyam or hrasva
 *   (per tithinirNayaH shloka 25: ekA''tidvAdashIvRddhau...)
 * - for each of ativRddhAdi cases: time point which must be clean of dashamI
 *   tithI in correponding ativRddhAdi case for the whole ekAdashI
 *   to be considered uposhya ("fastable").
 */
struct Ativrddhatvam {
    JulDays_UT prev_sunset;
    JulDays_UT sunrise;

    // Timepoints for uposhya status: if dashamI tithI extends after
    // corresponding time_point, then this ekAdashI is not uposhya
    // and fast must be moved to the next day.
    // Otherwise it's clean ekAdashI (and thus uposhya, "fastable").
    JulDays_UT time_point_ativrddha_54gh_40vigh; // for ativRddhA
    JulDays_UT time_point_vrddha_55gh;        // vRddhA
    JulDays_UT time_point_samyam_55gh_50vigh; // samyam
    JulDays_UT time_point_hrasva_55gh_55vigh; // hrasva

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

    JulDays_UT relevant_timepoint() const;
};

// For tests where we need to initialize ativrddhatvam and we don't care about
// it's value, so it's arbitrary.
[[maybe_unused]] constexpr Ativrddhatvam dummy_ativrddhatvam{
    JulDays_UT{double_days{0.0}}, //prev_sunset
    JulDays_UT{double_days{0.0}},    // sunrise;

    JulDays_UT{double_days{0.0}}, // time_point_ativrddha_54gh_40vigh; // for ativRddhA
    JulDays_UT{double_days{0.0}}, // time_point_vrddha_55gh;        // vRddhA
    JulDays_UT{double_days{0.0}}, // time_point_samyam_55gh_50vigh; // samyam
    JulDays_UT{double_days{0.0}}, // time_point_hrasva_55gh_55vigh; // hrasva
    JulDays_UT{double_days{0.0}}, // dashami_start;
    JulDays_UT{double_days{0.0}}, // ekadashi_start;
    JulDays_UT{double_days{0.0}}, // dvadashi_start;
    JulDays_UT{double_days{0.0}}, // trayodashi_start;
};

std::ostream & operator<<(std::ostream & s, const Ativrddhatvam::Ativrddhaadi & a);

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    date::year_month_day date;
    Paran paran;
    Location location;
    Ativrddhatvam ativrddhatvam;

    // used only for tests
    Vrata(date::year_month_day _date, Ativrddhatvam _ativrddhatvam = dummy_ativrddhatvam)
        : date(_date),
          ativrddhatvam(_ativrddhatvam){}

    // used only for tests
    Vrata(Vrata_Type _type, date::year_month_day _date, Ativrddhatvam _ativrddhatvam = dummy_ativrddhatvam)
        : type(_type),
          date(_date),
          ativrddhatvam(_ativrddhatvam){}

    // we always use this constructor for real calculations
    // "dummy" defaults only used in tests.
    Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran, Location _location = dummy_coord, Ativrddhatvam _ativrddhatvam = dummy_ativrddhatvam)
        : type(_type),
          date(_date),
          paran(_paran),
          location(_location),
          ativrddhatvam(_ativrddhatvam){}
    date::year_month_day local_paran_date();
};

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);
std::ostream &operator<<(std::ostream &o, Vrata const &v);

// ref to static vector of all known ekAdashI names in English, capitalized. E.g. "Varūthinī"
const std::vector<std::string> & ekadashi_names();
const std::vector<std::string> & ekadashi_names_rus();
bool ekadashi_name_rus_is_valid(const std::string & name);

} // namespace vp

#endif // VRATA_H

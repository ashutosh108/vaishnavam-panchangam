#ifndef VRATA_H
#define VRATA_H

#include "date-fixed.h"
#include "paran.h"
#include "juldays_ut.h"
#include "tithi.h"

#include <optional>
#include <ostream>

namespace vp {

enum class Vrata_Type {
    Ekadashi,
    Sandigdha_Ekadashi, // when Ekadashi Tithi starts before Arunodaya, but less than 0.25 muhurtas before.
    With_Atirikta_Dvadashi,
    Sandigdha_With_Atirikta_Dvadashi,
    Atirikta_Ekadashi,
    Sandigdha_Atirikta_Ekadashi
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
    JulDays_UT time_point_54gh_40vigh; // for ativRddhA
    JulDays_UT time_point_55gh;        // vRddhA
    JulDays_UT time_point_55gh_50vigh; // samyam
    JulDays_UT time_point_55gh_55vigh; // hrasva

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
};

std::ostream & operator<<(std::ostream & s, const Ativrddhatvam::Ativrddhaadi & a);

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    date::year_month_day date;
    Paran paran;
    std::optional<Ativrddhatvam> ativrddhatvam;

    // used only for tests
    Vrata(date::year_month_day _date) : date(_date){}

    // used only for tests
    Vrata(Vrata_Type _type, date::year_month_day _date)
        : type(_type),
          date(_date){}

    // we always use this constructor for real calculations
    Vrata(Vrata_Type _type, date::year_month_day _date, Paran _paran, std::optional<Ativrddhatvam> _ativrddhatvam = std::nullopt)
        : type(_type),
          date(_date),
          paran(_paran),
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

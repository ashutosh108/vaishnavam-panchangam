#ifndef VRATA_H
#define VRATA_H

#include "calc-error.h"
#include "date-fixed.h"
#include "juldays_ut.h"
#include "location.h"
#include "masa.h"
#include "paran.h"
#include "tithi.h"

#include <tl/expected.hpp>
#include "fmt-format-fixed.h"
#include <limits>
#include <map>
#include <optional>

namespace vp {

enum class Vrata_Type {
    Ekadashi,
    With_Atirikta_Ekadashi,
    With_Atirikta_Dvadashi,
    With_Shravana_Dvadashi_Next_Day,
    With_Shravana_Dvadashi_Same_Day,
};

constexpr inline bool is_atirikta(Vrata_Type type)
{
    return type == Vrata_Type::With_Atirikta_Dvadashi
           ||
           type == Vrata_Type::With_Atirikta_Ekadashi
           ||
           type == Vrata_Type::With_Shravana_Dvadashi_Next_Day;
}

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

struct NamedDate {
    std::string name;
};

using NamedDates = std::map<date::local_days, NamedDate>;

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

struct Vrata {
    Vrata_Type type = Vrata_Type::Ekadashi;
    date::year_month_day date;
    Paran paran;
    Location location;
    Vrata_Time_Points times = dummy_vrata_time_points;
    Chandra_Masa masa{Chandra_Masa::Unknown};
    Paksha paksha{Paksha::Unknown};

    static constexpr double_days nan_days = double_days{std::numeric_limits<double>::quiet_NaN()};
    std::optional<JulDays_UT> sunrise0; // only used when sunrise1 is moved one day ahead due to dAshamI viddha
    JulDays_UT sunset0{nan_days};
    JulDays_UT sunrise1{nan_days};      // on the first day of vrata (Ekādaśī or Dvādaśī)
    JulDays_UT sunrise2{nan_days};      // on second day after vrata start (≈Dvādaśī)
    JulDays_UT sunset2{nan_days};       // on second day after vrata start (≈Dvādaśī)
    JulDays_UT sunrise3{nan_days};      // on third day after vrata start (≈Trayodaśī)
    JulDays_UT sunset3{nan_days};       // on third day after vrata start (≈Trayodaśī)

    NamedDates dates_for_this_paksha;

    Vrata(){}

    // used only for tests
    Vrata(date::year_month_day _date, Chandra_Masa _masa, Paksha _paksha)
        : date(_date), masa(_masa), paksha(_paksha) {}

    // used only for tests
    Vrata(Vrata_Type _type, date::year_month_day _date, Chandra_Masa _masa, Paksha _paksha)
        : type(_type),
          date(_date),
          masa(_masa),
          paksha(_paksha) {}

    // we always use this constructor for real calculations
    // "dummy" defaults only used in tests.
    Vrata(Vrata_Type _type, date::year_month_day _date, Chandra_Masa _masa, Paksha _paksha, Paran _paran, const Location & _location = dummy_coord, const Vrata_Time_Points & _times = dummy_vrata_time_points)
        : type(_type),
          date(_date),
          paran(_paran),
          location(_location),
          times(_times),
          masa(_masa),
          paksha(_paksha){}
    date::year_month_day local_paran_date() const;
    std::string location_name() const;
    std::string ekadashi_name() const;

    static Vrata SampleVrata();
};

bool operator==(Vrata const &, Vrata const &);
bool operator!=(Vrata const &, Vrata const &);

// ref to static vector of all known ekAdashI names in English, capitalized. E.g. "Varūthinī"
const std::vector<std::string> & ekadashi_names();
const std::vector<std::string> & ekadashi_names_rus();
bool ekadashi_name_rus_is_valid(const std::string & name);

using MaybeVrata = tl::expected<Vrata, CalcError>;

struct MinMaxDate {
    std::optional<date::sys_days> min;
    std::optional<date::sys_days> max;
};

// All calculated vratas for the same date but different locations.
class VratasForDate {
private:
    using MaybeVratas = std::vector<MaybeVrata>;
public:
    // true if all vratas in the set are within 1 day from one another.
    bool all_from_same_ekadashi() const;

    date::sys_days guess_start_date_for_next_ekadashi(date::sys_days current_start_date);
    date::sys_days guess_start_date_for_prev_ekadashi(date::sys_days current_start_date);

    MinMaxDate minmax_date() const;
    std::optional<date::sys_days> min_date() const;
    std::optional<date::sys_days> max_date() const;

    inline void push_back(const MaybeVrata & vrata) {
        vector.push_back(vrata);
    }
    inline void push_back(MaybeVrata && vrata) {
        vector.push_back(vrata);
    }
    inline MaybeVratas::const_iterator cbegin() const {
        return vector.cbegin();
    }
    inline MaybeVratas::const_iterator cend() const {
        return vector.cend();
    }
    inline MaybeVratas::size_type size() const {
        return vector.size();
    }
    inline MaybeVratas::iterator begin() {
        return vector.begin();
    }
    inline MaybeVratas::const_iterator begin() const {
        return vector.cbegin();
    }
    inline MaybeVratas::iterator end() {
        return vector.end();
    }
    inline MaybeVratas::const_iterator end() const {
        return vector.cend();
    }
    inline void clear() {
        vector.clear();
    }
    inline bool empty() const {
        return vector.empty();
    }
private:
    MaybeVratas vector;
public:
    using value_type = decltype(vector)::value_type;
};

} // namespace vp

template<>
struct fmt::formatter<vp::Vrata_Type> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(vp::Vrata_Type vrata, FormatContext & ctx) {
        const char * name = "unknown";
        switch (vrata) {
        case vp::Vrata_Type::Ekadashi:
            name = "Ekādaśī"; break;
        case vp::Vrata_Type::With_Atirikta_Dvadashi:
            name = "Ekādaśī with Atiriktā Dvādaśī (two days fast)"; break;
        case vp::Vrata_Type::With_Atirikta_Ekadashi:
            name = "Ekādaśī with Atiriktā Ekādaśī (two days fast)"; break;
        case vp::Vrata_Type::With_Shravana_Dvadashi_Next_Day:
            name = "Ekādaśī with next-day Śravaṇa-dvādaśī (two days fast)"; break;
        case vp::Vrata_Type::With_Shravana_Dvadashi_Same_Day:
            name = "Ekādaśī with same-day Śravaṇa-dvādaśī"; break;
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
        fmt::format_to(ctx.out(), FMT_STRING(", {} māsa, {} pakṣa"), v.masa, v.paksha);
        fmt::format_to(ctx.out(), FMT_STRING(", parāṇ: {}"), v.paran);
        return ctx.out();
    }
};

#endif // VRATA_H

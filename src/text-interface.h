#ifndef VP_SRC_TEXT_INTERFACE_H
#define VP_SRC_TEXT_INTERFACE_H

#include "calc-flags.h"
#include "location.h"
#include "nakshatra.h"
#include "tz-fixed.h"
#include "vrata.h"

#include <chrono>
#include "filesystem-fixed.h"
#include <optional>
#include <tl/expected.hpp>
#include <unordered_map>

namespace vp::text_ui {

enum class TrackIntervalChange {
    None,
    Tithi,
    Nakshatra
};

struct NamedTimePoint {
    std::string name;
    JulDays_UT time_point;
    TrackIntervalChange trackIntervalChange = TrackIntervalChange::None;

    const char * trackIntervalString() const {
        switch (trackIntervalChange) {
        case TrackIntervalChange::None:
            return "║│";
        case TrackIntervalChange::Tithi:
            return "╠╪";
        case TrackIntervalChange::Nakshatra:
            return "║├";
        }
        return "??"; // Possible only on coding error
    }
};
using NamedTimePoints = std::vector<NamedTimePoint>;

struct DayByDayInfo {
    vp::Location location;
    date::year_month_day date;
    std::optional<JulDays_UT> sunrise1;
    std::optional<JulDays_UT> sunset1;
    std::optional<JulDays_UT> sunrise2;
    Saura_Masa saura_masa = Saura_Masa::Unknown;
    std::optional<JulDays_UT> saura_masa_until;
    Chandra_Masa chandra_masa = Chandra_Masa::Unknown;
    std::optional<JulDays_UT> chandra_masa_until;
    DiscreteTithi tithi = DiscreteTithi::Unknown();
    std::optional<JulDays_UT> tithi_until;
    DiscreteTithi tithi2 = DiscreteTithi::Unknown();
    std::optional<JulDays_UT> tithi2_until;
    DiscreteNakshatra nakshatra = DiscreteNakshatra::Unknown();
    std::optional<JulDays_UT> nakshatra_until;
    DiscreteNakshatra nakshatra2 = DiscreteNakshatra::Unknown();
    std::optional<JulDays_UT> nakshatra2_until;
    NamedTimePoints events;
};


/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir) */
void change_to_data_dir(const char* argv0);

date::year_month_day parse_ymd(const std::string_view s);

tl::expected<vp::Vrata, vp::CalcError> calc_and_report_one(date::year_month_day base_date, const Location & coord, const fmt::appender & out);
// Find next ekAdashI vrata for the named location, report details to the output buffer.
tl::expected<vp::Vrata, vp::CalcError> find_calc_and_report_one(date::year_month_day base_date, const char * location_name, const fmt::appender & out);

DayByDayInfo daybyday_calc_one(date::year_month_day base_date, const Location & coord, vp::CalcFlags flags);
void daybyday_print_one(date::year_month_day base_date, const char * location_name, const fmt::appender & out, vp::CalcFlags flags);
void calc_and_report_all(date::year_month_day d);
vp::VratasForDate calc(date::year_month_day base_date, std::string location_name, CalcFlags flags = CalcFlags::Default);
std::string program_name_and_version();

class LocationDb {
public:

    auto begin() { return locations().cbegin(); }
    auto end() { return locations().cend(); }
    static std::optional<Location> find_coord(const char *location_name);

private:
    static const std::vector<Location> & locations();
};

namespace detail {

fs::path determine_exe_dir(const char* argv0);
fs::path determine_working_dir(const char* argv0);

} // namespace detail

} // namespace vp::text_ui

#endif // VP_SRC_TEXT_INTERFACE_H

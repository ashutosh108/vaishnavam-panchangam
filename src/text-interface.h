#ifndef VP_SRC_TEXT_INTERFACE_H
#define VP_SRC_TEXT_INTERFACE_H

#include "calc-flags.h"
#include "location.h"
#include "tz-fixed.h"
#include "vrata.h"

#include <chrono>
#include "filesystem-fixed.h"
#include <optional>
#include <tl/expected.hpp>
#include <unordered_map>

namespace vp::text_ui {

struct NamedTimePoint {
    std::string name;
    JulDays_UT time_point;
};
using NamedTimePoints = std::vector<NamedTimePoint>;

struct DayByDayInfo {
    std::optional<JulDays_UT> sunrise1;
    std::optional<JulDays_UT> sunset1;
    std::optional<JulDays_UT> sunrise2;
    Saura_Masa saura_masa = Saura_Masa::Unknown;
    Chandra_Masa chandra_masa = Chandra_Masa::Unknown;
    NamedTimePoints events;
};


/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir) */
void change_to_data_dir(const char* argv0);

date::year_month_day parse_ymd(const std::string_view s);
tl::expected<vp::Vrata, vp::CalcError> calc_one(date::year_month_day base_date, Location location, CalcFlags flags = CalcFlags::Default);
tl::expected<vp::Vrata, vp::CalcError> calc_and_report_one(date::year_month_day base_date, Location coord, fmt::memory_buffer & buf);

// Find next ekAdashI vrata for the named location, report details to the output buffer.
tl::expected<vp::Vrata, vp::CalcError> find_calc_and_report_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf);
void report_details(const vp::MaybeVrata & vrata, fmt::memory_buffer & buf);

DayByDayInfo daybyday_calc_one(date::year_month_day base_date, Location coord, vp::CalcFlags flags);
void daybyday_print_one(date::year_month_day base_date, Location coord, fmt::memory_buffer & buf, vp::CalcFlags flags);
void daybyday_print_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf, vp::CalcFlags flags);
void calc_and_report_all(date::year_month_day d);
vp::VratasForDate calc_all(date::year_month_day, CalcFlags flags = CalcFlags::Default);
vp::VratasForDate calc_one(date::year_month_day base_date, std::string location_name, CalcFlags flags = CalcFlags::Default);
std::string version();
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

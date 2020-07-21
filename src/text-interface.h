#ifndef VP_SRC_TEXT_INTERFACE_H
#define VP_SRC_TEXT_INTERFACE_H

#include <chrono>
#include "filesystem-fixed.h"
#include <optional>
#include <tl/expected.hpp>

#include "location.h"
#include "swe.h"
#include "tz-fixed.h"
#include "vrata.h"

namespace vp::text_ui {

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir) */
void change_to_data_dir(const char* argv0);

date::year_month_day parse_ymd(const std::string_view s);
tl::expected<vp::Vrata, vp::CalcError> calc_one(date::year_month_day base_date, Location location);
tl::expected<vp::Vrata, vp::CalcError> calc_and_report_one(date::year_month_day base_date, Location coord, fmt::memory_buffer & buf);

// Find next ekAdashI vrata for the named location, report details to the output buffer.
tl::expected<vp::Vrata, vp::CalcError> find_calc_and_report_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf);

void print_detail_one(date::year_month_day base_date, const char *location_name, Location coord, fmt::memory_buffer & buf);
void print_detail_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf);
void calc_all(date::year_month_day d);
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

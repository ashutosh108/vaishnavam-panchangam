#ifndef VP_SRC_TEXT_INTERFACE_H
#define VP_SRC_TEXT_INTERFACE_H

#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>

#include "location.h"
#include "tz-fixed.h"

namespace vp::text_ui {

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir) */
void change_to_data_dir(const char* argv0);

date::year_month_day parse_ymd(const char *s);
void calc_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o=std::cout);
void calc_one(date::year_month_day base_date, const char * location_name, std::ostream &o=std::cout);
void print_detail_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o=std::cout);
void print_detail_one(date::year_month_day base_date, const char * location_name, std::ostream &o=std::cout);
void calc_all(date::year_month_day d, std::ostream &o=std::cout);

class LocationDb {
public:

    auto begin() { return locations().cbegin(); }
    auto end() { return locations().cend(); }
    static std::optional<Location> find_coord(const char *location_name);

private:
    static const std::vector<Location> & locations();
};

namespace detail {

std::filesystem::path determine_exe_dir(const char* argv0);
std::filesystem::path determine_working_dir(const char* argv0);

} // namespace detail

} // namespace vp::text_ui

#endif // VP_SRC_TEXT_INTERFACE_H

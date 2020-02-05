#include "text-interface.h"

#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>

#include "calc.h"
#include "vrata_detail.h"

using namespace vp;

namespace vp::text_ui {

date::year_month_day parse_ymd(const char *s) {
    std::tm tm{};
    std::istringstream stream{s};
    stream >> std::get_time(&tm, "%Y-%m-%d");
    return date::year_month_day{date::year{tm.tm_year+1900}, date::month{static_cast<unsigned int>(tm.tm_mon+1)}, date::day{static_cast<unsigned int>(tm.tm_mday)}};
}

std::optional<Location> find_coord(const char *location_name) {
    auto found = std::find_if(
                std::begin(detail::LocationDb::locations()),
                std::end(detail::LocationDb::locations()),
                [=](auto named_coord){
                    return strcmp(named_coord.name, location_name) == 0;
                }
    );
    if (found == std::end(detail::LocationDb::locations())) return std::nullopt;
    return found->coord;
}

void calc_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o) {
    auto vrata = Calc{coord}.find_next_vrata(base_date);
    if (!vrata.has_value()) {
        o << location_name << ": calculation error, can't find next Ekadashi. Sorry.\n";
    } else {
        Vrata_Detail vd{*vrata, coord};
        o << location_name << '\n' << vd << "\n\n";
    }
}

void calc_one(date::year_month_day base_date, const char * location_name, std::ostream &o) {
    std::optional<Location> coord = find_coord(location_name);
    if (!coord) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }
    calc_one(base_date, location_name, *coord, o);
}

void print_detail_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o) {
    o << location_name << ' ' << base_date << '\n';
    o << "<to be implemented>\n";
    Calc calc{coord};
    auto sunrise = calc.swe.get_sunrise(JulDays_UT{base_date});
    if (sunrise) {
        auto arunodaya_info = calc.get_arunodaya(*sunrise);
        if (arunodaya_info) {
            auto [arunodaya, arunodaya_half_ghatika_before] = *arunodaya_info;
            o << "arunodaya-1/2ghatika: " << arunodaya_half_ghatika_before
                    << ' ' << calc.swe.get_tithi(arunodaya_half_ghatika_before) << '\n';
            o << "arunodaya: " << arunodaya
                    << ' ' << calc.swe.get_tithi(arunodaya) << '\n';
        }
        o << "sunrise: " << *sunrise << ' ' << calc.swe.get_tithi(*sunrise) << '\n';
    }
}

void print_detail_one(date::year_month_day base_date, const char * location_name, std::ostream &o) {
    std::optional<Location> coord = find_coord(location_name);
    if (!coord) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }
    print_detail_one(base_date, location_name, *coord, o);
}

void calc_all(date::year_month_day d, std::ostream &o) {
    for (auto &l : detail::LocationDb::locations()) {
        calc_one(d, l.name, l.coord, o);
    }
}

namespace detail {

std::filesystem::path determine_exe_dir(const char* argv0) {
    namespace fs = std::filesystem;
    return fs::absolute(fs::path{argv0}).parent_path();
}

std::filesystem::path determine_working_dir(const char* argv0) {
    namespace fs = std::filesystem;
    auto exe_dir = detail::determine_exe_dir(argv0);

    constexpr int max_steps_up = 2;

    for (int step=0; step < max_steps_up; ++step) {
        // Most common case: "eph" and "tzdata" directories exist in the same directory as .exe
        if (fs::exists(exe_dir / "eph")) {
            return exe_dir;
        }

        // But when running exes from Debug/ or Release/ subdirectories, we need to make one step up.
        // But only make one step up: if the data dir still doesn't exist there either, too bad...
        exe_dir = exe_dir.parent_path();
    }
    // fallback: return whatever we got, even if we couldn't find proper working dir.
    return exe_dir;
}

}

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir or the one above) */
void change_to_data_dir(const char* argv0)
{
    auto working_dir = detail::determine_working_dir(argv0);
    std::filesystem::current_path(working_dir);
}

} // namespace vp::text_ui

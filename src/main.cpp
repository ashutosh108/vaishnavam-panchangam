#include <chrono>
#include <cstring>
#include "fmt-format-fixed.h"

#include "text-interface.h"

// include Windows.h should go after including date.h (which is included from text-interface.h).
// Otherwise troubles with min() which is used both as: 1) a macro in Windows.h 2) method function in date.h.
#ifdef _WIN32
#include <Windows.h>
#endif

void print_usage() {
    fmt::print("{}\n"
               "USAGE:\n"
               "vaishnavam-panchangam YYYY-MM-DD latitude longitude\n"
               "vaishnavam-panchangam YYYY-MM-DD location-name\n"
               "\n"
               "    latitude and longitude are given as decimal degrees (e.g. 30.7)\n",
               vp::text_ui::program_name_and_version());
}

int main(int argc, char *argv[]) try
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    vp::text_ui::change_to_data_dir(argv[0]);
    date::set_install("tzdata");
    if (argc-1 >= 1 && strcmp(argv[1], "-d") == 0) {
        if (argc-1 != 3) {
            print_usage();
            exit(-1);
        }
        auto base_date = vp::text_ui::parse_ymd(argv[2]);
        const char * const location_name = argv[3];
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(base_date, location_name, fmt::appender{buf}, vp::CalcFlags::Default);
        fmt::print("{}", std::string_view{buf.data(), buf.size()});
    } else {
        if (argc-1 != 1 && argc-1 != 2 && argc-1 != 3) {
            print_usage();
            exit(-1);
        }

        auto base_date = vp::text_ui::parse_ymd(argv[1]);
        if (argc-1 <= 1) {
            vp::text_ui::calc_and_report_all(base_date);
        } else if (argc-1 == 2) {
            const char * const location_name = argv[2];
            fmt::memory_buffer buf;
            vp::text_ui::find_calc_and_report_one(base_date, location_name, fmt::appender{buf});
            fmt::print("{}", std::string_view{buf.data(), buf.size()});
        } else {
            double latitude = std::stod(argv[2]);
            double longitude = std::stod(argv[3]);
            fmt::memory_buffer buf;
            vp::text_ui::calc_and_report_one(base_date, vp::Location{vp::Latitude{latitude}, vp::Longitude{longitude}}, fmt::appender{buf});
            fmt::print("{}", std::string_view{buf.data(), buf.size()});
        }
    }
} catch(const std::runtime_error & err) {
    fmt::print(stderr, "Fatal error: {}\n", err.what());
} catch(...) {
    fmt::print(stderr, "Fatal error: Unknown exception\n");
}

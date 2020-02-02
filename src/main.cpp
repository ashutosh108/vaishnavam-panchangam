#include <iostream>
#include <chrono>
#include <cstring>

#include "text-interface.h"

using namespace vp::text_ui;

void print_usage() {
    std::cout << "USAGE:\n";
    std::cout << "vaishnavam-panchangam YYYY-MM-DD latitude longitude\n";
    std::cout << "vaishnavam-panchangam YYYY-MM-DD location-name\n";
    std::cout << "\n";
    std::cout << "    latitude and longitude are given as decimal degrees (e.g. 30.7)\n";
}

int main(int argc, char *argv[])
{
    date::set_install("tzdata");
    if (argc-1 >= 1 && strcmp(argv[1], "-d") == 0) {
        if (argc-1 != 3) {
            print_usage();
            exit(-1);
        }
        auto base_date = parse_ymd(argv[2]);
        const char * const location_name = argv[3];
        print_detail_one(base_date, location_name);
    } else {
        if (argc-1 != 1 && argc-1 != 2 && argc-1 != 3) {
            print_usage();
            exit(-1);
        }

        auto base_date = parse_ymd(argv[1]);
        if (argc-1 <= 1) {
            calc_all(base_date);
        } else if (argc-1 == 2) {
            const char * const location_name = argv[2];
            calc_one(base_date, location_name);
        } else {
            double latitude = std::stod(argv[2]);
            double longitude = std::stod(argv[3]);
            calc_one(base_date, "<custom location>", vp::Location{latitude, longitude});
        }
    }
}

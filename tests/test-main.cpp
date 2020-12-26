#define CATCH_CONFIG_RUNNER
//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch-formatters.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include "fmt/chrono.h"
#include "tz-fixed.h"

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    int ret_code = 0;
    try {

#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif

        date::set_install("tzdata");
        ret_code = Catch::Session().run(argc, argv);
    } catch (std::exception & e) {
        fmt::print(FMT_STRING("Unexpected exception during test run, aborting: {}\n"), e.what());
        ret_code = 1;
    } catch(...) {
        fmt::print(FMT_STRING("Unexpected exception (not std::exception) during test run, aborting\n"));
        ret_code = 1;
    }

    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
    fmt::print(FMT_STRING("Run time: {}\n"), duration);

    return ret_code;
}

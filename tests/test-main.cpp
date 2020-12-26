#define CATCH_CONFIG_RUNNER
#include "catch-formatters.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include "fmt/chrono.h"
#include "tz-fixed.h"

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    date::set_install("tzdata");
    auto ret_code = Catch::Session().run(argc, argv);

    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
    fmt::print(FMT_STRING("Run time: {}\n"), duration);

    return ret_code;
}

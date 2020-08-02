#define CATCH_CONFIG_RUNNER
#include "catch-formatters.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include "tz-fixed.h"

int main(int argc, char* argv[]) {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    date::set_install("tzdata");
    return Catch::Session().run(argc, argv);
}

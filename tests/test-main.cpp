#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "date/tz.h"
#pragma GCC diagnostic pop

int main(int argc, char* argv[]) {
    date::set_install("../vendor/tzdata");
    return Catch::Session().run(argc, argv);
}

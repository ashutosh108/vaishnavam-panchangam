#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "tz-fixed.h"

int main(int argc, char* argv[]) {
    date::set_install("../vendor/tzdata");
    return Catch::Session().run(argc, argv);
}

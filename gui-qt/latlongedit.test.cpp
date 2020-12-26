//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch-formatters.h>

#include "latlongedit.h"

#include <QApplication>

namespace {
constexpr vp::Latitude arbitraty_latitude{30.0};
constexpr vp::Longitude arbitraty_longitude{60.0};
}

TEST_CASE("Can create LatLongEdit with Latitude and Longitude params") {
//    BENCHMARK("qwe") {}
    int argc{0};
    char ** argv = nullptr;
    QApplication a{argc, argv};
    LatLongEdit e{arbitraty_latitude, arbitraty_longitude, nullptr};
}

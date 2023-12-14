//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch-formatters.h>

#include "latlongedit.h"

#include <QApplication>

namespace {
constexpr vp::Latitude arbitraty_latitude{30.0};
constexpr vp::Longitude arbitraty_longitude{60.0};
constexpr vp::Coord arbitrary_coord{arbitraty_latitude, arbitraty_longitude};
}

TEST_CASE("LatLongEdit works") {
    int argc{0};
    char ** argv = nullptr;
    QApplication a{argc, argv};

    SECTION("Can create LatLongEdit") {
    //    BENCHMARK("qwe") {}
        LatLongEdit e{nullptr};
    }

    SECTION("LatLongEdit setCoord() works") {
        LatLongEdit e{nullptr};
        vp::Coord c2{arbitrary_coord.latitude + 5.0, arbitrary_coord.longitude + 5.0};
        e.setCoord(c2);
        REQUIRE(e.coord == c2);
        REQUIRE(e.coord != arbitrary_coord);
    }

    SECTION("LatLongEdit shows expected lat/long format") {
        LatLongEdit e{nullptr};
        e.setCoord(arbitrary_coord);
        REQUIRE(e.text().toStdString() == "30.0000, 60.0000");
    }
}

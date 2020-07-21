#include "date-fixed.h"
#include <fmt/core.h>
#include "location.h"
#include "text-interface.h"
#include "vrata.h"
#include "vrata_detail_printer.h"

#include "catch.hpp"


using namespace date;
using namespace vp;
using Catch::Matchers::Contains;

TEST_CASE("Print result is not empty") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{50.0_N, 50.0_E}, buf);
    REQUIRE(fmt::to_string(buf) != "");
}

TEST_CASE("aruNodaya is given in the Vrata_Detail_Printer in generic case") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{50.0_N, 50.0_E}, buf);
    REQUIRE_THAT(fmt::to_string(buf), Contains("aruṇodaya"));
}

TEST_CASE("aruNodaya is given in the Vrata_Detail_Printer in extreme north case (summer)") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{70.0_N, 50.0_E}, buf);
    REQUIRE_THAT(fmt::to_string(buf), Contains("aruṇodaya"));
}

TEST_CASE("both dates are given for two-days fasts") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, kiev_coord, buf);
    const auto str = fmt::to_string(buf);
    REQUIRE_THAT(str, Contains("on 2020-06-16 & 2020-06-17"));
}

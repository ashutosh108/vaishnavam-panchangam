#include "date-fixed.h"
#include "location.h"
#include "text-interface.h"
#include "vrata.h"
#include "vrata_detail_printer.h"

#include "catch.hpp"
#include <sstream>


using namespace date;
using namespace vp;
using Catch::Matchers::Contains;

TEST_CASE("Print result is not empty") {
    std::stringstream s;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{50.0_N, 50.0_E}, s);
    REQUIRE(s.str() != "");
}

TEST_CASE("aruNodaya is given in the Vrata_Detail_Printer in generic case") {
    std::stringstream s;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{50.0_N, 50.0_E}, s);
    REQUIRE_THAT(s.str(), Contains("aruṇodaya"));
}

TEST_CASE("aruNodaya is given in the Vrata_Detail_Printer in extreme north case (summer)") {
    std::stringstream s;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{70.0_N, 50.0_E}, s);
    REQUIRE_THAT(s.str(), Contains("aruṇodaya"));
}

TEST_CASE("both dates are given for two-days fasts") {
    std::stringstream s;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, kiev_coord, s);
    const auto str = s.str();
    REQUIRE_THAT(str, Contains("on 2020-06-16 & 2020-06-17"));
}

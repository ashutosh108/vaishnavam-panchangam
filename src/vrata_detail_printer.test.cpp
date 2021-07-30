#include "date-fixed.h"
#include <fmt/core.h>
#include "location.h"
#include "text-interface.h"
#include "vrata.h"
#include "vrata_detail_printer.h"

#include "catch-formatters.h"


using namespace date;
using namespace vp;
using Catch::Matchers::Contains;

TEST_CASE("Kiev 2020-06-14") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, kiev_coord, fmt::appender{buf});
    const auto str = fmt::to_string(buf);

    SECTION("Print result is not empty") {
        REQUIRE(str != "");
    }

    SECTION("aruNodaya is given in the Vrata_Detail_Printer in generic case") {
        REQUIRE_THAT(str, Contains("aruṇodaya"));
    }

    SECTION("both dates are given for two-days fasts") {
        REQUIRE_THAT(str, Contains("on 2020-06-16 & 2020-06-17"));
    }

    SECTION("harivAsara is not mentioned") {
        REQUIRE_THAT(str, !Contains("Harivāsara"));
    }
}

TEST_CASE("Kiev 2020-08-29") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/August/29, kiev_coord, fmt::appender{buf});
    const auto str = fmt::to_string(buf);

    SECTION("Print result is not empty") {
        REQUIRE_THAT(str, Contains("Harivāsara"));
        REQUIRE_THAT(str, Contains("23:53"));
    }
}

TEST_CASE("aruNodaya is given in the Vrata_Detail_Printer in extreme north case (summer)") {
    fmt::memory_buffer buf;
    const auto vrata = vp::text_ui::calc_and_report_one(2020_y/June/14, Location{70.0_N, 50.0_E}, fmt::appender{buf});
    REQUIRE_THAT(fmt::to_string(buf), Contains("aruṇodaya"));
}

TEST_CASE("':c' for Vrata_Detail_Printer means compact formatting: no header") {
    fmt::memory_buffer buf;
    fmt::format_to(fmt::appender(buf), "{:c}", vp::Vrata_Detail_Printer{Vrata::SampleVrata()});
    const auto str = fmt::to_string(buf);
    REQUIRE_THAT(str, !Contains("# ")); // "# Sample location" is a header which must not be present.
}

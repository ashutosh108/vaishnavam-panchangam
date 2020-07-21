#include "text-interface.h"

#include "location.h"

#include <array>
#include "catch.hpp"

TEST_CASE("get_exe_dir() exists and returns something") {
    auto exe_dir = vp::text_ui::detail::determine_exe_dir("imaginary/path/to/exe/file");

    // ensure that path ends with what we expect it to
    auto end_iterator = std::end(exe_dir);
    constexpr std::array<const char*, 4> expected_parts{"imaginary", "path", "to", "exe"};
    for (auto component=expected_parts.rbegin(); component != expected_parts.rend(); ++component) {
        REQUIRE(*component == *--end_iterator);
    }
}

TEST_CASE("no sunset: we decrease latitude until we get all necessary sunrises/sunsets") {
    using namespace date;
    auto date = 2020_y/June/3;
    auto vrata = vp::text_ui::calc_one(date, vp::murmansk_coord);
    REQUIRE(vrata.has_value());

    // expect location name to contain "adjusted" because there is no sunset in the original location at those dates.
    auto location_name = vrata->location_name();
    REQUIRE(location_name.find("adjusted") != std::string::npos);
}

TEST_CASE("parse_ymd works in normal case") {
    using namespace date::literals;
    REQUIRE(vp::text_ui::parse_ymd("2020-11-12") == 2020_y/nov/12);
}

TEST_CASE("parse_ymd returns some old date when given non-date string") {
    using namespace date::literals;
    REQUIRE(vp::text_ui::parse_ymd("non-date string") == date::year_month_day{});
}

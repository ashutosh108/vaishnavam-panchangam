#include "text-interface.h"

#include "location.h"

#include <algorithm>
#include <array>
#include "catch-formatters.h"

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
    REQUIRE(vp::text_ui::parse_ymd("non-date string") == date::year_month_day{});
}

TEST_CASE("calc_all returns array with non-error results") {
    using namespace date;
    auto vratas = vp::text_ui::calc_all(2020_y/January/1);

    REQUIRE(vratas.size() > 0);
    REQUIRE(std::all_of(vratas.cbegin(), vratas.cend(), [](const auto & vrata) {
        return vrata.has_value();
    }));
}

TEST_CASE("calc_all adjusts start date to ensure all locations have the same ekAdashI") {
    using namespace date;
    auto vratas = vp::text_ui::calc_all(2020_y/September/14);

    auto [it_min_date, it_max_date] = std::minmax_element(vratas.cbegin(), vratas.cend(), [](const vp::MaybeVrata &l, const vp::MaybeVrata &r) { return l->date < r->date; });
    auto length = date::sys_days{it_max_date->value().date} - date::sys_days{it_min_date->value().date};

    REQUIRE(length.count() <= date::days{1}.count());
}

TEST_CASE("can call calc_one with string for location name") {
    using namespace date;
    auto vratas = vp::text_ui::calc_one(2020_y/January/1, std::string("Kiev"));
    REQUIRE(!vratas.empty());
    REQUIRE(vratas.begin()->has_value());
}

TEST_CASE("print_detail_one for Udupi 2020-11-14 does NOT raise exception and includes Amavasya") {
    fmt::memory_buffer buf;
    using namespace date::literals;
    using Catch::Matchers::Contains;
    const date::year_month_day date{2020_y/11/14};
    REQUIRE_NOTHROW(vp::text_ui::print_detail_one(date, "Udupi", buf, vp::CalcFlags::Default));

    REQUIRE_THAT(fmt::to_string(buf), Contains("Amavasya"));
}

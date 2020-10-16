#include "catch-formatters.h"

#include "table-calendar-generator.h"
#include "text-interface.h"

#include <regex>
#include <sstream>

namespace {
auto some_vratas(date::year_month_day date = date::year{2020}/1/1) {
    return vp::text_ui::calc_all(date);
}

auto some_table(date::year_month_day date = date::year{2020}/1/1) {
    return vp::Table_Calendar_Generator::generate(some_vratas(date));
}
}

TEST_CASE("Table_Calendar_Generator returns reasonable table") {
    const auto table = some_table();
    REQUIRE(table.width() == 6);
    REQUIRE(table.at(0, 3).text == "January 6");
    REQUIRE(table.at(0, 4).text == "January 7");
    REQUIRE(table.at(0, 5).text == "January 8");

    REQUIRE(table.height() >= 20);

    REQUIRE(table.at(1, 0).text == "+5:30");
    REQUIRE(table.at(1, 1).text == "India");
    REQUIRE(table.at(1, 2).text == "Udupi");
    REQUIRE(table.at(1, 3).text == "Śuddhā Ekādaśī");
    REQUIRE(table.at(1, 4).text == ">10:06");
    REQUIRE(table.at(1, 5).text == "");
}

TEST_CASE("Table_Calendar_Generator returns reasonable table adds ' (DST)' for 'summer' times") {
    using namespace date;
    const auto table = some_table(2020_y/July/1);
    int num_with_dst = 0;
    int num_without_dst = 0;
    for (std::size_t row=0; row < table.height(); ++row) {
        if (!table.has_cell(row, 0)) { continue; }
        auto text = table.at(row, 0).text;
        if (text.find("DST") != std::string::npos) {
            ++num_with_dst;
        } else {
            ++num_without_dst;
        }
    }
    REQUIRE(num_with_dst > 0);
    REQUIRE(num_without_dst > 0);
}

TEST_CASE("Table_Calendar_Generator generates 'shrIH'/'Om tatsat' with dates as the top and bottom rows") {
    const auto table = some_table();

    size_t rows = table.height();

    REQUIRE(rows >= 3);

    REQUIRE(table.at(0, 3).text == "January 6");
    REQUIRE(table.at(rows-1, 3).text == "January 6");
    REQUIRE(table.at(0, 4).text == "January 7");
    REQUIRE(table.at(rows-1, 4).text == "January 7");
    REQUIRE(table.at(0, 5).text == "January 8");
    REQUIRE(table.at(rows-1, 5).text == "January 8");

    using Catch::Matchers::Contains;
    REQUIRE_THAT(table.at(0, 0).text, Contains("श्रीः"));
    REQUIRE_THAT(table.at(rows-1, 0).text, Contains("ॐ तत्सत्"));
}

TEST_CASE("generated table has 'mainpart' as a class for top/bottom date headers") {
    const auto table = some_table();
    using Catch::Matchers::Contains;

    REQUIRE_THAT(table.at(0, 3).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(0, 4).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(0, 5).classes, Contains("mainpart"));

    size_t rows = table.height();
    REQUIRE(rows >= 3);

    REQUIRE_THAT(table.at(rows-1, 3).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(rows-1, 4).classes, Contains("mainpart"));
    REQUIRE_THAT(table.at(rows-1, 5).classes, Contains("mainpart"));
}

namespace {
std::chrono::seconds utc_offset_string_to_seconds(std::string utc_offset_string) {
    // e.g. +5:30
    if (utc_offset_string.size() < strlen("+5:30")) { throw std::runtime_error("can't parse UTC offset (too short)" + utc_offset_string); }
    std::regex regex{R"(^([+-])(\d?\d):(\d\d).*)"};
    std::smatch match;
    if (!std::regex_match(utc_offset_string, match, regex)) {
        throw std::runtime_error("can't parse UTC offset (regex does not match) " + utc_offset_string);
    }
    std::string sign = match[1].str();
    std::string hours = match[2].str();
    std::string minutes = match[3].str();
    const auto absolute_offset = std::chrono::hours{std::stoi(hours)} + std::chrono::minutes{std::stoi(minutes)};
    if (sign == "+") {
        return absolute_offset;
    } else if (sign == "-") {
        return -absolute_offset;
    } else {
        throw std::runtime_error("can't parse UTC offset (sign is neither + or -)" + utc_offset_string);
    }
}
}

TEST_CASE("generated table has a separator row when switching to timezone 7 or more hours away from the previous one (e.g. between Petropavlovsk-Kamchatskiy and Yerevan") {
    using namespace std::chrono_literals;
    constexpr auto max_delta = 7h;
    const auto table = some_table();
    size_t separator_rows_count = 0;
    // skip first two rows (top-header and row without top-neighbor) and last two row (bottom-header and without bottom-neighbor)
    for (std::size_t row=2; row < table.height()-2; ++row) {
        if (table.row(row).has_class("separator")) {
            REQUIRE(table.has_cell(row-1, 0));
            REQUIRE(table.has_cell(row+1, 0));
            const auto next_offset = utc_offset_string_to_seconds(table.at(row+1, 0).text);
            const auto prev_offset = utc_offset_string_to_seconds(table.at(row-1,0).text);
            REQUIRE(abs(next_offset - prev_offset) > max_delta);
            ++separator_rows_count;
        }
    }
    REQUIRE(separator_rows_count == 1);
}

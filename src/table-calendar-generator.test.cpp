#include "catch-formatters.h"

#include "table-calendar-generator.h"
#include "text-interface.h"

#include <regex>
#include <sstream>

namespace {
auto some_vratas(date::year_month_day date) {
    return vp::text_ui::calc(date, "all");
}

auto some_table(date::year_month_day date = date::year{2018}/8/15) {
    return vp::Table_Calendar_Generator::generate(some_vratas(date));
}

std::chrono::seconds utc_offset_string_to_seconds(const std::string & utc_offset_string) {
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

TEST_CASE("default table") {
    const auto table = some_table();

    SECTION("Table_Calendar_Generator returns reasonable table") {
        REQUIRE(table.width() == 6);
        REQUIRE(table.at(0, 3).text == "August&nbsp;21");
        REQUIRE(table.at(0, 4).text == "August&nbsp;22");
        REQUIRE(table.at(0, 5).text == "August&nbsp;23");

        REQUIRE(table.height() >= 20);

        using Catch::Matchers::Contains;

        REQUIRE(table.at(1, 0).text == "+5:30");
        REQUIRE(table.at(1, 1).text == "India");
        REQUIRE_THAT(table.at(1, 2).text, Contains("Udupi"));
        REQUIRE(table.at(1, 3).text == "");
        REQUIRE(table.at(1, 4).text == "Pavitrā Ekādaśī");
        REQUIRE_THAT(table.at(1, 5).text, Contains("*"));
    }

    SECTION("Table_Calendar_Generator generates 'shrIH'/'Om tatsat' with dates as the top and bottom rows") {
        size_t rows = table.height();

        REQUIRE(rows >= 3);

        REQUIRE(table.at(0, 3).text == "August&nbsp;21");
        REQUIRE(table.at(rows-1, 3).text == "August&nbsp;21");
        REQUIRE(table.at(0, 4).text == "August&nbsp;22");
        REQUIRE(table.at(rows-1, 4).text == "August&nbsp;22");
        REQUIRE(table.at(0, 5).text == "August&nbsp;23");
        REQUIRE(table.at(rows-1, 5).text == "August&nbsp;23");

        using Catch::Matchers::Contains;
        REQUIRE_THAT(table.at(0, 0).text, Contains("श्रीः"));
        REQUIRE_THAT(table.at(rows-1, 0).text, Contains("ॐ तत्सत्"));
    }

    SECTION("generated table has 'mainpart' as a class for top/bottom date headers") {
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

    SECTION("generated table has a separator row when switching to timezone 7 or more hours away from the previous one (e.g. between Petropavlovsk-Kamchatskiy and Yerevan") {
        using namespace std::chrono_literals;
        constexpr auto max_delta = 7h;
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

    SECTION("generated table has &nbsp; as date separators") {
        REQUIRE(table.at(0, 3).text == "August&nbsp;21");
    }

    SECTION("generated table contains titles for PAraNam details") {
        using Catch::Matchers::Contains;
        REQUIRE_THAT(table.at(1, 5).title, Contains("06:22:42 (sunrise)…08:51:03 (1/5th of daytime)"));
    }

    SECTION("generated table contains titles with timezone name for cell with location name") {
        using Catch::Matchers::Contains;
        REQUIRE_THAT(table.at(1, 2).title, Contains("Asia/Kolkata"));
    }

    SECTION("generated table contains links with details in cells with location name, vrata, pAraNam") {
        using Catch::Matchers::Contains;
        REQUIRE_THAT(table.at(1, 2).text, Contains("<a href"));
    }

    auto find_row = [&table](const std::string & location_name) -> std::size_t {
        for (std::size_t row=1; row<table.height(); ++row) {
            CAPTURE(row);
            const auto text = table.at(row, 2).text;
            if (text.find(location_name) != std::string::npos) {
                return row;
            }
        }
        return 0;
    };

    SECTION("'title' attribute of Pāraṇam includes limit when paran_limit is set") {
        using Catch::Matchers::Contains;
        std::size_t row = find_row("Aktau");
        REQUIRE_THAT(table.at(row, 5).title, Contains("absolute limit is 09:45:38 (dvādaśī end)"));
    }
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

TEST_CASE("generated table shows year for non-default-year dates when default year is specified") {
    using namespace date;
    const auto table{vp::Table_Calendar_Generator::generate(some_vratas(2019_y/December/20), 2020_y)};

    using Catch::Matchers::Contains;
    std::vector<std::pair<size_t, size_t>> to_check{{0, 3}, {0, 4}, {0, 5}, {table.height()-1, 3}, {table.height()-1, 4}, {table.height()-1, 5}};
    for (const auto & pair : to_check) {
        CAPTURE(pair.first, pair.second);
        REQUIRE_THAT(table.at(pair.first, pair.second).text, Contains("2019"));
    }
}

TEST_CASE("Generated table contains additional custom dates with given descriptions") {
    using namespace date;
    auto table = vp::Table_Calendar_Generator::generate(some_vratas(2020_y/1/1), 2020_y, {{2020_y/1/5, "custom1"}, {2020_y/1/9, "custom2"}});
    REQUIRE(table.width() == 8);
    REQUIRE(table.at(1, 3).text == "custom1");
    REQUIRE(table.at(1, 7).text == "custom2");
}

TEST_CASE("Generated table contains Vasanta-pañcamī and other dates from that pakṣa", "[wip]") {
    using namespace date::literals;
    auto table = vp::Table_Calendar_Generator::generate(some_vratas(2021_y/2/10));
    REQUIRE(table.width() == 14);
    REQUIRE(table.at(1, 3).text == "Vasanta-pañcamī");
    REQUIRE(table.at(1, 6).text == "Ratha-saptamī");
    REQUIRE(table.at(1, 7).text == "Bhīṣmāṣtamī");
    REQUIRE(table.at(1, 8).text == "Madhva-navamī (cāndra)");
    REQUIRE(table.at(1, 13).text == "Pūrṇimā, End of Māgha-snāna-vrata");
}

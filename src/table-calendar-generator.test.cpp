#include "catch-formatters.h"

#include "table-calendar-generator.h"
#include "text-interface.h"

#include <regex>
#include <sstream>

using namespace date;
using Catch::Matchers::Contains;
using namespace std::chrono_literals;

namespace {
auto some_vratas(date::year_month_day date) {
    return vp::text_ui::calc(date, "all");
}

// 2018-08-15 is chosen by default because we have both ordinary ekādaśīs
// and atirikā-{ekā,dvā}daśīs there which is an interesting case to test for.
auto some_table(date::year_month_day date = 2018_y/8/15) {
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

        REQUIRE(table.at(1, 0).text == "+5:30");
        REQUIRE(table.at(1, 1).text == "India");
        REQUIRE_THAT(table.at(1, 2).text, Contains("Udupi"));
        REQUIRE_THAT(table.at(1, 3).text, Contains("HV > 01:04"));
        REQUIRE_THAT(table.at(1, 3).text, Contains("22.08"));
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

        REQUIRE_THAT(table.at(0, 0).text, Contains("श्रीः"));
        REQUIRE_THAT(table.at(rows-1, 0).text, Contains("ॐ तत्सत्"));
    }

    SECTION("generated table has 'mainpart' as a class for top/bottom date headers") {
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
        REQUIRE_THAT(table.at(1, 5).title, Contains("06:22:42 (sunrise)…08:51:03 (1/5th of daytime)"));
    }

    SECTION("generated table contains titles with timezone name for cell with location name") {
        REQUIRE_THAT(table.at(1, 2).title, Contains("Asia/Kolkata"));
    }

    SECTION("generated table contains links with details in cells with location name, vrata, pAraNam") {
        REQUIRE_THAT(table.at(1, 2).text, Contains("<a href"));
    }

    auto find_row = [&table](const std::string & location_name) -> std::size_t {
        for (std::size_t row=1; row<table.height(); ++row) {
            CAPTURE(row);
            // skip empty row (e.g. separator between far-east and central Russia)
            if (table.row(row).data.size() < 2) { continue; }
            const auto text = table.at(row, 2).text;
            if (text.find(location_name) != std::string::npos) {
                return row;
            }
        }
        return 0;
    };

    SECTION("'title' attribute of Pāraṇam includes limit when paran_limit is set") {
        std::size_t row = find_row("Aktau");
        REQUIRE_THAT(table.at(row, 5).title, Contains("absolute limit is 09:45:38 (dvādaśī end)"));
    }

    SECTION("atirikā-dvādaśī is in separate cell from previous ekādaśī") {
        std::size_t row = find_row("Simferopol");
        REQUIRE_THAT(table.at(row, 3).text, Contains("Pavitrā Ekādaśī"));
        REQUIRE_THAT(table.at(row, 3).text, !Contains("Atiriktā Dvādaśī"));
        REQUIRE_THAT(table.at(row, 4).text, Contains("Atiriktā Dvādaśī"));
        REQUIRE_THAT(table.at(row, 4).text, !Contains("Pavitrā Ekādaśī"));
    }

    SECTION("css classes are properly set in table") {
        std::size_t row = find_row("Aktau");
        REQUIRE_THAT(table.at(row, 4).classes, Contains("mainpart"));
        REQUIRE_THAT(table.at(row, 4).classes, Contains("vrata"));
    }
}

TEST_CASE("Table_Calendar_Generator returns reasonable table adds ' (DST)' for 'summer' times") {
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
    const auto table{vp::Table_Calendar_Generator::generate(some_vratas(2019_y/December/20), 2020_y)};

    std::vector<std::pair<size_t, size_t>> to_check{{0, 3}, {0, 4}, {0, 5}, {table.height()-1, 3}, {table.height()-1, 4}, {table.height()-1, 5}};
    for (const auto & pair : to_check) {
        CAPTURE(pair.first, pair.second);
        REQUIRE_THAT(table.at(pair.first, pair.second).text, Contains("2019"));
    }
}

TEST_CASE("Custom dates work OK") {
    auto table = vp::Table_Calendar_Generator::generate(
                some_vratas(2020_y/1/1), 2020_y,
                {
                    {date::local_days{2020_y/1/5}, "custom1"},
                    {date::local_days{2020_y/1/9}, "custom2"},
                    {date::local_days{2020_y/1/11}, ""},
                });
    SECTION("Generated table contains additional custom dates with given descriptions") {
        REQUIRE(table.width() == 10);
        REQUIRE(table.at(1, 3).text == "custom1");
        REQUIRE(table.at(1, 7).text == "custom2");
        REQUIRE(table.at(1, 9).text == "");
    }
    SECTION("Custom date with non-empty desription is marked yellow (mainpart + custom CSS class)") {
        REQUIRE_THAT(table.at(1, 3).classes, Contains("mainpart"));
        REQUIRE_THAT(table.at(1, 3).classes, Contains("custom"));
        REQUIRE_THAT(table.at(1, 7).classes, Contains("mainpart"));
        REQUIRE_THAT(table.at(1, 7).classes, Contains("custom"));
    }
    SECTION("Custom date with empty description is NOT marked yellow ('mainpart', but not 'custom' CSS class)") {
        REQUIRE_THAT(table.at(1, 9).classes, Contains("mainpart"));
        REQUIRE_THAT(table.at(1, 9).classes, !Contains("custom"));
    }
}

TEST_CASE("Generated table contains Vasanta-pañcamī and other dates from that pakṣa", "[wip]") {
    auto table = vp::Table_Calendar_Generator::generate(some_vratas(2021_y/2/10));
    REQUIRE(table.width() == 15); // 2 + 12 + one column for skipped date
    REQUIRE(table.at(1, 3).text == "Vasanta-pañcamī");
    REQUIRE(table.at(1, 6).text == "Ratha-saptamī");
    REQUIRE(table.at(1, 7).text == "Bhīṣmāṣṭamī");
    REQUIRE(table.at(1, 8).text == "Madhva-navamī (cāndra)");
    REQUIRE(table.at(1, 14).text == "Pūrṇimā, Māgha-snāna-vrata ends");
}

TEST_CASE("table for Vrata with multiple nameworthy dates on the same day lists all nameworthy dates in the same cell, separated by full stop.") {
    vp::VratasForDate vratas;
    {
        auto vrata = vp::Vrata::SampleVrata(); // 2000_y/1/1
        vrata.dates_for_this_paksha.emplace(date::local_days{2000_y/1/3}, vp::NamedDate{"event1"});
        vrata.dates_for_this_paksha.emplace(date::local_days{2000_y/1/3}, vp::NamedDate{"event2"}); // two events on same day
        vratas.push_back(std::move(vrata));
    }
    const auto table = vp::Table_Calendar_Generator::generate(vratas);
    REQUIRE_THAT(table.at(1, 5).text, Contains("event1. event2"));
}

TEST_CASE("table includes empty ('...') columns for skipped dates", "[wip]") {
    vp::VratasForDate vratas;
    {
        // Vrata on Jan 1st, Paran on Jan 2nd. Nothing on Jan 3rd, so it
        // should become "skipped column", and a separate event on Jan 4th.
        auto vrata = vp::Vrata::SampleVrata(); // 2000_y/1/1
        vrata.dates_for_this_paksha.emplace(date::local_days{2000_y/1/4}, vp::NamedDate{"separate event"});
        vratas.push_back(std::move(vrata));
    }
    const auto table = vp::Table_Calendar_Generator::generate(vratas);
    REQUIRE_THAT(table.at(0, 5).text, Contains("..."));
    REQUIRE_THAT(table.at(1, 6).text, Contains("separate event"));
}

#include "catch-formatters.h"

#include "edit-custom-dates.h"

#include <QRegularExpression>
#include <QString>

TEST_CASE("qstring_to_custom_dates works for simple two-line case") {
    using namespace date;
    auto set = vp::detail::qstring_to_custom_dates("2020-01-01  text\n2020-01-02");
    REQUIRE(!set.empty());
    REQUIRE(set[date::local_days{2020_y/1/1}] == "text");
    REQUIRE(set.find(date::local_days{2020_y/1/2}) != set.end()); // otherwise next line could match non-existing key
    REQUIRE(set[date::local_days{2020_y/1/2}] == "");
}

TEST_CASE("qstring_to_custom_dates for almost default case") {
    auto set = vp::detail::qstring_to_custom_dates(R"(# format for each line is:
# YYYY-MM-DD description
# lines starting from "#" are ignored as comments

# Example (remove # symbols from needed lines):
2020-01-01 Extra day
2020-01-10 Another extra day)");
    using namespace date;
    REQUIRE(!set.empty());
    REQUIRE(set[date::local_days{2020_y/1/1}] == "Extra day");
    REQUIRE(set[date::local_days{2020_y/1/10}] == "Another extra day");
}


TEST_CASE("qstring_to_custom_dates returns empty when all lines are #commented out") {
    auto set = vp::detail::qstring_to_custom_dates("# 2020-01-01 text\n#2020-01-02");
    REQUIRE(set.empty());
}

TEST_CASE("custom_dates_to_qstring returns no non-comment lines for empty set") {
    vp::Custom_Dates dates;
    const auto str = vp::detail::custom_dates_to_qstring(dates);
    const auto lines = str.split(QRegularExpression{R"([\r\n]+)"});
    for (const auto & line : lines) {
        if (!line.isEmpty()) {
            CAPTURE(line.toStdString());
            REQUIRE(line.startsWith("#"));
        }
    }
}

TEST_CASE("custom_dates_to_qstring retains dates and text for a test case") {
    using namespace date;
    vp::Custom_Dates dates{{date::local_days{2020_y/1/1}, "custom1"}, {date::local_days{2020_y/1/2}, ""}};
    const auto str = vp::detail::custom_dates_to_qstring(dates);
    CAPTURE(str.toStdString());
    REQUIRE(str.contains("\n2020-01-01 custom1\n"));
    REQUIRE(str.contains("\n2020-01-02\n"));
}

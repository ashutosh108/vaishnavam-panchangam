
#include "html-parser.h"
#include "vrata.h"

#include "catch.hpp"
#include "date-fixed.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>

std::string slurp_file(std::filesystem::path filename) {
    std::ifstream f;
    f.open(filename);
    if (!f) {
        throw std::system_error(errno, std::system_category(), "can't open file '" + filename.string() + "'");
    }

    std::stringstream sstr;
    sstr << f.rdbuf();
    return sstr.str();
}

static std::filesystem::path source_dir_path{std::filesystem::path{__FILE__}.parent_path()};

/* decode strings like "30 апреля" as date::month_day.
 * Throw on error.
 */
date::month_day decode_month_day(std::string s) {
    static std::unordered_map<std::string, date::month> month_map{
        {"января", date::January}
        ,{"февраля", date::February}
        ,{"марта", date::March}
        ,{"апреля", date::April}
        ,{"мая", date::May}
        ,{"июня", date::June}
        ,{"июля", date::July}
        ,{"августа", date::August}
        ,{"сентября", date::September}
        ,{"октября", date::October}
        ,{"ноября", date::November}
        ,{"декабря", date::December}
    };
    std::string month_str;
    int day;
    {
        std::stringstream stream{s};
        stream >> day >> month_str;
    }
    auto iter = month_map.find(month_str);
    if (iter == month_map.end()) {
        throw std::domain_error("can't parse month_day string '" + s + "' (" + std::to_string(day) + " " + month_str + ")");
    }
    return iter->second / day;
}

using col_to_date = std::map<std::size_t, date::year_month_day>;

/* return map "column number => date" for table header */
col_to_date get_date_headers(html::Table & t, date::year year) {
    std::size_t col_count = t.get_row_length(0);

    std::map<std::size_t, date::year_month_day> map;

    auto first_cell_text = t.get(0, 0);
    for (std::size_t col=1; col < col_count; ++col) {
        auto cell_text = t.get(0, col);

        // skip all cells identical to the first one (॥ श्रीः ॥).
        if (cell_text == first_cell_text) continue;
        auto ymd = year / decode_month_day(cell_text);
        map[col] = ymd;
    }
    return map;
}

/* In: text from cell in pañcāṅgam, possibly including "such-and-such ekAdashI" in Russian language. E.g. "Варӯтӿинӣ экāдащӣ"
 * Detect whether known ekAdashI is described in this cell.
 * fail check on unknown ekAdashI name.
 * Return empty string if ekAdashI is not mentioned at all.
 * Return ekAdashI name otherwise;
 */
std::string get_ekadashi_name(const std::string & text) {
    std::regex regex{"([^\\s,.]+)[ -](э|Э)кāдащӣ"};
    std::smatch match;
    if (!std::regex_search(text, match, regex)) {
        return "";
    }
    std::string ekadashi_name = match[1].str();
    // require ekadashi_name to be known
    REQUIRE_THAT(ekadashi_name,
                 Catch::Predicate<const std::string &>(
                     [](const std::string & s) -> bool {
                        return std::find(vp::ekadashi_names_rus().begin(), vp::ekadashi_names_rus().end(), s) != vp::ekadashi_names_rus().end();
                     }
    ));
    return ekadashi_name;
}

TEST_CASE("get_ekadashi_name works") {
    REQUIRE(get_ekadashi_name("Варӯтӿинӣ экāдащӣ") == "Варӯтӿинӣ");
    REQUIRE(get_ekadashi_name(", Варӯтӿинӣ экāдащӣ, ") == "Варӯтӿинӣ");
}

// TODO: remove [[maybe_unused]] after completing this check.
size_t check_ekadashi([[maybe_unused]] const std::vector<std::string> & row, [[maybe_unused]] std::size_t col, [[maybe_unused]] const std::string & ekadashi_name) {
    return 0;
}

std::string join(const std::vector<std::string> & v, char joiner=';') {
    std::string joined;
    bool first = true;
    for (const auto & cell : v) {
        if (!first) {
            joined += joiner;
        }
        first = false;
        joined += cell;
    }
    return joined;
}

TEST_CASE("join() works") {
    REQUIRE("a;b" == join(std::vector<std::string>{"a", "b"}));
}

/* If indicated cell text represents vrata we know how to handle (ekAdashI), then check if our calculations give the same result
 * Returns number of cells handled.
 * It's three for atiriktA ekAdashI or atiriktA dvAdashI(vrata+atiriktA+pAraNam),
 * Two for normal case of single-day ekAdashI (vrata+pAraNam)
 * Zero if we didn't detect any vrata we would know how to handle.
 */
std::size_t check_vrata(const std::string & case_slug, const std::vector<std::string> & row, std::size_t col) {
    std::string ekadashi_name = get_ekadashi_name(row[col]);
    if (ekadashi_name.empty()) return 0;
    // first +1 is to compensate zero-based counting
    // second +1 means we require that there is at least one more column (for pAraNam).
    REQUIRE(row.size() >= col+1+1);
    std::size_t cells_handled = check_ekadashi(row, col, ekadashi_name);
    if (cells_handled < 2) {
        std::string row_str = join(row);
        REQUIRE_THAT(cells_handled,
                     Catch::Predicate<std::size_t>([](std::size_t cells_handled_) -> bool {
                         return cells_handled_ >= 2;
                     },
                     case_slug + ": can't handle ekAdashI at col " + std::to_string(col) + " of " + row_str
                     ));
    }
    return cells_handled;
}

/* Returns number of vratas handled in this row (should always be 1 for I haven't seen two ekAdashI vratas in a single table yet)
 */
std::size_t check_vratas_from_row(const std::string & case_slug, const std::vector<std::string> & row, const col_to_date & date_map) {
    // manual loop because occasionally we need to increment the iterator manually to skip some cells
    std::size_t vratas_handled = 0;
    for (auto iter = date_map.begin(); iter != date_map.end(); ++iter) {
        auto & col = iter->first;

        std::size_t handled_cells = check_vrata(case_slug, row, col);
        vratas_handled += (handled_cells > 0);
        // TODO: when handled_cells > 1, check if all correponding next dates in date_map are always one day after a previous one,
        // fail the test otherwise.
        for (; handled_cells > 2; --handled_cells) {
            ++iter;
            if (iter == date_map.end()) break;
        }
    }
    return vratas_handled;
}

void test_one_precalculated_case(const std::string & slug) {
    std::string filename{std::string{"data/precalculated-"} + slug + ".html"};
    auto s = slurp_file(source_dir_path / filename);
    REQUIRE(!s.empty());
    auto p = html::TableParser(std::move(s));
    auto t = p.next_table();
    REQUIRE(t.has_value());

    auto date_headers = get_date_headers(*t, date::year{2019});
    std::size_t row_count = t->row_count();
    // from row 1 because row 0 is date headers only
    for (size_t row=1; row < row_count; ++row) {
        auto & row_data = t->get_row(row);

        // skip header rows, they have colspan=3 in the beginning
        if (row_data[0] == row_data[1] && row_data[0] == row_data[2]) {
            continue;
        }

        // TODO: check that timezone in the table matches our data

        std::size_t ekadashi_etc_count = check_vratas_from_row(slug, row_data, date_headers);
        std::string row_str;
        bool first = true;
        for (const auto & cell : row_data) {
            if (!first) {
                row_str += ';';
            }
            first = false;
            row_str += cell;
        }
        REQUIRE_THAT(
                    ekadashi_etc_count,
                    Catch::Predicate<std::size_t>(
                        [] (std::size_t count) -> bool {
                            return count == 1;
                        },
                        slug + ", row " + std::to_string(row+1) + ": ekAdashI count should be strictly 1\n" + row_str
                    ));
        REQUIRE(ekadashi_etc_count == 1);
    }
}

TEST_CASE("precalculated ekAdashIs", "[!hide]") {
    test_one_precalculated_case(std::string{"2019-04-27"});
}

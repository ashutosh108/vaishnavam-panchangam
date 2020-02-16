#include "calc.h"
#include "html-parser.h"
#include "location.h"
#include "vrata.h"
#include "vrata_detail.h"

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

class Precalculated_Vrata {
    date::year_month_day date;
    std::string case_slug; // test case id (slug like 2019-04-27)
    vp::Location location;
public:
    Precalculated_Vrata(std::string case_slug_, vp::Location location_, date::year_month_day date_)
        : date(date_), case_slug(std::move(case_slug_)), location(location_) {}
    bool operator==(const vp::Vrata_Detail & other) const {
        return date == other.vrata.date;
    }
    friend std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v);
};

std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v) {
    return s << "case '" << v.case_slug << "' " << v.location.name << " date=" << v.date;
}


Precalculated_Vrata get_precalc_ekadashi(const std::string & case_slug, const vp::Location & location, [[maybe_unused]] html::Table::Row & row_data, [[maybe_unused]] std::size_t col, [[maybe_unused]] const std::string & ekadashi_name, date::year_month_day date) {
    // TODO: extract vrata type and pAraNam time.
    return Precalculated_Vrata{case_slug, location, date};
}

// TODO: remove [[maybe_unused]] after completing this check.
size_t check_ekadashi(const std::string & case_slug, const vp::Location & location, html::Table::Row & row_data, std::size_t col, const std::string & ekadashi_name, col_to_date & date_map) {
    auto date = date_map[col];
    auto precalc_vrata = get_precalc_ekadashi(case_slug, location, row_data, col, ekadashi_name, date);
    auto our_vrata = vp::Calc{location}.find_next_vrata(date);
    REQUIRE(our_vrata.has_value());
    auto our_vrata_detail = vp::Vrata_Detail{*our_vrata, location};
    REQUIRE(precalc_vrata == our_vrata_detail);
    return (our_vrata->is_two_days() ? 3 : 2);
}

std::string join(const html::Table::Row & v, char joiner=';') {
    std::string joined;
    bool first = true;
    for (const auto & [col, cell]: v) {
        if (!first) {
            joined += joiner;
        }
        first = false;
        joined += cell;
    }
    return joined;
}

TEST_CASE("join() works") {
    REQUIRE("a;b" == join(html::Table::Row{{1, "a"}, {2, "b"}}));
}

/* If indicated cell text represents vrata we know how to handle (ekAdashI), then check if our calculations give the same result
 * Returns number of cells handled.
 * It's three for atiriktA ekAdashI or atiriktA dvAdashI(vrata+atiriktA+pAraNam),
 * Two for normal case of single-day ekAdashI (vrata+pAraNam)
 * Zero if we didn't detect any vrata we would know how to handle.
 */
std::size_t check_vrata(const std::string & case_slug, const vp::Location & location, html::Table::Row & row_data, std::size_t col, col_to_date & date_map) {
    std::string ekadashi_name = get_ekadashi_name(row_data[col]);
    if (ekadashi_name.empty()) return 0;
    // first +1 is to compensate zero-based counting
    // second +1 means we require that there is at least one more column (for pAraNam).
    REQUIRE(row_data.size() >= col+1+1);
    std::size_t cells_handled = check_ekadashi(case_slug, location, row_data, col, ekadashi_name, date_map);
    if (cells_handled < 2) {
        std::string row_str = join(row_data);
        REQUIRE_THAT(cells_handled,
                     Catch::Predicate<std::size_t>([](std::size_t cells_handled_) -> bool {
                         return cells_handled_ == 2 || cells_handled_ == 3;
                     },
                     case_slug + ": " + std::to_string(cells_handled) + " cells handled (should have been 2 or 3) at col " + std::to_string(col) + " of " + row_str
                     ));
    }
    return cells_handled;
}

vp::Location find_location_by_name_rus(const std::string & name) {
    static std::vector<std::pair<const char *, vp::Location>> rus_locations {
        { "Odessa", vp::odessa_coord },
        { "Vinnitsa", vp::vinnitsa_coord },
        { "Kiev", vp::kiev_coord },
        { "San Francisco", vp::sanfrantsisko_coord },
        { "Tiraspol", vp::tiraspol_coord },
        { "Khmelnytskyi", vp::hmelnitskiy_coord },
        { "Kishinev", vp::kishinev_coord },
        { "Voronezh", vp::voronezh_coord },
        { "Kharkov", vp::harkov_coord },
        { "Khabarovsk", vp::habarovsk_coord },
        { "Lugansk", vp::lugansk_coord },
        { "Moscow", vp::moskva_coord },
        { "Vrindavan", vp::vrindavan_coord },
        { "Los Angeles", vp::losanjeles_coord },
        { "Kolkata", vp::kalkuta_coord },
        { "Saint Petersburg", vp::spb_coord },
        { "Freiburg im Breisgau", vp::freiburg_coord },
        { "Nikolaev", vp::nikolaev_coord },
        { "Ramenskoye, Moscow Oblast", vp::ramenskoe_m_obl_coord },
        { "Minsk", vp::minsk_coord },
        { "Barnaul", vp::barnaul_coord },
        { "New Delhi", vp::newdelhi_coord },
        { "Dusseldorf", vp::dusseldorf_coord },
        { "Cologne", vp::koeln_kkd_coord },
        { "Sochi", vp::sochi_coord },
        { "Velikiy Novgorod", vp::novgorod_coord },
        { "London", vp::london_coord },
        { "Manchester", vp::manchester_coord },
        { "Panaji", vp::panaji_coord },
        { "Mumbai", vp::bombey_coord },
        { "Pune", vp::pune_coord },
        { "Simferopol", vp::simferopol_coord },
        { "Manali, Himachal Pradesh", vp::manali_coord },
        { "Pyatigorsk", vp::pyatigorsk_coord },
        { "Kirov", vp::kirov_coord },
        { "Washington, D.C.", vp::washington_coord },
        { "Gokarna, Karnataka", vp::gokarna_coord },
        { "Tel Aviv", vp::telaviv_coord },
        { "Tomsk", vp::tomsk_coord },
        { "Kiev", vp::kiel_coord },
        { "Omsk", vp::omsk_coord },
        { "Tashkent", vp::tashkent_coord },
        { "Удупи", vp::udupi_coord },
        { "Варшава", vp::varshava_coord },
        { "Donetsk", vp::donetsk_coord },
        { "Tbilisi", vp::tbilisi_coord },
        { "Sukhum", vp::suhum_coord },
        { "Kremenchug", vp::kremenchug_coord },
        { "Puno", vp::puno_coord },
        { "Vladivostok", vp::vladivostok_coord },
        { "Pernem", vp::pernem_coord },
        { "Krasnodar", vp::krasnodar_coord },
        { "Meadow Lake, Canada", vp::meadowlake_coord },
        { "Toronto", vp::toronto_coord },
        { "Fredericton", vp::frederikton_coord },
        { "Perm", vp::perm_coord },
        { "Ufa", vp::ufa_coord },
        { "Smolensk", vp::smolensk_coord },
        { "Krivoy Rog", vp::krivoyrog_coord },
        { "Petropavlovsk-Kamchatsky", vp::petropavlovskkamchatskiy_coord },
        { "Ko Pha-ngan", vp::kophangan_coord },
        { "Denpasar", vp::denpasar_coord },
        { "Mundelein", vp::mundelein_coord },
        { "Bishkek", vp::bishkek_coord },
        { "Vienna", vp::vena_coord },
        { "Stary Oskol", vp::staryyoskol_coord },
        { "Edmonton", vp::edmonton_coord },
        { "Novosibirsk", vp::novosibirsk_coord },
        { "Yerevan", vp::erevan_coord },
        { "Stavropol", vp::stavropol_coord },
        { "Pokhara", vp::pokhara_coord },
        { "Murmansk", vp::murmansk_coord },
        { "Mirny, Sakha Republic", vp::mirnyy_coord },
        { "Riga", vp::riga_coord },
        { "Surgut", vp::surgut_coord },
        { "Ryazan", vp::ryazan_coord },
        { "Athens", vp::afiny_coord },
        { "Chita", vp::chita_coord },
        { "Poltava", vp::poltava_coord },
        { "Kazan", vp::kazan_coord },
        { "Aktau", vp::aktau_coord },
        { "Tallinn", vp::tallin_coord },
        { "Jurmala", vp::yurmala_coord },
        { "Semikarakorsk", vp::semikarakorsk_coord },
        { "Colombo", vp::kolombo_coord },
        { "Ulyanovsk", vp::ulyanovsk_coord },
        { "Tagbilaran", vp::tagbilaran_coord },
        { "Gomel", vp::gomel_coord },
        { "Yekaterinburg", vp::ekaterinburg_coord },
        { "Vilnius", vp::vilnyus_coord },
        { "Kostomuksha", vp::kostomuksha_coord },
        { "Almaty", vp::almaata_coord },
        { "Kolomyia", vp::kolomyya_coord },
        { "Samara", vp::samara_coord },
        { "Chelyabinsk", vp::chelyabinsk_coord },
        { "Tekeli, Kazakhstan", vp::tekeli_coord },
        { "Volgograd", vp::volgograd_coord },
        { "Tambov", vp::tambov_coord },
        { "Marseille", vp::marsel_coord },
        { "Barcelona", vp::barcelona_coord },
        { "Madrid", vp::madrid_coord },
        { "Miami", vp::miami_coord }
    };
    auto iter = std::find_if(rus_locations.begin(), rus_locations.end(), [&name](const auto & pair) {
        return pair.first == name;
    });
    if (iter == rus_locations.end()) {
        throw std::runtime_error("location '" + name + "' is not known in test data, aborting");
    }
    return iter->second;
}

/* Returns number of vratas handled in this row (should always be 1 for I haven't seen two ekAdashI vratas in a single table yet)
 */
std::size_t check_vratas_from_row(const std::string & case_slug, html::Table::Row & row, col_to_date & date_map) {
    try {
        vp::Location location = find_location_by_name_rus(row[2]);

        // manual loop because occasionally we need to increment the iterator manually to skip some cells
        std::size_t vratas_handled = 0;
        for (auto iter = date_map.begin(); iter != date_map.end(); ++iter) {
            auto & col = iter->first;

            std::size_t handled_cells = check_vrata(case_slug, location, row, col, date_map);
            vratas_handled += (handled_cells > 0);
            // TODO: when handled_cells > 1, check if all correponding next dates in date_map are always one day after a previous one,
            // fail the test otherwise.

            // skip all extra processed cells (beyond 1 which will be skipped as part of standard raw loop)
            for (; handled_cells > 1; --handled_cells) {
                ++iter;
                if (iter == date_map.end()) break;
            }
        }
        return vratas_handled;
    } catch(...) {
        Catch::cerr() << case_slug << ", " << join(row) << '\n';
        throw;
    }
}

void test_one_precalculated_table(const std::string & slug) {
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

        REQUIRE_THAT(row_data.size(),
                     Catch::Predicate<std::size_t>(
                         [](std::size_t size) -> bool {
                            return size > 3;
                         },
                         slug + ": row " + std::to_string(row) + ": size(" + std::to_string(row_data.size()) + ") must be > 3"
                         ));

        // skip header rows, they have colspan=3 in the beginning
        if (row_data[0] == row_data[1] && row_data[0] == row_data[2]) {
            continue;
        }

        // TODO: check that timezone in the table matches our data

        std::size_t ekadashi_etc_count = check_vratas_from_row(slug, row_data, date_headers);
        std::string row_str = join(row_data);
        REQUIRE_THAT(
                    ekadashi_etc_count,
                    Catch::Predicate<std::size_t>(
                        [] (std::size_t count) -> bool {
                            return count == 1;
                        },
                        slug + ", row " + std::to_string(row+1) + ": ekAdashI count should be strictly 1\n" + row_str
                    ));
    }
}

TEST_CASE("precalculated ekAdashIs", "[!hide]") {
    test_one_precalculated_table(std::string{"2019-04-27"});
}

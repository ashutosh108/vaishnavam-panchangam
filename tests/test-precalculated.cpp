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

std::string slurp_file(const std::filesystem::path & filename) {
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
date::month_day decode_month_day(const std::string& s) {
    static std::unordered_map<std::string, date::month> month_map{
        {"января", date::January},
        {"февраля", date::February},
        {"марта", date::March},
        {"апреля", date::April},
        {"мая", date::May},
        {"июня", date::June},
        {"июля", date::July},
        {"августа", date::August},
        {"сентября", date::September},
        {"октября", date::October},
        {"ноября", date::November},
        {"декабря", date::December}
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

template <class Container, class T>
bool contains(Container & c, T & val) {
    return std::find(c.begin(), c.end(), val) != c.end();
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
    CAPTURE(text, ekadashi_name);
    // require ekadashi_name to be known
    REQUIRE(contains(vp::ekadashi_names_rus(), ekadashi_name));
    return ekadashi_name;
}

TEST_CASE("get_ekadashi_name works") {
    REQUIRE(get_ekadashi_name("Варӯтӿинӣ экāдащӣ") == "Варӯтӿинӣ");
    REQUIRE(get_ekadashi_name(", Варӯтӿинӣ экāдащӣ, ") == "Варӯтӿинӣ");
}

struct Precalculated_Vrata {
    date::year_month_day date;
    vp::Location location;
    Precalculated_Vrata(vp::Location location_, date::year_month_day date_)
        : date(date_), location(location_) {}
    bool operator==(const vp::Vrata_Detail & other) const {
        return date == other.vrata.date && location == other.location;
    }
    friend std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v);
};

bool operator==(const Precalculated_Vrata & one, const Precalculated_Vrata & other) {
    return one.date == other.date && one.location == other.location;
}

std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v) {
    return s << v.location.name << " date=" << v.date;
}


Precalculated_Vrata get_precalc_ekadashi(const vp::Location & location, [[maybe_unused]] html::Table::Row & row_data, [[maybe_unused]] std::size_t col, [[maybe_unused]] const std::string & ekadashi_name, date::year_month_day date) {
    // TODO: extract vrata type and pAraNam time.
    return Precalculated_Vrata{location, date};
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

/* Try extracting vrata data from indicated cell.
 */
std::optional<Precalculated_Vrata> try_extract_vrata_from_cell(const vp::Location & location, html::Table::Row & row_data, std::size_t col, col_to_date & date_map) {
    std::string ekadashi_name = get_ekadashi_name(row_data[col]);
    if (ekadashi_name.empty()) return std::nullopt;
    return get_precalc_ekadashi(location, row_data, col, ekadashi_name, date_map[col]);
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

/* Extracts single vrata from a row
 */
Precalculated_Vrata extract_vrata_from_row(html::Table::Row & row, col_to_date & date_map) {
    vp::Location location = find_location_by_name_rus(row[2]);

    // manual loop because occasionally we need to increment the iterator manually to skip some cells
    for (auto iter = date_map.begin(); iter != date_map.end(); ++iter) {
        auto & col = iter->first;

        std::optional<Precalculated_Vrata> vrata = try_extract_vrata_from_cell(location, row, col, date_map);
        if (vrata)
            return *vrata;
    }
    CAPTURE(row, date_map);
    throw std::runtime_error("can't extract vrata from row");
}

date::year get_year_from_slug(const char * slug) {
    std::istringstream s{slug};
    date::year year{};
    s >> date::parse("%Y", year);
    return year;
}

std::vector<Precalculated_Vrata> extract_vratas_from_precalculated_table(std::string && s, date::year year) {
    auto p = html::TableParser(std::move(s));
    auto t = p.next_table();
    if (!t) throw std::runtime_error("can't parse table");
    std::vector<Precalculated_Vrata> vratas;

    auto date_headers = get_date_headers(*t, year);
    std::size_t row_count = t->row_count();
    // from row 1 because row 0 is date headers only
    for (size_t row=1; row < row_count; ++row) {
        CAPTURE(row);
        auto & row_data = t->get_row(row);
        CAPTURE(join(row_data));
        REQUIRE(row_data.size() > 3);

        // skip header rows, they have colspan=3 in the beginning
        if (row_data[0] == row_data[1] && row_data[0] == row_data[2]) {
            continue;
        }

        // TODO: check that timezone in the table matches our data (move to extract_vrata?)

        vratas.push_back(extract_vrata_from_row(row_data, date_headers));
    }
    return vratas;
}

// TODO: check that we extract all relevant info in cases of:
// 1. standard ekAdashI with standard pAraNam
// 2. standard ekAdashI with "start-end" pAraNam
// 3. standard ekAdashI with "> start" pAraNam
// 4. standard ekAdashI with "< end" pAraNam
// 5-12. same four cases for atirikA ekAdashI and atiriktA dvAdashI
TEST_CASE("we capture all relevant data from tables") {
    using namespace date;
    using namespace vp;
    auto vratas = extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>*", 2019_y);
    REQUIRE(vratas.size() == 1);
    Precalculated_Vrata expected{vp::udupi_coord, 2019_y/January/1};
    REQUIRE(expected == vratas[0]);
}

void check_precalculated_vrata(const Precalculated_Vrata & vrata) {
    auto our_vrata = vp::Calc{vrata.location}.find_next_vrata(vrata.date);
    REQUIRE(our_vrata.has_value());
    auto our_vrata_detail = vp::Vrata_Detail{*our_vrata, vrata.location};
    REQUIRE(vrata == our_vrata_detail);
}

void check_precalculated_vratas(const std::vector<Precalculated_Vrata> & vratas) {
    for (const auto & v : vratas) {
        check_precalculated_vrata(v);
    }
}

void test_one_precalculated_table_slug(const char * slug) {
    CAPTURE(slug);

    std::string filename{std::string{"data/precalculated-"} + slug + ".html"};
    auto s = slurp_file(source_dir_path / filename);
    REQUIRE(!s.empty());

    auto year = get_year_from_slug(slug);
    //sanity check
    REQUIRE(year >= date::year{2000});
    REQUIRE(year < date::year{2030});

    std::vector<Precalculated_Vrata> vratas = extract_vratas_from_precalculated_table(std::move(s), year);
    check_precalculated_vratas(vratas);
}

TEST_CASE("precalculated ekAdashIs", "[!hide]") {
    test_one_precalculated_table_slug("2019-04-27");
}

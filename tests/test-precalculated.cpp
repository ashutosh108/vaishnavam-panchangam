#include "calc.h"
#include "html-table-parser.h"
#include "location.h"
#include "vrata.h"
#include "vrata_detail.h"

#include "catch.hpp"
#include <cstdlib>
#include "date-fixed.h"
#include "filesystem-fixed.h"
#include <fstream>
#include <map>
#include <regex>
#include <sstream>

std::string slurp_file(const fs::path & filename) {
    std::ifstream f;
    f.open(filename);
    if (!f) {
        throw std::system_error(errno, std::system_category(), "can't open file '" + filename.string() + "'");
    }

    std::stringstream sstr;
    sstr << f.rdbuf();
    return sstr.str();
}

static fs::path source_dir_path{fs::path{__FILE__}.parent_path()};

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
        {"іюня", date::June},
        {"іюля", date::July},
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
    CAPTURE(col_count);

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

std::ostream & operator<<(std::ostream & s, const std::optional<date::sys_seconds> & t) {
    if (t) {
        s << date::format("%Y-%m-%d %H:%M:%S %Z", *t);
    } else {
        s << "unspecified";
    }
    return s;
}

std::string to_str(const std::optional<date::sys_seconds> t) {
    std::stringstream s;
    if (t) {
        s << date::format("%Y-%m-%d %H:%M:%S %Z", *t);
    } else {
        s << "unspecified";
    }
    return s.str();
}

struct Precalculated_Vrata {
    date::year_month_day date;
    vp::Vrata_Type type;
    vp::Location location;
    std::optional<date::sys_seconds> paranam_start;
    std::optional<date::sys_seconds> paranam_end;
    bool skip = false;
    bool already_fixed = false;
    Precalculated_Vrata(vp::Location location_,
                        date::year_month_day date_,
                        vp::Vrata_Type type_ = vp::Vrata_Type::Ekadashi,
                        std::optional<date::sys_seconds> paranam_start_ = std::nullopt,
                        std::optional<date::sys_seconds> paranam_end_ = std::nullopt)
        : date(date_),
          type(type_),
          location(location_),
          paranam_start(paranam_start_),
          paranam_end(paranam_end_) {}
    bool operator==(const vp::Vrata_Detail & other) const {
        UNSCOPED_INFO("comparing: " << date << "<=>" << other.vrata.date << ";\n"
                      << location.name << "<=>" << other.location.name << ";\n"
                      << type << "<=>" << other.vrata.type << ";\n"
                      << to_str(paranam_start) << "<=>" << other.vrata.paran.paran_start << ";\n"
                      << to_str(paranam_end) << "<=>" << other.vrata.paran.paran_end);
        if (date != other.vrata.date || location != other.location) {
            UNSCOPED_INFO("dates and locations must match, but they don't");
            return false;
        }
        // allow mismatch between vrata types when precalc is Ekadashi and now-calc is Sandigdha_Ekadashi
        if (type != other.vrata.type && !(type==vp::Vrata_Type::Ekadashi && other.vrata.type == vp::Vrata_Type::Sandigdha_Ekadashi)) {
            UNSCOPED_INFO("vrata types must match, but they don't");
            return false;
        }
        if (other.vrata.paran.type == vp::Paran::Type::Standard) {
            UNSCOPED_INFO("paranam_start=" << to_str(paranam_start) << ", end=" << to_str(paranam_end));
            if (paranam_start) {
                UNSCOPED_INFO("in case of standard paranam, start time must not be set");
            }
            if (paranam_end) {
                UNSCOPED_INFO("in case of standard paranam, end time must not be set");
            }
            return !paranam_start && !paranam_end;
        }
        // it must be ">HH:MM" form, check with 1-min precision
        if (other.vrata.paran.type == vp::Paran::Type::From_Quarter_Dvadashi) {
            UNSCOPED_INFO("paran_type: From_Quarter_Dvadashi");
            // ">HH:MM" must have start and must NOT have end time
            if (!paranam_start) {
                UNSCOPED_INFO("pAraNam start must be set, but it is not");
                return false;
            }
            if (paranam_end) {
                UNSCOPED_INFO("pAraNam end must NOT be set, but it is set");
                return false;
            }

            if (!other.vrata.paran.paran_start) {
                UNSCOPED_INFO("now-calculated pAraNam must have start time");
                return false;
            }

            auto other_rounded = other.vrata.paran.paran_start->round_to_minute_up();
            bool result = paranam_start.value() == other_rounded;
            if (!result) {
                UNSCOPED_INFO("precaltulated pAraNam start should equal to now-calculated one, but it is not");
            }
            return result;
        }
        if (other.vrata.paran.type == vp::Paran::Type::Puccha_Dvadashi || other.vrata.paran.type == vp::Paran::Type::Until_Dvadashi_End) {
            UNSCOPED_INFO("paran type: " << other.vrata.paran.type);
            // "<HH:MM" might have start time and MUST have end time
            if (!paranam_end) {
                UNSCOPED_INFO("pAraNam end must be set");
                return false;
            }

            if (!other.vrata.paran.paran_end) {
                UNSCOPED_INFO("now-calculated pAraNam must have end time");
                return false;
            }

            auto other_rounded = other.vrata.paran.paran_end->round_to_minute_down();
            bool result = *paranam_end == other_rounded;
            if (!result) {
                UNSCOPED_INFO("pAraNam end must match, but it doesn't");
            }
            return result;
        }
        UNSCOPED_INFO("paranam comparison: start " << to_str(paranam_start) << " <=> " << date::format("%F %T %Z", other.vrata.paran.paran_start->round_to_minute_up())
                      << "\nend " << to_str(paranam_end) << " <=> " << date::format("%F %T %Z", other.vrata.paran.paran_end->round_to_minute_down()));
        UNSCOPED_INFO("end result: " << (paranam_start == other.vrata.paran.paran_start.value().round_to_minute_up() &&
                                         paranam_end == other.vrata.paran.paran_end.value().round_to_minute_down()));
        bool start_matches = paranam_start == other.vrata.paran.paran_start.value().round_to_minute_up();
        bool end_matches = paranam_end == other.vrata.paran.paran_end.value().round_to_minute_down();
        if (!start_matches) {
            UNSCOPED_INFO("pAraNam start does not match");
        }
        if (!end_matches) {
            UNSCOPED_INFO("pAraNam end does not match");
        }
        return start_matches && end_matches;
    }
    friend std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v);
};

bool operator==(const Precalculated_Vrata & one, const Precalculated_Vrata & other) {
    return one.date == other.date && one.location == other.location
            && one.type == other.type
            && one.paranam_start == other.paranam_start
            && one.paranam_end == other.paranam_end;
}

std::ostream & operator<<(std::ostream & s, const Precalculated_Vrata & v) {
    s << v.type << "@" << v.location.name << " on " << v.date;
    auto z = date::locate_zone(v.location.timezone_name);
    s << ", pAraNam: ";
    if (v.paranam_start) {
        s << date::make_zoned(z, *v.paranam_start);
    } else {
        s << "unspecified";
    }
    s << "..";
    if (v.paranam_end) {
         s << date::make_zoned(z, *v.paranam_end);
    } else {
        s << "unspecified";
    }
    return s;
}

std::chrono::seconds h_m_s_from_string(const std::string & s) {
    using namespace std::chrono_literals;
    std::istringstream stream{s};
    std::chrono::seconds h_m_s;
    stream >> date::parse("%H:%M", h_m_s);
    if (!stream.good()) {
        throw std::runtime_error{"can't parse '" + s + "' as HH:MM"};
    }

    // if there are more characters to read, they must be ":SS"
    if (stream.rdbuf()->in_avail() != 0) {
        std::chrono::seconds sec;
        stream >> date::parse(":%S", sec);
        if (!stream.good() || stream.rdbuf()->in_avail() != 0) {
            throw std::runtime_error{"can't parse '" + s + "' as HH:MM:SS"};
        }
        h_m_s += sec;
    }
    if (h_m_s >= 24h) {
        throw std::runtime_error("HH:MM[:SS] is 24 hours or more: '" + s + "'");
    }
    return h_m_s;
}

TEST_CASE("h_m_from_string works for basic cases") {
    using namespace std::string_literals;
    using namespace std::chrono;
    REQUIRE(hours{0} + minutes{0} == h_m_s_from_string("0:00"s));
    REQUIRE(hours{0} + minutes{0} == h_m_s_from_string("00:00"s));
    REQUIRE(hours{23} + minutes{45} == h_m_s_from_string("23:45"s));
    REQUIRE(hours{23} + minutes{59} == h_m_s_from_string("23:59"s));
    REQUIRE(hours{23} + minutes{45} == h_m_s_from_string("23:45:00"s));
    REQUIRE(23h + 45min + 15s == h_m_s_from_string("23:45:15"s));
    REQUIRE(23h + 45min + 59s == h_m_s_from_string("23:45:59"s));
    REQUIRE(hours{23} + minutes{59} == h_m_s_from_string("23:59"s));
    // 24hours or more should throw:
    REQUIRE_THROWS(h_m_s_from_string("24:00"));
    REQUIRE_THROWS(h_m_s_from_string("36:15"));
}

std::pair<std::optional<date::sys_seconds>, std::optional<date::sys_seconds>> parse_precalc_paranam(std::string s, date::year_month_day date, const char * timezone_name) {
    using namespace date;
    std::smatch match;
    // "*" alone or "*;" followed by other descriptions
    if (std::regex_search(s, match, std::regex{R"~(^\*($|[;,]\s+))~"})) {
        return {{}, {}};
    } else if (std::regex_search(s, match, std::regex{R"regex(^(?:!\s+)?(\d?\d:\d\d(?::\d\d)?)\s*(?:-|—)\s*(\d?\d:\d\d(?::\d\d)?)$)regex"})) {
        if (match.size() >= 2) {
            auto start_h_m_s{h_m_s_from_string(match[1].str())};
            auto end_h_m_s{h_m_s_from_string(match[2].str())};
            auto time_zone = date::locate_zone(timezone_name);
            auto start_zoned = date::make_zoned(time_zone, date::local_days(date) + start_h_m_s);
            auto end_zoned = date::make_zoned(time_zone, date::local_days(date) + end_h_m_s);
            return {start_zoned.get_sys_time(), end_zoned.get_sys_time()};
        }
    } else if (std::regex_search(s, match, std::regex{R"~(&gt;\s*(\d?\d:\d\d))~"})) {
        if (match.size() >= 1) {
            auto start_h_m_s{h_m_s_from_string(match[1].str())};
            auto time_zone = date::locate_zone(timezone_name);
            auto start_zoned = date::make_zoned(time_zone, date::local_days(date) + start_h_m_s);
            vp::JulDays_UT start_time{date::local_days(date)+start_h_m_s, time_zone};
            return {start_zoned.get_sys_time(), std::nullopt};
        }
    } else if (std::regex_search(s, match, std::regex{R"~(&lt;\s*(\d?\d:\d\d))~"})) {
        if (match.size() >= 1) {
            auto end_h_m_s{h_m_s_from_string(match[1].str())};
            auto time_zone = date::locate_zone(timezone_name);
            auto end_zoned = date::make_zoned(time_zone, date::local_days(date) + end_h_m_s);
            return {std::nullopt, end_zoned.get_sys_time()};
        }
    }
    throw std::runtime_error("can't parse paran time '" + s + "'");
}

bool is_atirikta(std::string cell_text, /*out*/ vp::Vrata_Type & type) {
    if (cell_text.find("Атириктā экāдащӣ") != std::string::npos) {
        type = vp::Vrata_Type::Atirikta_Ekadashi;
        return true;
    } else if (cell_text.find("Атириктā двāдащӣ") != std::string::npos) {
        type = vp::Vrata_Type::With_Atirikta_Dvadashi;
        return true;
    }
    return false;
}

Precalculated_Vrata get_precalc_ekadashi(const vp::Location & location, [[maybe_unused]] html::Table::Row & row_data, [[maybe_unused]] std::size_t col, [[maybe_unused]] const std::string & ekadashi_name, date::year_month_day date) {
    // TODO: shravanA dvAdashI
    // TODO: joined ekAdashI/atiriktA cells
    vp::Vrata_Type type {vp::Vrata_Type::Ekadashi};
    std::optional<date::sys_seconds> paranam_start;
    std::optional<date::sys_seconds> paranam_end;
    if (is_atirikta(row_data[col+1], type)) {
        date::year_month_day day3{date::sys_days(date) + date::days{2}};
        std::tie(paranam_start, paranam_end) = parse_precalc_paranam(row_data[col+2], day3, location.timezone_name);
    } else {
        date::year_month_day day2{date::sys_days(date) + date::days{1}};
        std::tie(paranam_start, paranam_end) = parse_precalc_paranam(row_data[col+1], day2, location.timezone_name);
    }
    return Precalculated_Vrata{location, date, type, paranam_start, paranam_end};
}

std::string join(const html::Table::Row & v, char joiner=';') {
    std::string joined;
    bool first_string = true;
    for (const auto & pair : v) {
        if (!first_string) {
            joined += joiner;
        }
        first_string = false;
        joined += pair.second;
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
        { "Одесса", vp::odessa_coord },
        { "Vinnitsa", vp::vinnitsa_coord },
        { "Киев", vp::kiev_coord },
        { "San Francisco", vp::sanfrantsisko_coord },
        { "Tiraspol", vp::tiraspol_coord },
        { "Khmelnytskyi", vp::hmelnitskiy_coord },
        { "Кишинев", vp::kishinev_coord },
        { "Воронеж", vp::voronezh_coord },
        { "Харьков", vp::harkov_coord },
        { "Хабаровск", vp::habarovsk_coord },
        { "Lugansk", vp::lugansk_coord },
        { "Москва", vp::moskva_coord },
        { "Vrindavan", vp::vrindavan_coord },
        { "Los Angeles", vp::losanjeles_coord },
        { "Колката", vp::kalkuta_coord },
        { "Душанбе", vp::dushanbe_coord },
        { "Санкт-Петербург", vp::spb_coord },
        { "Freiburg im Breisgau", vp::freiburg_coord },
        { "Николаев", vp::nikolaev_coord },
        { "Ramenskoye, Moscow Oblast", vp::ramenskoe_m_obl_coord },
        { "Минск", vp::minsk_coord },
        { "Барнаул", vp::barnaul_coord },
        { "Нью-Дели", vp::newdelhi_coord },
        { "Dusseldorf", vp::dusseldorf_coord },
        { "Cologne", vp::koeln_kkd_coord },
        { "Sochi", vp::sochi_coord },
        { "Velikiy Novgorod", vp::novgorod_coord },
        { "Лондон", vp::london_coord },
        { "Manchester", vp::manchester_coord },
        { "Panaji", vp::panaji_coord },
        { "Mumbai", vp::bombey_coord },
        { "Pune", vp::pune_coord },
        { "Симферополь", vp::simferopol_coord },
        { "Манали", vp::manali_coord },
        { "Пятигорск", vp::pyatigorsk_coord },
        { "Киров", vp::kirov_coord },
        { "Washington, D.C.", vp::washington_coord },
        { "Гокарна", vp::gokarna_coord },
        { "Tel Aviv", vp::telaviv_coord },
        { "Томск", vp::tomsk_coord },
        { "Kiev", vp::kiel_coord },
        { "Омск", vp::omsk_coord },
        { "Tashkent", vp::tashkent_coord },
        { "Удупи", vp::udupi_coord },
        { "Варшава", vp::varshava_coord },
        { "Донецк", vp::donetsk_coord },
        { "Тбилиси", vp::tbilisi_coord },
        { "Sukhum", vp::suhum_coord },
        { "Кременчуг", vp::kremenchug_coord },
        { "Puno", vp::puno_coord },
        { "Владивосток", vp::vladivostok_coord },
        { "Pernem", vp::pernem_coord },
        { "Краснодар", vp::krasnodar_coord },
        { "Meadow Lake", vp::meadowlake_coord },
        { "Торонто", vp::toronto_coord },
        { "Фредериктон", vp::fredericton_coord },
        { "Пермь", vp::perm_coord },
        { "Уфа", vp::ufa_coord },
        { "Смоленск", vp::smolensk_coord },
        { "Кривой Рог", vp::krivoyrog_coord },
        { "Петропавловск-Камчатскій", vp::petropavlovskkamchatskiy_coord },
        { "Ко Пха Нган Ко Самуи", vp::kophangan_coord },
        { "Денпасар", vp::denpasar_coord },
        { "Mundelein", vp::mundelein_coord },
        { "Бишкек", vp::bishkek_coord },
        { "Вѣна", vp::vena_coord },
        { "Старый Оскол", vp::staryyoskol_coord },
        { "Edmonton", vp::edmonton_coord },
        { "Новосибирск", vp::novosibirsk_coord },
        { "Ереван", vp::erevan_coord },
        { "Ставрополь", vp::stavropol_coord },
        { "Pokhara", vp::pokhara_coord },
        { "Мурманск", vp::murmansk_coord },
        { "Мирный", vp::mirnyy_coord },
        { "Рига", vp::riga_coord },
        { "Сургут", vp::surgut_coord },
        { "Рязань", vp::ryazan_coord },
        { "Athens", vp::afiny_coord },
        { "Chita", vp::chita_coord },
        { "Полтава", vp::poltava_coord },
        { "Казань", vp::kazan_coord },
        { "Актау", vp::aktau_coord },
        { "Таллин", vp::tallin_coord },
        { "Юрмала", vp::jurmala_coord },
        { "Семикаракорск", vp::semikarakorsk_coord },
        { "Colombo", vp::kolombo_coord },
        { "Ульяновск", vp::ulyanovsk_coord },
        { "Tagbilaran", vp::tagbilaran_coord },
        { "Гомель", vp::gomel_coord },
        { "Екатеринбург", vp::ekaterinburg_coord },
        { "Вильнюс", vp::vilnyus_coord },
        { "Костомукша", vp::kostomuksha_coord },
        { "Алма-Ата", vp::almaata_coord },
        { "Коломыя", vp::kolomyya_coord },
        { "Самара", vp::samara_coord },
        { "Челябинск", vp::chelyabinsk_coord },
        { "Текели", vp::tekeli_coord },
        { "Волгоград", vp::volgograd_coord },
        { "Тамбов", vp::tambov_coord },
        { "Марсель", vp::marsel_coord },
        { "Барселона", vp::barcelona_coord },
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

/* Extracts single vrata from a row */
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
    INFO("extract_vratas_from_precalculated_table()")

    auto date_headers = get_date_headers(*t, year);
    CAPTURE(date_headers);
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

TEST_CASE("do not allow empty paran type cell") {
    REQUIRE_THROWS_AS(
                extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>", date::year{2019}),
                std::runtime_error);
}

TEST_CASE("precalc parsing: 1. standard ekAdashI with standard pAraNam") {
    using namespace date;
    auto vratas = extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>*", 2019_y);
    REQUIRE(vratas.size() == 1);
    Precalculated_Vrata expected{vp::udupi_coord, 2019_y/January/1};
    REQUIRE(expected == vratas[0]);
}

TEST_CASE("precalc parsing: 2. standard ekAdashI with 'start-end' pAraNam") {
    using namespace date;
    auto vratas = extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>6:07 - 6:08", 2019_y);
    REQUIRE(vratas.size() == 1);
    // 5:30 is indian timezone shift from UTC
    date::sys_seconds paran_start{date::sys_days(2019_y/January/2) + std::chrono::hours{6} + std::chrono::minutes{7} - std::chrono::minutes{5*60+30}};
    date::sys_seconds paran_end{date::sys_days(2019_y/January/2) + std::chrono::hours{6} + std::chrono::minutes{8} - std::chrono::minutes{5*60+30}};
    Precalculated_Vrata expected{vp::udupi_coord, 2019_y/January/1, vp::Vrata_Type::Ekadashi, paran_start, paran_end};
    REQUIRE(expected == vratas[0]);
}

TEST_CASE("precalc parsing: 3. standard ekAdashI with '> start' pAraNam") {
    using namespace date;
    auto vratas = extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>&gt;6:07", 2019_y);
    REQUIRE(vratas.size() == 1);
    // 5:30 is indian timezone shift from UTC
    date::sys_seconds paran_start{date::sys_days(2019_y/January/2) + std::chrono::hours{6} + std::chrono::minutes{7} - std::chrono::minutes{5*60+30}};
    Precalculated_Vrata expected{vp::udupi_coord, 2019_y/January/1, vp::Vrata_Type::Ekadashi, paran_start, {}};
    REQUIRE(expected == vratas[0]);
}

TEST_CASE("precalc parsing: 4. standard ekAdashI with '< end' pAraNam") {
    using namespace date;
    auto vratas = extract_vratas_from_precalculated_table(
                "<table><td><td><td><td>1 января<td>2 января"
                "<tr><td><td><td>Удупи<td>Варӯтӿинӣ экāдащӣ<td>&lt;6:08", 2019_y);
    REQUIRE(vratas.size() == 1);
    // 5:30 is indian timezone shift from UTC
    date::sys_seconds paran_end{date::sys_days(2019_y/January/2) + std::chrono::hours{6} + std::chrono::minutes{8} - std::chrono::minutes{5*60+30}};
    Precalculated_Vrata expected{vp::udupi_coord, 2019_y/January/1, vp::Vrata_Type::Ekadashi, {}, paran_end};
    REQUIRE(expected == vratas[0]);
}

void check_precalculated_vrata(const Precalculated_Vrata & vrata) {
    CAPTURE(vrata);
    auto our_vrata = vp::Calc{vrata.location}.find_next_vrata(vrata.date);
    REQUIRE(our_vrata.has_value());
    auto our_vrata_detail = vp::Vrata_Detail{*our_vrata, vrata.location};
    REQUIRE(vrata == our_vrata_detail);
}

void check_precalculated_vratas(const std::vector<Precalculated_Vrata> & vratas) {
    for (const auto & v : vratas) {
        if (v.skip) continue;
        check_precalculated_vrata(v);
    }
}

enum class Fix {
    ParanStartTime, ParanEndTime, Skip,
    AddMinutesToParanStartTimeIfExists,
    AddMinutesToParanEndTimeIfExists,
    ParanDate,
};

struct FixItem {
    Fix type;
    std::string from;
    std::string to;
};

using Fixes = std::map<vp::Location, std::vector<FixItem>>;

date::local_time<std::chrono::seconds> local_from_y_m_d_h_m(const std::string & y_m_d_h_m) {
    std::istringstream s{y_m_d_h_m};
    date::local_time<std::chrono::seconds> local_time;
    s >> date::parse("%Y-%m-%d %H:%M", local_time);
    if (!s.good()) {
        throw std::runtime_error("can't parse " + y_m_d_h_m + " as local time");
    }
    return local_time;
}

enum class Round { Up, Down };

typedef void (vp::JulDays_UT::*Rounder)(void);

void replace_time(std::optional<date::sys_seconds> & time, const std::string & from_str, const std::string & to_str, const char * timezone_name) {
    if (!time && from_str != "unspecified") {
        throw std::runtime_error("can't fix time from " + from_str + " to " + to_str + ": it's empty");
    }
    auto time_zone = date::locate_zone(timezone_name);
    if (from_str == "unspecified") {
        if (time) {
            std::stringstream s;
            s << "can't fix time from " << from_str << " to " << to_str << ": it's not unspecified but is " << date::format("%Y-%m-%d %T %Z", *time) << " instead";
            throw std::runtime_error(s.str());
        }
    } else {
        auto existing_local = date::make_zoned(time_zone, time.value()).get_local_time();
        auto expected_local = date::floor<date::days>(existing_local) + h_m_s_from_string(from_str);
        if (existing_local != expected_local) {
            std::stringstream s;
            s << "can't replace " << from_str << "=>" << to_str <<  " in " << existing_local << ": HH:MM:SS do not match";
            throw std::runtime_error(s.str());
        }
    }
    if (to_str == "unspecified") {
        time = std::nullopt;
    } else {
        auto new_local_time = local_from_y_m_d_h_m(to_str);
        auto new_zoned_time = date::make_zoned(time_zone, new_local_time);
        auto new_sys_time{new_zoned_time.get_sys_time()};
        time = new_sys_time;
    }
}

void shift_time_if_exists(std::optional<date::sys_seconds> & time, const std::string & shift_by) {
    char * end_ptr;
    int shift_by_min = std::strtol(shift_by.c_str(), &end_ptr, 10);
    if (end_ptr != shift_by.c_str()+shift_by.size()) {
        throw std::runtime_error("can't parse time shift value '" + shift_by + "'");
    }
    if (!time) return;
    *time += std::chrono::minutes(shift_by_min);
}

void change_paran_date(date::year_month_day & date, const std::string & from, const std::string & to) {
    std::istringstream s_from{from};
    date::year_month_day from_ymd;
    s_from >> date::parse("%Y-%m-%d", from_ymd);
    if (!s_from.good() || s_from.rdbuf()->in_avail() != 0) {
        std::stringstream out;
        out << date << ": can't replace '" << from << "' by '" << to << "': can't parse '" << from << "' as YYYY-MM-DD";
        throw std::runtime_error(out.str());
    }

    std::istringstream s_to{to};
    date::year_month_day to_ymd;
    s_to >> date::parse("%Y-%m-%d", to_ymd);
    if (!s_to.good() || s_to.rdbuf()->in_avail() != 0) {
        std::stringstream out;
        out << date << ": can't replace '" << from << "' by '" << to << "': can't parse '" << to << "' as YYYY-MM-DD";
        throw std::runtime_error(out.str());
    }

    if (date != from_ymd) {
        std::stringstream out;
        out << date << ": can't replace '" << from << "' by '" << to << "': dates don't match";
        throw std::runtime_error(out.str());
    }
    date = to_ymd;
}


void apply_vrata_fix(Precalculated_Vrata & vrata, const FixItem & fix) {
    switch (fix.type) {
    case Fix::ParanStartTime:
        replace_time(vrata.paranam_start, fix.from, fix.to, vrata.location.timezone_name);
        break;
    case Fix::ParanEndTime:
        replace_time(vrata.paranam_end, fix.from, fix.to, vrata.location.timezone_name);
        break;
    case Fix::Skip:
        vrata.skip = true;
        break;
    case Fix::AddMinutesToParanStartTimeIfExists:
        shift_time_if_exists(vrata.paranam_start, fix.to);
        break;
    case Fix::AddMinutesToParanEndTimeIfExists:
        shift_time_if_exists(vrata.paranam_end, fix.to);
        break;
    case Fix::ParanDate:
        change_paran_date(vrata.date, fix.from, fix.to);
    }
    vrata.already_fixed = true;
}

static vp::Location all_coord{"all", 0.0, 0.0, "UTC"};

void fix_vratas(std::vector<Precalculated_Vrata> & vratas, Fixes && fixes) {
    // first apply individual fixes
    for (auto & vrata : vratas) {
        auto fix_iter = fixes.find(vrata.location);
        if (fix_iter != fixes.end()) {
            for (auto & fix : fix_iter->second) {
                apply_vrata_fix(vrata, fix);
            }
        }
    }
    // if we have fixes to be applied to all coordinates, apply them,
    // but only to vratas NOT affected by individual fixes.
    if (auto iter=fixes.find(all_coord); iter != fixes.end()) {
        for (auto & vrata : vratas) {
            if (vrata.already_fixed) continue;
            for (auto & fix : iter->second) {
                apply_vrata_fix(vrata, fix);
            }
        }
    }
}

void test_one_precalculated_table_slug(const char * slug, Fixes fixes={}) {
    CAPTURE(slug);

    std::string filename{std::string{"data/precalculated-"} + slug + ".html"};
    auto s = slurp_file(source_dir_path / filename);
    REQUIRE(!s.empty());

    auto year = get_year_from_slug(slug);
    //sanity check
    REQUIRE(year >= date::year{2000});
    REQUIRE(year < date::year{2030});

    CAPTURE(year);
    std::vector<Precalculated_Vrata> vratas = extract_vratas_from_precalculated_table(std::move(s), year);
    fix_vratas(vratas, std::move(fixes));
    CAPTURE(vratas.size());
    check_precalculated_vratas(vratas);
}

TEST_CASE("precalculated ekAdashIs") {
    test_one_precalculated_table_slug(
                "2017-11-12", {
                    {vp::murmansk_coord, {{Fix::ParanStartTime, "10:30", "2017-11-15 10:36"}}},
                    {vp::riga_coord, {{Fix::ParanEndTime, "unspecified", "2017-11-15 09:40"}}},
                    {vp::jurmala_coord, {{Fix::ParanEndTime, "unspecified", "2017-11-15 09:40"}}}
                });
    test_one_precalculated_table_slug(
                "2017-11-27", {
                    {vp::murmansk_coord, {{Fix::Skip, "", ""}}}, // TODO: no sunrise cases
                    {vp::london_coord, {{Fix::AddMinutesToParanStartTimeIfExists, "", "-1"}}},
                });
//    test_one_precalculated_table_slug("2017-12-11"); // TODO: joined ekAdashI/atiriktA cells
    test_one_precalculated_table_slug(
                "2017-12-26", {
                    {vp::murmansk_coord, {{Fix::Skip, "", ""}}}, // TODO: no sunrise cases
                });
    test_one_precalculated_table_slug(
                "2018-01-10", {
                            {vp::petropavlovskkamchatskiy_coord, {{Fix::AddMinutesToParanStartTimeIfExists, "", "+3"}}},
                            {vp::murmansk_coord, {{Fix::Skip, "", ""}}}, // TODO: no sunrise cases
                });
    test_one_precalculated_table_slug(
                "2018-01-23", {
                    {vp::kophangan_coord,
                        {{Fix::AddMinutesToParanStartTimeIfExists, "", "+1"},
                         {Fix::AddMinutesToParanEndTimeIfExists, "", "+1"}}},
                    {all_coord,
                        {{Fix::AddMinutesToParanEndTimeIfExists, "", "+1"}}},
                });
//    test_one_precalculated_table_slug("2018-02-08"); // TODO: joined ekAdashI/atiriktA cells
    test_one_precalculated_table_slug("2018-02-24");
//    test_one_precalculated_table_slug("2018-03-10"); // TODO: shravaNA dvAdashI
//    test_one_precalculated_table_slug("2018-03-17"); // TODO: non-ekAdashI tables (chAndra-yugAdi etc)
    test_one_precalculated_table_slug("2018-03-23");
//    test_one_precalculated_table_slug("2018-04-09"); // TODO: joined ekAdashI/atiriktA cells
    test_one_precalculated_table_slug(
                "2018-04-24", {
                    {vp::kremenchug_coord,
                     // case manually verified by Ashutosha on 2020-02-21 and confirmed by Narasimha:
                     // the reason for difference is the underlying data difference. Old Panchangam gives sunrise < dvadashi_end
                     // (thus brief interval of pAraNam in dvAdashI puccha), and new data are sunrise > dvadashi, so standard pAraNam.
                     {{Fix::ParanStartTime, "05:36:30", "unspecified"},
                      {Fix::ParanEndTime, "05:37:00", "unspecified"}}},
                    {vp::fredericton_coord,
                      {{Fix::AddMinutesToParanStartTimeIfExists, "", "+1"}}},
                });
    test_one_precalculated_table_slug(
                "2018-05-09", {
                    {vp::manali_coord,
                     {{Fix::ParanStartTime, "unspecified", "2018-05-12 05:34"}}},
                    {vp::kalkuta_coord,
                     {{Fix::ParanStartTime, "unspecified", "2018-05-12 05:34"}}},
                    {vp::ekaterinburg_coord,
                     {{Fix::ParanStartTime, "unspecified", "2018-05-12 05:04"}}},
                    {vp::petropavlovskkamchatskiy_coord,
                     {{Fix::ParanStartTime, "05:36", "unspecified"},
                      {Fix::ParanEndTime, "05:36:30", "unspecified"}}},
                    {all_coord,
                     {{Fix::AddMinutesToParanStartTimeIfExists, "", "-1"}}},
                });
//    test_one_precalculated_table_slug("2018-05-14_adhimaasa"); // TODO: non-ekadashi tables (asdhimAsa start here)
    test_one_precalculated_table_slug(
                "2018-05-23", {
                    {vp::petropavlovskkamchatskiy_coord,
                     {{Fix::ParanStartTime, "unspecified", "2018-05-26 06:16"}}},
                });
    test_one_precalculated_table_slug(
                "2018-06-07", {
                    {vp::erevan_coord,
                     {{Fix::ParanEndTime, "unspecified", "2018-06-11 08:34"}}},
                    {vp::tbilisi_coord,
                     {{Fix::ParanEndTime, "08:33", "unspecified"}}},
                    {vp::stavropol_coord,
                     {{Fix::ParanEndTime, "unspecified", "2018-06-11 07:34"}}},
                    {vp::murmansk_coord,
                     {{Fix::Skip, "", ""}}}, // TODO: "no sunset" cases
                    {vp::tallin_coord,
                     {{Fix::ParanEndTime, "unspecified", "2018-06-11 07:34"}}},
                    {all_coord,
                     {{Fix::AddMinutesToParanEndTimeIfExists, "", "+1"}}},
                });
    test_one_precalculated_table_slug(
                "2018-06-21", {
                    {all_coord,
                     {{Fix::AddMinutesToParanStartTimeIfExists, "", "+1"}}},
                    {vp::murmansk_coord,
                     {{Fix::Skip, "", ""}}}, // TODO: "no sunset" cases
                });
    test_one_precalculated_table_slug(
                "2018-07-06", {
                    {vp::mirnyy_coord,
                     {{Fix::ParanStartTime, "5:18", "2018-07-10 06:17"}}},
                    {vp::habarovsk_coord,
                     {{Fix::ParanDate, "2018-07-09", "2018-07-10"}, // date change because we consider it (barely) sandigdha, while precalc table does not
                      {Fix::ParanStartTime, "7:18", "unspecified"}}},
                    {vp::vladivostok_coord,
                     {{Fix::AddMinutesToParanStartTimeIfExists, "", "-1"}}},
                    {vp::murmansk_coord,
                     {{Fix::Skip, "", ""}}}, // TODO: "no sunset" cases
                });
//    test_one_precalculated_table_slug("2018-07-20");
//    test_one_precalculated_table_slug("2018-08-05");
//    test_one_precalculated_table_slug("2018-08-19");
//    test_one_precalculated_table_slug("2018-08-31");
//    test_one_precalculated_table_slug("2018-09-12");
//    test_one_precalculated_table_slug("2018-09-22");
//    test_one_precalculated_table_slug("2018-10-03");
//    test_one_precalculated_table_slug("2018-10-18");
//    test_one_precalculated_table_slug("2018-11-01");
//    test_one_precalculated_table_slug("2018-11-17");
//    test_one_precalculated_table_slug("2018-12-01");
//    test_one_precalculated_table_slug("2018-12-12");
//    test_one_precalculated_table_slug("2018-12-29");
//    test_one_precalculated_table_slug("2019-01-09");
//    test_one_precalculated_table_slug("2019-01-13");
//    test_one_precalculated_table_slug("2019-01-29");
//    test_one_precalculated_table_slug("2019-02-12");
//    test_one_precalculated_table_slug("2019-02-28");
//    test_one_precalculated_table_slug("2019-03-15");
//    test_one_precalculated_table_slug("2019-03-29");
//    test_one_precalculated_table_slug("2019-04-11");
    test_one_precalculated_table_slug("2019-04-27");
//    test_one_precalculated_table_slug("2019-05-13");
//    test_one_precalculated_table_slug("2019-05-28");
//    test_one_precalculated_table_slug("2019-06-11");
//    test_one_precalculated_table_slug("2019-06-26");
//    test_one_precalculated_table_slug("2019-07-10");
//    test_one_precalculated_table_slug("2019-07-27");
//    test_one_precalculated_table_slug("2019-08-04");
//    test_one_precalculated_table_slug("2019-08-08");
//    test_one_precalculated_table_slug("2019-08-20");
//    test_one_precalculated_table_slug("2019-09-01");
//    test_one_precalculated_table_slug("2019-09-07");
//    test_one_precalculated_table_slug("2019-09-23");
//    test_one_precalculated_table_slug("2019-10-04");
//    test_one_precalculated_table_slug("2019-10-22");
//    test_one_precalculated_table_slug("2019-11-05");
//    test_one_precalculated_table_slug("2019-11-20");
//    test_one_precalculated_table_slug("2019-12-01");
//    test_one_precalculated_table_slug("2019-12-16");
//    test_one_precalculated_table_slug("2019-12-31");
//    test_one_precalculated_table_slug("2020-01-03");
//    test_one_precalculated_table_slug("2020-01-12");
//    test_one_precalculated_table_slug("2020-01-18");
//    test_one_precalculated_table_slug("2020-01-30");
//    test_one_precalculated_table_slug("2020-02-18");
}

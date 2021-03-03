#include "text-interface.h"

#include "location.h"

#include <algorithm>
#include <array>
#include "catch-formatters.h"
#include <regex>

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
    auto vratas = vp::text_ui::calc(date, "Murmansk");
    REQUIRE(vratas.size() == 1);
    REQUIRE(vratas.begin()->has_value());

    // expect location name to contain "adjusted" because there is no sunset in the original location at those dates.
    auto location_name = vratas.begin()->value().location_name();
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
    auto vratas = vp::text_ui::calc(2020_y/January/1, "all");

    REQUIRE(vratas.size() > 0);
    REQUIRE(std::all_of(vratas.cbegin(), vratas.cend(), [](const auto & vrata) {
        return vrata.has_value();
    }));
}

TEST_CASE("calc_all adjusts start date to ensure all locations have the same ekAdashI") {
    using namespace date;
    auto vratas = vp::text_ui::calc(2020_y/September/14, "all");

    auto [it_min_date, it_max_date] = std::minmax_element(vratas.cbegin(), vratas.cend(), [](const vp::MaybeVrata &l, const vp::MaybeVrata &r) { return l->date < r->date; });
    auto length = it_max_date->value().date - it_min_date->value().date;

    REQUIRE(length <= date::days{1});
}

TEST_CASE("can call calc_one with string for location name") {
    using namespace date;
    auto vratas = vp::text_ui::calc(2020_y/January/1, std::string("Kiev"));
    REQUIRE(!vratas.empty());
    auto & vrata = *vratas.begin();
    REQUIRE(vrata.has_value());
}

TEST_CASE("print_detail_one for Udupi 2020-11-14 does NOT raise exception and includes Amavasya") {
    fmt::memory_buffer buf;
    using namespace date::literals;
    using Catch::Matchers::Contains;
    const date::year_month_day date{2020_y/11/14};
    REQUIRE_NOTHROW(vp::text_ui::daybyday_print_one(date, "Udupi", buf, vp::CalcFlags::Default));

    REQUIRE_THAT(fmt::to_string(buf), Contains("Amavasya"));
}

TEST_CASE("daybyday_print_one() includes sauramasa info") {
    using namespace date::literals;
    using Catch::Matchers::Contains;
    using Catch::Matchers::Matches;

    SECTION("'sankranti' event is present for saura masa-crossing day-by-day report (2020-12-15)") {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(2020_y/12/15, "Udupi", buf, vp::CalcFlags::Default);

        auto s = fmt::to_string(buf);
        REQUIRE_THAT(s, Contains("Saura māsa: Vṛścika"));
        REQUIRE_THAT(s, Contains("Dhanu sankranti"));
    }

    SECTION("'sankranti' event must NOT be present for non-masa-crossing day-by-day report (2020-12-17)") {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(2020_y/12/17, "Udupi", buf, vp::CalcFlags::Default);

        auto s = fmt::to_string(buf);
        REQUIRE_THAT(s, Contains("Saura māsa: Dhanu"));
        REQUIRE_THAT(s, !Contains("sankranti"));
    }
}

TEST_CASE("daybyday_print_one() includes chandramasa info") {
    using namespace date::literals;
    using Catch::Matchers::Contains;
    using Catch::Matchers::Matches;

    SECTION("Chandra-masa info when not crossing any Purnima/Amavasya events (2020-11-23)") {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(2020_y/11/23, "Udupi", buf, vp::CalcFlags::Default);

        auto s = fmt::to_string(buf);
        SECTION("'Chāndra māsa: such-and-such is present") {
            REQUIRE_THAT(s, Contains("Chāndra māsa: Kārtikā"));
        }
        SECTION("'such-and-such masa starts' is NOT present") {
            REQUIRE_THAT(s, !Contains("Mārgaśīrṣa"));
        }
    }

    SECTION("Chandra-masa info when we DO cross Purnima (2020-11-30)") {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(2020_y/11/30, "Udupi", buf, vp::CalcFlags::Default);

        auto s = fmt::to_string(buf);
        REQUIRE_THAT(s, Contains("Chāndra māsa: Kārtikā"));
        REQUIRE_THAT(s, !Contains("Mārgaśīrṣa"));
    }

    SECTION("Chandra-masa info when we DO cross Amavasya (2020-11-14)") {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(2020_y/11/14, "Udupi", buf, vp::CalcFlags::Default);

        auto s = fmt::to_string(buf);
        REQUIRE_THAT(s, Contains("Chāndra māsa: Aśvin"));
        REQUIRE_THAT(s, Contains("Kārtikā"));
    }
}

namespace {
std::string get_time_str_for_description(const std::string & buf, const std::string & search_for) {
    auto pos = buf.find(search_for);
    if (pos == std::string::npos) { return {}; }
    auto newline_pos = buf.rfind('\n', pos);
    if (newline_pos == std::string::npos) { return {}; }
    return buf.substr(newline_pos+1, strlen("2020-11-26 15:58:14.076468 EET"));
}
}

TEST_CASE("ekādaśī details and day-by-day give the same time for dvādaśī's 1/4") {
    using namespace date::literals;
    constexpr auto date = 2020_y/11/25;
    constexpr auto location_name = "Udupi";

    const auto daybyday_dvadashi_quarter_time = [&]() {
        fmt::memory_buffer buf;
        vp::text_ui::daybyday_print_one(date, location_name, buf, vp::CalcFlags::Default);
        return get_time_str_for_description(fmt::to_string(buf), "First quarter of Shukla Dvadashi ends");
    }();

    const auto details_dvadashi_quarter_time = [&]() {
        fmt::memory_buffer buf;
        vp::text_ui::find_calc_and_report_one(date, location_name, buf);
        return get_time_str_for_description(fmt::to_string(buf), "Dvādaśī's first quarter ends");
    }();

    REQUIRE(daybyday_dvadashi_quarter_time == details_dvadashi_quarter_time);
}

TEST_CASE("daybyday_calc_one()") {
    using namespace date::literals;
    auto info = vp::text_ui::daybyday_calc_one(2020_y/12/14, vp::kiev_coord, vp::CalcFlags::Default);
    SECTION("gives both sunrises and sunset for the target date") {
        REQUIRE(info.sunrise1.has_value());
        REQUIRE(info.sunset1.has_value());
        REQUIRE(info.sunrise2.has_value());
        REQUIRE(info.saura_masa == vp::Saura_Masa::Vrishchika);
        REQUIRE(info.chandra_masa == vp::Chandra_Masa::Kartika);
    }

    SECTION("contains 'until' time for saura māsa") {
        using namespace std::chrono_literals;
        REQUIRE(info.saura_masa_until.has_value());
        REQUIRE(info.saura_masa_until->round_to_minute() == date::sys_days{2020_y/12/15} + 16h + 2min);
    }

    SECTION("contains 'until' time for chandra māsa") {
        using namespace std::chrono_literals;
        REQUIRE(info.chandra_masa_until.has_value());
        REQUIRE(info.chandra_masa_until->round_to_minute() == date::sys_days{2020_y/12/14} + 16h + 17min);
    }

    SECTION("contains one tithi and correponding 'until' time") {
        using namespace std::chrono_literals;
        REQUIRE(info.tithi == vp::DiscreteTithi::Amavasya());
        REQUIRE(info.tithi_until->round_to_minute() == date::sys_days{2020_y/12/14} + 16h + 17min);
        REQUIRE(info.tithi2 == vp::DiscreteTithi::Unknown());
        REQUIRE_FALSE(info.tithi2_until.has_value());
    }

    SECTION("contains nakshatra{,2} and correponding 'until' time(s)") {
        using namespace std::chrono_literals;
        REQUIRE(info.nakshatra == vp::DiscreteNakshatra::Jyeshtha());
        REQUIRE(info.nakshatra_until->round_to_minute() == date::sys_days{2020_y/12/14} + 17h + 56min); // 19:56 local time
        REQUIRE(info.nakshatra2 == vp::DiscreteNakshatra::Unknown());
        REQUIRE(!info.nakshatra2_until.has_value());
    }
}

TEST_CASE("two nakshatra starts within the same day are reported properly") {
    using namespace date::literals;
    using namespace std::chrono_literals;
    auto info = vp::text_ui::daybyday_calc_one(2021_y/3/1, vp::udupi_coord, vp::CalcFlags::Default);
    REQUIRE(info.nakshatra == vp::DiscreteNakshatra::UttaraPhalguni());
    const auto tz_offset = 5h+30min; //IST
    REQUIRE(info.nakshatra_until->round_to_minute() == date::sys_days{2021_y/3/1} + 7h + 37min - tz_offset); // 07:37 local time
    REQUIRE(info.nakshatra2 == vp::DiscreteNakshatra::Hasta());
    REQUIRE(info.nakshatra2_until->round_to_minute() == date::sys_days{2021_y/3/2} + 5h + 32min - tz_offset); // 05:32 next day
}

TEST_CASE("daybyday_print_one()") {
    using namespace date::literals;
    using namespace Catch::Matchers;
    fmt::memory_buffer buf;
    vp::text_ui::daybyday_print_one(2020_y/12/14, "Kiev", buf, vp::CalcFlags::Default);
    auto s = fmt::to_string(buf);
    SECTION("add '-----' separators before both sunrises") {
        std::regex r{R"(----\n[^\n]*sunrise)"};
        auto iter_begin = std::sregex_iterator(s.begin(), s.end(), r);
        auto iter_end = std::sregex_iterator{};
        CAPTURE(s);
        REQUIRE(std::distance(iter_begin, iter_end) == 2);
    }
    SECTION("saura masa 'until' is present") {
        REQUIRE_THAT(s, Contains("Vṛścika (until 2020-12-15"));
    }

    SECTION("chandra māsa 'until' is present") {
        REQUIRE_THAT(s, Contains("Kārtikā (until 2020-12-14"));
    }
    SECTION("Tithi/until is present") {
        REQUIRE_THAT(s, Contains("Amāvāsyā until 18:17"));
    }
    SECTION("nakshatra/until are present") {
        REQUIRE_THAT(s, Contains("Jyeṣṭhā until 19:56"));
        REQUIRE_THAT(s, !Contains("Mūla until"));
    }
}

TEST_CASE("DayByDay for 2021-02-17 Udupi gives tithi (which does not change from sunrise till next sunrise)") {
    using namespace date::literals;
    auto info = vp::text_ui::daybyday_calc_one(2021_y/2/17, vp::udupi_coord, vp::CalcFlags::Default);
    REQUIRE(info.tithi == vp::DiscreteTithi::Shukla_Shashthi());
}

TEST_CASE("Vasanta-pañcamī etc are present in 2021") {
    using namespace date;
    auto vratas = vp::text_ui::calc(2021_y/February/10, std::string("Saint Petersburg"), vp::CalcFlags::RefractionOn);
    REQUIRE(!vratas.empty());
    auto & vrata = *vratas.begin();
    REQUIRE(vrata.has_value());
    REQUIRE(vrata->dates_for_this_paksha.size() == 5);
    const auto any_date_for = [&](date::year_month_day date) {
        const auto iter = vrata->dates_for_this_paksha.find(date::local_days{date});
        if (iter == vrata->dates_for_this_paksha.end()) {
            throw std::runtime_error(fmt::format(FMT_STRING("can't find nameworthy date for {}"), date));
        }
        return iter->second;
    };
    REQUIRE(any_date_for(2021_y/February/16).name == "Vasanta-pañcamī");
    REQUIRE(any_date_for(2021_y/February/18).name == "Ratha-saptamī");
    REQUIRE(any_date_for(2021_y/February/20).name == "Bhīṣmāṣtamī");
    REQUIRE(any_date_for(2021_y/February/21).name == "Madhva-navamī (cāndra)");
    REQUIRE(any_date_for(2021_y/February/27).name == "Pūrṇimā, End of Māgha-snāna-vrata");
}

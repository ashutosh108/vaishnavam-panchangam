#include "paran.h"

#include "catch-formatters.h"
#include "date-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

TEST_CASE("Can create with default constructor") {
    [[maybe_unused]] Paran p{};
}

TEST_CASE("Can create paran of \"before\" type") {
    JulDays_UT arbitrary_time{2019_y/March/19, 11h + 3min};
    Paran p{Paran::Type::Standard, std::nullopt, arbitrary_time};
}

TEST_CASE("Default-constructed parans are equal") {
    REQUIRE(Paran{} == Paran{});
    REQUIRE_FALSE(Paran{} != Paran{});
}

TEST_CASE("Default-constructed and \"before\" parans are not equal") {
    JulDays_UT arbitrary_time{2019_y/March/19, 11h + 3min};
    REQUIRE(Paran{} != Paran{Paran::Type::Standard, std::nullopt, arbitrary_time});
}

TEST_CASE("\"before\" parans with different times are not equal") {
    JulDays_UT arbitrary_time1{2019_y/March/19, 11h + 3min};
    JulDays_UT arbitrary_time2{2019_y/March/19, 11h + 15min};
    Paran p1{Paran::Type::Standard, std::nullopt, arbitrary_time1};
    Paran p2{Paran::Type::Standard, std::nullopt, arbitrary_time2};
    REQUIRE(p1 != p2);
}

TEST_CASE("Can print paran") {
    REQUIRE(fmt::to_string(Paran{}) != "");
}

TEST_CASE("Can compare paran types") {
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::Puccha_Dvadashi});
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::From_Quarter_Dvadashi});
}

TEST_CASE("compact paran format: standard paran is '*'") {
    SECTION("standard pāraṇam without limit") {
        JulDays_UT arbitrary_time{2019_y/March/19, 5h + 3min + 5s};
        auto timezone = date::locate_zone("Europe/Moscow");
        Paran p{Paran::Type::Standard, arbitrary_time, std::nullopt, timezone};

        REQUIRE("*" == fmt::format("{:c}", p));
    }
    SECTION("standard pāraṇam *with* limit") {
        JulDays_UT arbitrary_time{2019_y/March/19, 5h + 3min + 5s};
        JulDays_UT limit_time{2019_y/March/19, 9h + 3min + 5s};
        auto timezone = date::locate_zone("Europe/Moscow");
        Paran p{Paran::Type::Standard, arbitrary_time, std::nullopt, limit_time, timezone};

        REQUIRE("* (<12:03)" == fmt::format("{:c}", p));
    }
}

TEST_CASE("compact paran format: >08:04 case") {
    JulDays_UT arbitrary_time{2019_y/March/19, 5h + 3min + 5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::From_Quarter_Dvadashi, arbitrary_time, std::nullopt, timezone};

    REQUIRE(">08:04" == fmt::format("{:c}", p));
}

TEST_CASE("compact paran format: <09:03 case") {
    JulDays_UT arbitrary_time{2019_y/March/19, 6h + 3min + 5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::From_Quarter_Dvadashi, std::nullopt, arbitrary_time, timezone};

    REQUIRE("<09:03" == fmt::format("{:c}", p));
}

TEST_CASE("compact paran format: wide range (rounding to minute)") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 5s};
    JulDays_UT time2{2019_y/March/19, 6h + 3min + 5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::Puccha_Dvadashi, time1, time2, timezone};

    REQUIRE(u8"08:04–\u200B09:03" == fmt::format("{:c}", p));
}

TEST_CASE("compact paran format: narrow range (rounding to seconds)") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 4.3s};
    JulDays_UT time2{2019_y/March/19, 5h + 8min + 5.5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::From_Quarter_Dvadashi, time1, time2, timezone};

    REQUIRE(u8"08:03:05–\u200B08:08:05" == fmt::format("{:c}", p));
}

TEST_CASE("paran_start/end_str rounds to minutes for large (>=48 min) interval") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 4s};
    JulDays_UT time2{2019_y/March/19, 5h + 3min + 4s + 48min};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::From_Quarter_Dvadashi, time1, time2, timezone};

    REQUIRE("08:04" == p.start_str());
    REQUIRE("08:51" == p.end_str());
}

TEST_CASE("paran_start/end_str rounds to minutes when rounded interval is just 5 minutes") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 0.5s};
    JulDays_UT time2{2019_y/March/19, 5h + 9min + 59.5s};
    Paran p{Paran::Type::From_Quarter_Dvadashi, time1, time2};

    REQUIRE("05:04" == p.start_str());
    REQUIRE("05:09" == p.end_str());
}

TEST_CASE("paran_start/end_str rounds to minutes when rounded interval is 4 minutes") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 0.5s};
    JulDays_UT time2{2019_y/March/19, 5h + 8min + 59.5s};
    Paran p{Paran::Type::From_Quarter_Dvadashi, time1, time2};

    REQUIRE("05:03:01" == p.start_str());
    REQUIRE("05:08:59" == p.end_str());
}

TEST_CASE("can get and print types of paran start/end") {
    auto start_time = vp::JulDays_UT{date::sys_days{2020_y/January/1} + 5h + 30min + 15s};
    auto end_time = start_time + 55min;
    vp::Paran paran{vp::Paran::Type::Standard, start_time, end_time};

    using Catch::Matchers::Contains;
    REQUIRE_THAT(fmt::to_string(paran.start_type()), Contains("sunrise"));
    REQUIRE_THAT(fmt::to_string(paran.end_type()), Contains("1/5th of daytime"));
}

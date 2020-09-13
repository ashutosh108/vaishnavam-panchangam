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
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::Until_Dvadashi_End});
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::From_Quarter_Dvadashi});
}

TEST_CASE("compact paran format: standard paran is '*'") {
    JulDays_UT arbitrary_time{2019_y/March/19, 5h + 3min + 5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::Standard, arbitrary_time, std::nullopt, timezone};

    REQUIRE("*" == fmt::format("{:c}", p));
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

    REQUIRE("08:04–09:03" == fmt::format("{:c}", p));
}

TEST_CASE("compact paran format: narrow range (rounding to seconds)") {
    JulDays_UT time1{2019_y/March/19, 5h + 3min + 4.3s};
    JulDays_UT time2{2019_y/March/19, 5h + 13min + 5.5s};
    auto timezone = date::locate_zone("Europe/Moscow");
    Paran p{Paran::Type::From_Quarter_Dvadashi, time1, time2, timezone};

    REQUIRE("08:03:05–08:13:05" == fmt::format("{:c}", p));
}

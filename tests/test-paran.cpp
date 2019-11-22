#include "catch.hpp"

#include <sstream>

#include "date/date.h"
#include "paran.h"

using namespace date;

TEST_CASE("Can create with default constructor") {
    [[maybe_unused]] Paran p{};
}

TEST_CASE("Can create paran of \"before\" type") {
    Swe_Time arbitrary_time{2019_y, March, 19_d, 11, 3, 0};
    Paran p{Paran::Type::Standard, std::nullopt, arbitrary_time};
}

TEST_CASE("Default-constructed parans are equal") {
    REQUIRE(Paran{} == Paran{});
    REQUIRE_FALSE(Paran{} != Paran{});
}

TEST_CASE("Default-constructed and \"before\" parans are not equal") {
    Swe_Time arbitrary_time{2019_y, March, 19_d, 11, 3, 0};
    REQUIRE(Paran{} != Paran{Paran::Type::Standard, std::nullopt, arbitrary_time});
}

TEST_CASE("\"before\" parans with different times are not equal") {
    Swe_Time arbitrary_time1{2019_y, March, 19_d, 11, 3, 0};
    Swe_Time arbitrary_time2{2019_y, March, 19_d, 11, 15, 0};
    Paran p1{Paran::Type::Standard, std::nullopt, arbitrary_time1};
    Paran p2{Paran::Type::Standard, std::nullopt, arbitrary_time2};
    REQUIRE(p1 != p2);
}

TEST_CASE("Can print paran") {
    std::ostringstream s;
    s << Paran{};
    REQUIRE(s.str() != "");
}

TEST_CASE("Can compare paran types") {
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::Until_Dvadashi_End});
    REQUIRE(Paran{Paran::Type::Standard} != Paran{Paran::Type::From_Quarter_Dvadashi});
}

#include "catch.hpp"

#include <sstream>

#include "paran.h"

TEST_CASE("Can create with default constructor") {
    Paran p{};
}

TEST_CASE("Can create paran of \"before\" type") {
    Swe_Time arbitrary_time{2019, 3, 19, 11, 3, 0};
    Paran p{Paran::Type::before, arbitrary_time};
}

TEST_CASE("Default-constructed parans are equal") {
    REQUIRE(Paran{} == Paran{});
    REQUIRE(!(Paran{} != Paran{}));
}

TEST_CASE("Default-constructed and \"before\" parans are not equal") {
    Swe_Time arbitrary_time{2019, 3, 19, 11, 3, 0};
    REQUIRE(Paran{} != Paran{Paran::Type::before, arbitrary_time});
}

TEST_CASE("\"before\" parans with different times are not equal") {
    Swe_Time arbitrary_time1{2019, 3, 19, 11, 3, 0};
    Swe_Time arbitrary_time2{2019, 3, 19, 11, 15, 0};
    Paran p1{Paran::Type::before, arbitrary_time1};
    Paran p2{Paran::Type::before, arbitrary_time2};
    REQUIRE(p1 != p2);
}

TEST_CASE("Can print paran") {
    std::ostringstream s;
    s << Paran{};
    REQUIRE(s.str() != "");
}

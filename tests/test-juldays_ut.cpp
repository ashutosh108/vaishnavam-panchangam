#include "juldays_ut.h"

#include "disable-warning.h"

#include "catch.hpp"
#include <chrono>
#include <sstream>
#include "tz-fixed.h"

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

TEST_CASE( "Can create JulDays_UT from double_days" ) {
    const double_days arbitrary_double_days{124.0};
    [[maybe_unused]] JulDays_UT t{arbitrary_double_days};
    REQUIRE(true);
}

TEST_CASE ("Pretty print JulDays_UT") {
    std::stringstream s;
    s << JulDays_UT{double_days{2458552.686736239120364}};
    REQUIRE(s.str() == "2019-03-10 04:28:54.011060 UTC");
}

TEST_CASE ("raw_julian_days_ut") {
    JulDays_UT t{double_days{123.50}};
    REQUIRE(t.raw_julian_days_ut() == double_days{123.50});
}

TEST_CASE ("construct from Y, m, d") {
    std::stringstream s;
    s << JulDays_UT{2019_y/March/10};
    REQUIRE(s.str() == "2019-03-10 00:00:00.000000 UTC");
}

TEST_CASE ("construct from Y, m, d, hours") {
    std::stringstream s;
    s << JulDays_UT{2019_y/March/10, double_hours{5.25}};
    REQUIRE(s.str() == "2019-03-10 05:15:00.000000 UTC");
}

TEST_CASE("hours matter") {
    JulDays_UT t1{2019_y/March/10, double_hours{0.0}};
    JulDays_UT t2{2019_y/March/10, double_hours{6.0}};
    double_days diff = t2 - t1;
    REQUIRE(diff == double_days{0.25});
}

TEST_CASE("can compare") {
    JulDays_UT t1{2019_y/March/10_d};
    JulDays_UT t2{double_days{2458552.5}};
    REQUIRE(t1 == t2);
}

TEST_CASE("create from Y M D h m s", "[hms]") {
    JulDays_UT t1{2019_y/March/17, double_hours{4.2267416752777778}};
    JulDays_UT t2{2019_y/March/17, 4h + 13min + 36.270031s};
    REQUIRE(t1 == t2);
}

TEST_CASE("Can create from Date") {
    date::year_month_day d{2019_y/March/9};
    JulDays_UT t{d};
    REQUIRE(t == JulDays_UT{2019_y/March/9});
}

TEST_CASE("Can add double to JulDays_UT") {
    // make sure that first argument to + can be const
    JulDays_UT const t1{2019_y/March/17};
    REQUIRE(t1+double_days{1.0} == JulDays_UT{2019_y/March/18});
}

TEST_CASE("+= returns correct value") {
    REQUIRE((JulDays_UT{2019_y/March/17}+=double_days{1.0}) == JulDays_UT{2019_y/March/18});
}

TEST_CASE("+= modifies JulDays_UT") {
    JulDays_UT t{2019_y/March/17};
    t += double_days{1.0};
    REQUIRE(t == JulDays_UT{2019_y/March/18});
}

TEST_CASE("Substracting double from JulDays_UT means substracting days") {
    JulDays_UT const t1{2019_y/March/17};
    REQUIRE(t1-double_days{1.0} == JulDays_UT{2019_y/March/16});
}

TEST_CASE("-= returns correct value") {
    REQUIRE((JulDays_UT{2019_y/March/17}-=double_days{1.0}) == JulDays_UT{2019_y/March/16});
}

TEST_CASE("-= modifies JulDays_UT") {
    JulDays_UT t{2019_y/March/17};
    t -= double_days{1.0};
    REQUIRE(t == JulDays_UT{2019_y/March/16});
}

TEST_CASE("Can compare <") {
    REQUIRE(JulDays_UT{2019_y/March/17} < JulDays_UT{2019_y/March/18});
}

TEST_CASE("Can compare >") {
    REQUIRE(JulDays_UT{2019_y/March/18} > JulDays_UT{2019_y/March/17});
}

TEST_CASE("Can substract JulDays_UT from JulDays_UT") {
    REQUIRE((JulDays_UT{2019_y/April/16} - JulDays_UT{2019_y/April/10}) == double_days{6.0});
}

TEST_CASE("Pretty-print zoned time") {
    std::stringstream s;
    s << JulDays_Zoned{"Europe/Kiev", JulDays_UT{2019_y/March/10, double_hours{4.5}}};
    REQUIRE(s.str() == "2019-03-10 06:30:00.000000 EET");
}

TEST_CASE("Can create Juldays_UT from local hh:mm") {
    date::local_time<double_days> local = date::local_days(2019_y/January/1) + std::chrono::hours{3} + std::chrono::minutes{15};
    JulDays_UT jd{local, date::locate_zone("Europe/Moscow")};
    // 0.25 means 00:15am UTC which correposnds to 03:15 Moscow local time
    REQUIRE(JulDays_UT{2019_y/January/1, double_hours{0.25}} == jd);
}

TEST_CASE("round_to_minute_up() works") {
    using namespace std::chrono_literals;
    auto date{2019_y/January/1};
    std::pair pair1{3h+5min+1s, 3h+6min};
    std::pair pair2{0h+0min+0s, 0h+0min};
    std::pair pair3{22h+59min+59s, 23h+0min};
    REQUIRE(date::sys_days(date) + pair1.second == JulDays_UT{date, pair1.first}.round_to_minute_up());
    REQUIRE(date::sys_days(date) + pair2.second == JulDays_UT{date, pair2.first}.round_to_minute_up());
    REQUIRE(date::sys_days(date) + pair3.second == JulDays_UT{date, pair3.first}.round_to_minute_up());
}

TEST_CASE("round_to_minute_down() works") {
    using namespace std::chrono_literals;
    auto date{2019_y/January/1};
    std::pair pair1{3h+5min+1s, 3h+5min};
    std::pair pair2{0h+0min+0s, 0h+0min};
    std::pair pair3{22h+59min+59s, 22h+59min};
    REQUIRE(date::sys_days(date) + pair1.second == JulDays_UT{date, pair1.first}.round_to_minute_down());
    REQUIRE(date::sys_days(date) + pair2.second == JulDays_UT{date, pair2.first}.round_to_minute_down());
    REQUIRE(date::sys_days(date) + pair3.second == JulDays_UT{date, pair3.first}.round_to_minute_down());
}

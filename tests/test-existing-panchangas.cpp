#include "catch-formatters.h"

#include "location.h"
#include "swe.h"

#include <array>

using namespace date::literals;
using namespace std::chrono_literals;

namespace {
template <class Duration>
bool operator==(const date::zoned_time<Duration> & left, std::chrono::minutes right) {
    const auto rounded = date::round<std::chrono::minutes>(left.get_local_time());
    return date::format("%H:%M", rounded) == date::format("%H:%M", right);
}
std::chrono::minutes constexpr operator""_hm(const char * s, std::size_t len) {
    int hours=-1, minutes=-1;
    if (len != 5) {
        throw std::domain_error("length is not 5: " + std::string{s});
    }
    if (!(s[0] >= '0' && s[0] <= '2' && s[1] >= '0' && s[1] <= '9')) {
        throw std::domain_error("can't parse hours: " + std::string{s});
    }
    hours = (s[0] - '0')*10 + (s[1] - '0');
    if (!(s[3] >= '0' && s[3] <= '5' && s[4] >= '0' && s[4] <= '9')) {
        throw std::domain_error("can't parse minutes: " + std::string{s});
    }
    minutes = (s[3] - '0')*10 + (s[4] - '0');
    return std::chrono::hours{hours} + std::chrono::minutes{minutes};
}
}

TEST_CASE("compare sunrises with Palimaru 2020 panchangams") {
    using vp::operator""_N;
    using vp::operator""_E;
    int min_fails = std::numeric_limits<int>::max();
    double min_sum_distance = std::numeric_limits<double>::max();
    vp::Location location_at_min;
    int i_at_min = 0, j_at_min = 0;
    for (int i=0; i<60; ++i) {
        for (int j=0; j<60; ++j) {
            vp::Location udupi{13'20'27_N + ((i-30)/90.0),  74'45'06_E + (j-30)/90.0};
            vp::Swe swe(udupi, vp::CalcFlags::RefractionOn | vp::CalcFlags::SunriseByDiscCenter | vp::CalcFlags::RiseSetGeocentricOff);
            int num_fails = 0;
            double sum_distance = 0;
            const auto check_sunrise_sunset = [&swe, &num_fails, &sum_distance](date::year_month_day date, std::chrono::minutes expected_sunrise, std::chrono::minutes expected_sunset) {
                const auto sunrise = swe.find_sunrise_v(vp::JulDays_UT{date}).as_zoned_time(date::locate_zone("Asia/Kolkata"));
                const auto sunset = swe.find_sunset_v(vp::JulDays_UT{date}).as_zoned_time(date::locate_zone("Asia/Kolkata"));
                const auto expected_local_sunrise = date::local_days{date} + expected_sunrise;
                const auto expected_local_sunset = date::local_days{date} + expected_sunset;
                const auto delta_sunrise = (sunrise.get_local_time() - expected_local_sunrise).count();
                const auto delta_sunset = (sunset.get_local_time() - expected_local_sunset).count();
                const auto sum_squares = delta_sunrise * delta_sunrise + delta_sunset * delta_sunset;
                sum_distance += sum_squares;
//                CHECK(sunrise == expected_sunrise);
                if (!(sunrise == expected_sunrise))  {
                    ++num_fails;
                }
                if (!(sunset == expected_sunset))  {
                    ++num_fails;
                }
                return sum_squares;
            };
            struct date_sr_ss {
                date::year_month_day date;
                std::chrono::minutes sunrise;
                std::chrono::minutes sunset;
            };
            constexpr std::array<date_sr_ss, 52> test_data{{
                { 2020_y/3/25,  "06:33"_hm, "18:40"_hm },
                { 2020_y/4/8,   "06:23"_hm, "18:41"_hm },
                { 2020_y/4/9,   "06:23"_hm, "18:41"_hm },
                { 2020_y/4/23,  "06:14"_hm, "18:43"_hm },
                { 2020_y/4/24,  "06:14"_hm, "18:43"_hm },
                { 2020_y/5/7,   "06:08"_hm, "18:46"_hm },
                { 2020_y/5/8,   "06:08"_hm, "18:46"_hm },
                { 2020_y/5/22,  "06:05"_hm, "18:49"_hm },
                { 2020_y/5/23,  "06:05"_hm, "18:50"_hm },
                { 2020_y/6/5,   "06:04"_hm, "18:53"_hm },
                { 2020_y/6/6,   "06:04"_hm, "18:54"_hm },
                { 2020_y/6/21,  "06:06"_hm, "18:58"_hm },
                { 2020_y/6/22,  "06:07"_hm, "18:58"_hm },
                { 2020_y/7/5,   "06:10"_hm, "19:00"_hm },
                { 2020_y/7/6,   "06:10"_hm, "19:00"_hm }, // 06:10:33.375818 != 06:10
                { 2020_y/7/20,  "06:13"_hm, "19:00"_hm },
                { 2020_y/7/21,  "06:14"_hm, "19:00"_hm }, // 06:14:43.358569 != 06:14
                { 2020_y/8/3,   "06:17"_hm, "18:56"_hm },
                { 2020_y/8/4,   "06:17"_hm, "18:58"_hm }, // 06:17:53.856309 != 06:17
                { 2020_y/8/19,  "06:20"_hm, "18:49"_hm },
                { 2020_y/8/20,  "06:20"_hm, "18:48"_hm },
                { 2020_y/9/2,   "06:21"_hm, "18:41"_hm },
                { 2020_y/9/3,   "06:21"_hm, "18:40"_hm },
                { 2020_y/9/17,  "06:21"_hm, "18:30"_hm },
                { 2020_y/9/18,  "06:21"_hm, "18:29"_hm },
                { 2020_y/10/1,  "06:22"_hm, "18:21"_hm },
                { 2020_y/10/2,  "06:22"_hm, "18:20"_hm },
                { 2020_y/10/16, "06:23"_hm, "18:11"_hm },
                { 2020_y/10/17, "06:23"_hm, "18:10"_hm },
                { 2020_y/10/31, "06:26"_hm, "18:04"_hm },
                { 2020_y/11/1,  "06:26"_hm, "18:03"_hm },
                { 2020_y/11/15, "06:31"_hm, "17:59"_hm },
                { 2020_y/11/16, "06:32"_hm, "17:59"_hm },
                { 2020_y/11/30, "06:38"_hm, "18:00"_hm },
                { 2020_y/12/1,  "06:39"_hm, "18:00"_hm },
                { 2020_y/12/14, "06:47"_hm, "18:04"_hm },
                { 2020_y/12/15, "06:47"_hm, "18:04"_hm },
                { 2020_y/12/30, "06:55"_hm, "18:12"_hm },
                { 2020_y/12/31, "06:55"_hm, "18:13"_hm },
                { 2021_y/1/13,  "06:59"_hm, "18:20"_hm },
                { 2021_y/1/14,  "06:59"_hm, "18:21"_hm },
                { 2021_y/1/28,  "07:00"_hm, "18:28"_hm },
                { 2021_y/1/29,  "07:00"_hm, "18:29"_hm }, // 06:59:13.999646 != 07:00
                { 2021_y/2/11,  "06:58"_hm, "18:33"_hm },
                { 2021_y/2/12,  "06:58"_hm, "18:34"_hm }, // 06:56:03.721620 != 06:58
                { 2021_y/2/27,  "06:50"_hm, "18:37"_hm },
                { 2021_y/2/28,  "06:50"_hm, "18:37"_hm }, // 06:48:48.946106 != 06:50
                { 2021_y/3/13,  "06:42"_hm, "18:39"_hm },
                { 2021_y/3/14,  "06:41"_hm, "18:39"_hm }, // 06:40:20.415732 != 06:41
                { 2021_y/3/28,  "06:31"_hm, "18:40"_hm },
                { 2021_y/3/29,  "06:30"_hm, "18:40"_hm },
                { 2021_y/4/12,  "06:21"_hm, "18:41"_hm }
            }};
            for (const auto & test_datum : test_data) {
                check_sunrise_sunset(test_datum.date, test_datum.sunrise, test_datum.sunset);
            }
            if (sum_distance < min_sum_distance) {
                min_sum_distance = sum_distance;
                i_at_min = i;
                j_at_min = j;
                location_at_min = udupi;
                min_fails = num_fails;
            }
        }
    }
    CAPTURE(i_at_min);
    CAPTURE(j_at_min);
    CAPTURE(location_at_min.longitude.longitude);
    CAPTURE(location_at_min.latitude.latitude);
    CAPTURE(min_sum_distance);
    REQUIRE(min_fails == 0);
}

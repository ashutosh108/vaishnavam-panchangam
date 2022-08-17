#include "calc.h"
#include "catch-formatters.h"
#include "date-fixed.h"

#include "jayanti.h"

#include <chrono>

using namespace date;
using namespace vp;
using namespace std::literals::chrono_literals;

namespace Catch {

template<>
struct StringMaker<std::vector<RohiniBahulashtamiYoga>> {
    static std::string convert(const std::vector<RohiniBahulashtamiYoga> & yogas) {
        return fmt::to_string(yogas);
    }
};

template<>
struct StringMaker<Saura_Masa_Point> {
    static std::string convert(const Saura_Masa_Point & p) {
        fmt::memory_buffer buf;
        fmt::appender out{buf};
        fmt::format_to(out, FMT_STRING("{}+{}"), p.floor(), std::fmod(p.val, 1));
        return fmt::to_string(buf);
    }
};

} // namespace Catch

void check_that_there_is_only_one_viable_candidate(const std::vector<RohiniBahulashtamiYoga> & yogas) {
    if (yogas.size() >= 2) {
        CHECK((yogas[0].kalpa() != yogas[1].kalpa() || yogas[0].simha_masa_at_midnight > yogas[1].simha_masa_at_midnight));
    }
}

/**
 * ‘Rohiṇī-bahulāṣṭamī-yoga’ is defined as a combination of Rohiṇī-nakṣatra
 * and Bahulāṣṭamī in the period of time from a sunrise to a next sunrise
 * (a definition of a ‘day’). In the majority of cases this combination
 * is in the form of an overlap, but in some rare years nakṣatra and tithi
 * start or end during the same day without overlapping.
 *
 * This test ensures that Rohiṇī-bahulāṣṭamī-yoga:
 * 1. Happens every year (with one special case, 1862, as noted in the test
 *    and comment below).
 * 2. Never more that 4 times.
 * 3. In most years, only 1 or 2 times (7 exceptions).
 */
TEST_CASE("Rohini-bahulashtami-yoga calculations behave properly", "[.][jayanti]") {
    Location udupi{13'20'27_N,  74'45'06_E};
    const auto START_YEAR = 1800_y;
    const auto END_YEAR = 2399_y;

    Calc c{udupi};
    auto contains = [](const auto & container, const auto & item) -> bool{
        return std::find(container.cbegin(), container.cend(), item) != container.end();
    };

    for (auto year = START_YEAR; year < END_YEAR; year++) {
        const auto yogas = rohini_bahulashtami_yogas_in_year(c, year);

        REQUIRE(yogas);
        CAPTURE(year);
        CAPTURE(*yogas);

#ifndef CLIP_YOGA_BY_SIMHA
            REQUIRE(yogas->size() > 0);
#endif
        REQUIRE(yogas->size() <= 4);

        const auto count_with_simha_masa_at_midnight = std::count_if(yogas->cbegin(), yogas->cend(), [](auto & iter) -> bool {
                return iter.simha_masa_at_midnight;
        });
        CAPTURE(count_with_simha_masa_at_midnight);

        const std::vector<date::year> years_with_two_good_candidates = {
#ifdef CLIP_YOGA_BY_SIMHA
            2083_y, 2109_y, 2128_y, 2136_y, 2315_y, 2338_y, 2372_y, 2391_y,
#else
            1922_y, 2082_y, 2083_y, 2109_y, 2128_y, 2136_y, 2220_y, 2288_y,
            2315_y, 2338_y, 2361_y, 2372_y, 2391_y
#endif
            // 1922
            // [kalpa 2: 1922-08-16 00:51:44.282780 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 1922-09-13 00:53:17.470486 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X],
            // [kalpa 3: 1922-09-14 00:53:18.276637 UTC, Simha, no Rohini, K8, Rohini@SU, K8@SU, X]
            //
            // 2082
            // [kalpa 2: 2082-08-17 00:52:19.444609 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2082-09-14 00:53:29.703733 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X]
            //
            // 2083
            // [kalpa 1: 2083-09-03 00:53:20.114809 UTC, Simha, Rohini, K8, no Rohini@SU, no K8@SU, X],
            // [kalpa 1: 2083-09-04 00:53:21.548316 UTC, Simha, Rohini, K8, Rohini@SU, K8@SU, X]
            //
            // 2109
            // [kalpa 2: 2109-08-19 00:52:29.841213 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2109-09-16 00:53:31.390708 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X]
            //
            // 2128
            // [kalpa 2: 2128-08-18 00:52:28.211329 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2128-09-15 00:53:31.569987 UTC, Simha, Rohini, no K8, no Rohini@SU, no K8@SU, X],
            // [kalpa 3: 2128-09-16 00:53:31.881191 UTC, Simha, no Rohini, K8, Rohini@SU, K8@SU, X]
            //
            // 2136
            // [kalpa 2: 2136-08-20 00:52:40.084368 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2136-09-17 00:53:32.565717 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, -],
            // [kalpa 3: 2136-09-18 00:53:32.929103 UTC, no Simha, no Rohini, K8, no Rohini@SU, K8@SU, -]
            //
            // 2220
            // [kalpa 2: 2220-08-22 00:52:58.008230 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2220-09-19 00:53:38.631425 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, -],
            // [kalpa 4.1: 2220-09-20 00:53:38.910402 UTC, no Simha, no Rohini, no K8, no Rohini@SU, K8@SU, -]
            //
            // 2288
            // [kalpa 2: 2288-08-20 00:52:58.818887 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2288-09-17 00:53:42.759184 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X],
            // [kalpa 3: 2288-09-18 00:53:42.818568 UTC, Simha, no Rohini, K8, no Rohini@SU, K8@SU, -]
            //
            // 2315
            // [kalpa 2: 2315-08-23 00:53:07.528758 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2315-09-20 00:53:43.042144 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X]
            //
            // 2338
            // [kalpa 1: 2338-09-07 00:53:44.560100 UTC, Simha, Rohini, K8, no Rohini@SU, no K8@SU, X],
            // [kalpa 1: 2338-09-08 00:53:45.032638 UTC, Simha, Rohini, K8, Rohini@SU, K8@SU, X]
            //
            // 2361
            // [kalpa 2: 2361-08-24 00:53:21.540631 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2361-09-21 00:53:47.398387 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, -],
            // [kalpa 3: 2361-09-22 00:53:47.531920 UTC, no Simha, no Rohini, K8, no Rohini@SU, K8@SU, -]
            //
            // 2372
            // [kalpa 2: 2372-08-23 00:53:20.584370 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2372-09-20 00:53:48.789005 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X]
            //
            // 2391
            // [kalpa 2: 2391-08-24 00:53:24.059386 UTC, Simha, Rohini, no K8, no Rohini@SU, K8@SU, -],
            // [kalpa 2: 2391-09-21 00:53:49.626900 UTC, Simha, Rohini, no K8, Rohini@SU, no K8@SU, X],
        };

        if (!contains(years_with_two_good_candidates, year)) {
            check_that_there_is_only_one_viable_candidate(*yogas);
        }

        if (year != 1862_y) {
            /**
             * We know that in 1800..2399 there is only one year, 1862, where
             * there are no candidates after filtering for "Simha-masa
             * on midnight".
             */
            // [kalpa 3: 1862-09-15 00:53:17.339125 UTC, no Simha, no Rohini, K8, Rohini@SU, no K8@SU, -],

#ifdef CLIP_YOGA_BY_SIMHA
            REQUIRE(yogas->size() > 0);
#endif

            REQUIRE((count_with_simha_masa_at_midnight > 0 || year == 1862_y));

            /**
             * Make sure we never get "no kalpa" candidates, except 1862
             */
            REQUIRE(yogas->at(0).kalpa() != RoK8YogaKalpa::None);
        }

        /**
         * There are only few years with 3 or 4 candidates. And only two (1923 and 2074) with 3 candidate days in a row.
         */
        //   [1884-08-14 00:51:34.593796 UTC, true],[1884-09-11 00:53:14.882087 UTC, true],[1884-09-12 00:53:15.789224 UTC, true]
        //   [1922-08-16 00:51:44.282780 UTC, true],[1922-09-13 00:53:17.470486 UTC, true],[1922-09-14 00:53:18.276637 UTC, true]
        // 3![1923-09-02 00:53:01.616056 UTC, true],[1923-09-03 00:53:03.794157 UTC, true],[1923-09-04 00:53:05.792939 UTC, true]
        // 3![2074-08-15 00:52:04.057768 UTC, false],[2074-09-12 00:53:28.069584 UTC, true],[2074-09-13 00:53:28.544496 UTC, true],[2074-09-14 00:53:29.007217 UTC, true]
        //   [2128-08-18 00:52:28.211329 UTC, true],[2128-09-15 00:53:31.569987 UTC, true],[2128-09-16 00:53:31.881191 UTC, true]
        //   [2288-08-20 00:52:58.818887 UTC, true],[2288-09-17 00:53:42.759184 UTC, true],[2288-09-18 00:53:42.818568 UTC, true]
        //   [2383-08-22 00:53:15.048210 UTC, true],[2383-09-20 00:53:49.591012 UTC, true],[2383-09-21 00:53:49.605094 UTC, true]
        REQUIRE((count_with_simha_masa_at_midnight <= 2 || (year == 1884_y || year == 1922_y || year == 1923_y || year == 2074_y || year == 2128_y || year == 2288_y || year == 2383_y)));

        for (auto yoga : *yogas) {
            CAPTURE(yoga.midnight);
            CAPTURE(yoga.simha_masa_at_midnight);
            REQUIRE(yoga.sunrises.start.year_month_day().year() == year);

            /**
             * Yoga should be at least close to Simha masa, in the [-0.27 masas before..+0.61 masas after] range
             */
            // Known extreme cases:
            // Karkataka+0.732572862375668: 2194-08-10
            // Kanyā+0.5164679170465192: 1885-09-30
            // Kanyā+0.6022727516301067: 2343-10-10

            if (!yoga.simha_masa_at_midnight) {
                const auto saura_masa = c.saura_masa_at(yoga.midnight);
                REQUIRE(saura_masa > (Saura_Masa::Simha - 0.27));
                REQUIRE(saura_masa <= (Saura_Masa::Kanya + 0.61));
            }

            /**
             * Chandra masa check: ensure that it is usually Shravana or Bhadrapada,
             * with occasional Adhika-masas and very rare Ashadha. See std::vector-s
             * below.
             */

            const auto chandra_masa = c.chandra_masa_amanta(yoga.midnight);
            CAPTURE(chandra_masa);

            std::vector<date::year> adhikas_before_shravana{
                1806_y, 1928_y, 1966_y, 2061_y, 2069_y, 2145_y, 2183_y,
            };
            std::vector<date::year> adhikas_before_bhadrapada{
                1814_y, 1833_y, 1852_y, 1871_y, 1890_y, 1936_y, 1955_y, 1974_y,
                1993_y, 2012_y, 2031_y, 2050_y, 2096_y, 2115_y, 2134_y, 2153_y,
                2172_y, 2191_y, 2210_y, 2237_y, 2256_y, 2267_y, 2275_y, 2286_y,
                2294_y, 2313_y, 2332_y, 2351_y, 2370_y, 2397_y,
            };
            std::vector<date::year> adhikas_after_bhadrapada{};
            std::vector<date::year> ashadha_or_shravana{
                2194_y, 2213_y, 2373_y,
            };
            if (chandra_masa == Chandra_Masa::Adhika) {
                const auto saura_masa_frac = std::fmod(c.saura_masa_at(yoga.midnight).val, 1.0);
                const auto days_to_skip_till_next_mid_masa = Saura_Masa_Avg_Length * (0.5 - saura_masa_frac + 1.0);
                const auto next_chandra_masa = c.chandra_masa_amanta(yoga.midnight + double_days{days_to_skip_till_next_mid_masa});
                CAPTURE(next_chandra_masa);

                if (contains(adhikas_before_shravana, year)) {
                    REQUIRE(next_chandra_masa == Chandra_Masa::Shravana);
                } else if (contains(adhikas_before_bhadrapada, year)) {
                    REQUIRE(next_chandra_masa == Chandra_Masa::Bhadrapada);
                } else if (contains(adhikas_after_bhadrapada, year)) {
                    REQUIRE(next_chandra_masa == Chandra_Masa::Ashvin);
                } else {
                    REQUIRE(false);
                }
            } else {
                if (contains(ashadha_or_shravana, year)) {
                    REQUIRE(chandra_masa >= Chandra_Masa::Ashadha);
                    REQUIRE(chandra_masa <= Chandra_Masa::Shravana);
                } else {
                    REQUIRE(chandra_masa >= Chandra_Masa::Shravana);
                    REQUIRE(chandra_masa <= Chandra_Masa::Bhadrapada);
                }
            }

#ifdef DEBUG
//            fmt::print("Rohini-bahulashtami-yoga: {}\n", yoga.sunrise);
#endif
        }
    }
}

TEST_CASE("rohini_bahulashtami_yogas_in_year gives expected four kala cases for known dates", "[jayanti]") {
    Location udupi{13'20'27_N,  74'45'06_E};
    Calc c{udupi};

    auto check = [&](date::year year, date::year_month_day expected_ymd, RoK8YogaKalpa expected_kalpa) {
        const auto yogas = rohini_bahulashtami_yogas_in_year(c, year);
        REQUIRE(yogas);
        CAPTURE(*yogas);
        REQUIRE(yogas->at(0).midnight.year_month_day() == expected_ymd);
        REQUIRE(yogas->at(0).kalpa() == expected_kalpa);
    };

    check(2022_y, 2022_y/8/19, RoK8YogaKalpa::Kalpa4_1);
    check(2021_y, 2021_y/8/30, RoK8YogaKalpa::Kalpa1);
    check(2020_y, 2020_y/9/9, RoK8YogaKalpa::Kalpa2);
}

TEST_CASE("find_krishna_jayanti does not repeat past (August 19's) Jayanti before September's Indirā Ekādaśī", "[jayanti]") {
    Location udupi{13'20'27_N,  74'45'06_E};
    Calc c{udupi};

    const auto indira_ekadashi = c.find_next_vrata(date::local_days{2022_y/September/1});

    REQUIRE(indira_ekadashi);

    const auto jayanti = find_krishna_jayanti(*indira_ekadashi, c);
    REQUIRE(!jayanti.has_value());
    REQUIRE(std::holds_alternative<NoJayantiOnThisHalfMasa>(jayanti.error()));
}

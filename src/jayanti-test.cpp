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

    for (auto year = START_YEAR; year < END_YEAR; year++) {
        const auto yogas = rohini_bahulashtami_yogas_in_year(c, year);

        REQUIRE(yogas);
        CAPTURE(year);
        CAPTURE(*yogas);

        REQUIRE(yogas->size() > 0);
        REQUIRE(yogas->size() <= 4);

        const auto count_with_simha_masa_on_midnight = std::count_if(yogas->cbegin(), yogas->cend(), [](auto & iter) -> bool {
                return iter.simha_masa_on_midnight;
        });
        CAPTURE(count_with_simha_masa_on_midnight);

        /**
         * We know that in 1800..2399 there is only one year, 1862, where
         * there are no candidates after filtering for "Simha-masa
         * on midnight".
         */
        //   [1862-09-15 00:53:17.339125 UTC, false]
        REQUIRE((count_with_simha_masa_on_midnight > 0 || year == 1862_y));

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
        REQUIRE((count_with_simha_masa_on_midnight <= 2 || (year == 1884_y || year == 1922_y || year == 1923_y || year == 2074_y || year == 2128_y || year == 2288_y || year == 2383_y)));

        for (auto yoga : *yogas) {
            CAPTURE(yoga.midnight);
            CAPTURE(yoga.simha_masa_on_midnight);
            REQUIRE(yoga.sunrise.year_month_day().year() == year);

            /**
             * Yoga should be at least close to Simha masa, in the [-0.27 masas before..+0.61 masas after] range
             */
            // Known extreme cases:
            // Karkataka+0.732572862375668: 2194-08-10
            // Kanyā+0.5164679170465192: 1885-09-30
            // Kanyā+0.6022727516301067: 2343-10-10

            if (!yoga.simha_masa_on_midnight) {
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

            auto contains = [](const auto & container, const auto & item) -> bool{
                return std::find(container.cbegin(), container.cend(), item) != container.end();
            };

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

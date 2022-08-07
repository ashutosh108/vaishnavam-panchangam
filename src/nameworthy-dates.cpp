#include "nameworthy-dates.h"

#include "calc.h"
#include "html-util.h"
#include "masa.h"
#include "tithi.h"

#include <array>
#include <string>

namespace {
std::string paran_title(const vp::Paran & paran) {
    std::string title;

    if (paran.paran_start) {
        title += fmt::format(FMT_STRING("{} ({})"), paran.start_str_seconds(), paran.start_type());
    }
    title += "…";
    if (paran.paran_end) {
        title += fmt::format(FMT_STRING("{} ({})"), paran.end_str_seconds(), paran.end_type());
    }
    if (paran.paran_limit) {
        const auto limit_str = date::format("%H:%M:%S", date::floor<std::chrono::seconds>(paran.paran_limit->as_zoned_time(paran.time_zone).get_local_time()));
        title += fmt::format(FMT_STRING(", absolute limit is {} (dvādaśī end)"), limit_str);
    }
    return title;
}
}

void insert_ekadashi_paran_etc(vp::NamedDates & dates, const vp::Vrata & vrata) {
    dates.emplace(vrata.date, vp::NamedDate{fmt::format(FMT_STRING("{} Ekādaśī"), vrata.ekadashi_name()), "", "vrata"});
    const auto day1_additional_event_name = vrata.day1_additional_event_name();
    if (!day1_additional_event_name.empty()) {
        dates.emplace(vrata.date, vp::NamedDate{day1_additional_event_name, "", "vrata"});
    }
    const auto day2_additional_event_name = vrata.day2_additional_event_name();
    if (!day2_additional_event_name.empty()) {
        dates.emplace(vrata.date + date::days{1}, vp::NamedDate{day2_additional_event_name, "", "vrata"});
    }
    // 'c' means compact formatting ("*" for standard pAraNam, otherwise something like ">06:45", "<07:45" or "06:45-07.45")
    const auto paran = fmt::format(FMT_STRING("{:c}"), vrata.paran);
    const auto paran_with_href = fmt::format(FMT_STRING(R"(<a href="#{}">{}</a>)"), html::escape_attribute(vrata.location_name()), html::escape_attribute(paran));
    dates.emplace(vrata.local_paran_date(), vp::NamedDate{paran_with_href, paran_title(vrata.paran), ""});

    if (const auto harivasara = vrata.harivasara()) {
        const auto harivasara_local = harivasara->as_zoned_time(vrata.location.time_zone()).get_local_time();
        const auto harivasara_date = date::floor<date::days>(harivasara_local);
        const auto h_m = date::format("%H:%M", harivasara_local);
        std::string str;
        if (harivasara_date != vrata.date) {
            str = fmt::format(FMT_STRING("HV > {}"), h_m);
        } else {
            str = fmt::format(FMT_STRING("HV > {} <small>{}</small>"), h_m, date::format("%d.%m", harivasara_date));
        }
        dates.emplace(vrata.date - date::days{1}, vp::NamedDate{str});
    }
}

static tl::expected<date::local_days, vp::CalcError>
local_sun_date_covering_given_time(vp::Swe & swe, const date::time_zone * time_zone, vp::JulDays_UT time) {
    const auto sunset = swe.next_sunset(time);
    if (!sunset) { return tl::make_unexpected(sunset.error()); }

    const auto sunset_local = sunset->as_zoned_time(time_zone).get_local_time();
    return date::floor<date::days>(sunset_local) - date::days{1};
}

/**
 * @brief find_krishna_jayanti: find Krishna Jayanti day, IF it is found in this half of Chaandra-maasa
 * @param vrata Ekadashi vrata for this half of Chaandra-maasa
 * @param calc  Calculator to be used.
 * @return
 */
static tl::expected<date::local_days, vp::CalcError>
find_krishna_jayanti(const vp::Vrata & vrata, vp::Calc & calc) {
    auto base_time = vrata.sunrise1 - vp::double_days{10};
    const auto ashtami_start = calc.find_exact_tithi_start(base_time, vp::Tithi::Krishna_Ashtami());
    const auto ashtami_end = calc.find_exact_tithi_start(ashtami_start, vp::Tithi::Krishna_Ashtami_End());
    const auto rohini_start = calc.find_nakshatra_start(base_time, vp::Nakshatra::ROHINI_START());
    const auto rohini_end = calc.find_nakshatra_start(rohini_start, vp::Nakshatra::ROHINI_END());

    const auto intersection_start = std::max(ashtami_start, rohini_start);
    const auto intersection_end = std::min(ashtami_end, rohini_end);

    // Krishna Jayanti requires Krishna-ashtami and Rohini to overlap
    if (intersection_start > intersection_end) {
        return tl::make_unexpected(vp::CalcError{vp::NoRohiniAshtamiIntersectionForJayanti{}});
    }

    const auto min_time = std::min(ashtami_start, rohini_start);
    const auto max_time = std::max(ashtami_end, rohini_end);

    /**
     * ‘Rohiṇī-bahulāṣṭamī-yoga’ is defined as a combination of Rohiṇī-nakṣatra
     * and Bahulāṣṭamī in the period of time from a sunrise to a next sunrise
     * (a definition of a ‘day’). In the majority of cases this combination
     * is in the form of an overlap, but in some rare years nakṣatra and tithi
     * start or end during the same day without overlapping.
     *
     * Current 2022 CE is one of such years, while the previous one was 2003 CE.
     *
     *
     * 1st Kalpa
     *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra both Rohiṇī
     *      and Bahulāṣṭamī are present. These are the ideal astronomical
     *      circumstances, coinsiding with those at the actual Appearance
     *      of Bhagavān Śrī-Kṛṣṇa, ‘sampūrṇo jayantī-kalpaḥ’.
     * 2nd Kalpa
     *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra only Rohiṇī
     *      is present.
     * 3rd Kalpa
     *      Rohiṇī-bahulāṣṭamī-yoga, at the time of ardharātra only Bahulāṣṭamī
     *      is present.
     * 4th Kalpa
     *      Rohiṇī-bahulāṣṭamī-yoga, neither Rohiṇī, nor Bahulāṣṭamī is present
     *      at the time of ardharātra. This day is Jayantī nevertheless,
     *      provided Kalpas 1-3 are not to be found.
     *
     * FIXME:
     *      Please note that currently the code does not quite match
     *      the description above. The code needs to be updated to match.
     */

    const auto midnight1 = calc.first_midnight_after(intersection_start);
    if (!midnight1) {
        return tl::make_unexpected(midnight1.error());
    }
    if (*midnight1 < intersection_end) {
        /**
         * 1-е калпо (первый разсматриваемый варіант соблюденія условій):
         * на полночь приходятся и крьшн̣āшт̣амӣ, и Рохин̣ӣ.
         */
        return local_sun_date_covering_given_time(calc.swe, vrata.location.time_zone(), *midnight1);
    }

    const auto midnight2 = calc.first_midnight_after(rohini_start);
    if (!midnight2) {
        return tl::make_unexpected(midnight2.error());
    }
    if (*midnight2 < rohini_end) {
        /**
          * 2-е калпо: в полночь есть Рохин̣ӣ, которая пересѣкается с ашт̣амью
          * в другое время.
          */
        return local_sun_date_covering_given_time(calc.swe, vrata.location.time_zone(), *midnight2);
    }

    const auto midnight3 = calc.first_midnight_after(ashtami_start);
    if (!midnight3) {
        return tl::make_unexpected(midnight3.error());
    }
    if (*midnight3 < ashtami_end) {
        /**
         * 3-е калпо: в полночь есть ашт̣амӣ, которая пересѣкается с Рохин̣ью
         * в другое время.
         */
        return local_sun_date_covering_given_time(calc.swe, vrata.location.time_zone(), *midnight3);
    }

    return tl::make_unexpected(vp::CalcError{vp::NoRohiniAshtamiIntersectionForJayanti{}});
}

vp::NamedDates vp::nameworthy_dates_for_this_paksha(const vp::Vrata &vrata, CalcFlags flags)
{
    vp::NamedDates dates;
    auto calc = vp::Calc{Swe{vrata.location, flags}};

    insert_ekadashi_paran_etc(dates, vrata);
    if (vrata.masa == vp::Chandra_Masa::Magha && vrata.paksha == vp::Paksha::Shukla) {
        auto base_time = vrata.sunrise1 - double_days{10};

        struct TithiWithName {
            vp::DiscreteTithi tithi;
            const char * name;
        };
        static TithiWithName tithis_with_name[] = {
            { vp::DiscreteTithi::Shukla_Panchami(), "Vasanta-pañcamī" },    // Vasanta-pa~ncamii
            { vp::DiscreteTithi::Shukla_Saptami(), "Ratha-saptamī" },       // Ratha-saptamii
            { vp::DiscreteTithi::Shukla_Ashtami(), "Bhīṣmāṣṭamī" },         // Bhi.smaa.s.tamii
            { vp::DiscreteTithi::Shukla_Navami(), "Madhva-navamī (cāndra)" }, // Madhva-navamii (caandra)
            { vp::DiscreteTithi::Purnima(), "Pūrṇimā, Māgha-snāna-vrata ends" }, // Puur.nimaa, Maagha-snaana-vrata ends
        };

        for (const auto & tithi_with_name : tithis_with_name) {
            const auto date = calc.find_exact_tithi_date(base_time, tithi_with_name.tithi, vrata.location.time_zone());
            if (date) {
                dates.emplace(*date, NamedDate{tithi_with_name.name, "", "custom"});
            }
        }
    }
    else if ((vrata.masa == vp::Chandra_Masa::Shravana || vrata.masa == vp::Chandra_Masa::Bhadrapada) && vrata.paksha == vp::Paksha::Krishna) {
        // potential Krishna Jayanti. Find out.
        if (const auto date = find_krishna_jayanti(vrata, calc); date) {
            dates.emplace(*date, NamedDate{"Śrī-Kṛṣṇa-jayantī", "", "custom"});
            dates.emplace(*date+date::days{1}, NamedDate{"Śrī-Kṛṣṇa-līlotsava", "", "custom"});
        }
    }
    return dates;
}

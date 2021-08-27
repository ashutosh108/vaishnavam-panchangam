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

#ifdef KRISHNA_JAYANTI
static tl::expected<date::local_days, vp::CalcError>
local_sun_date_covering_given_time(vp::Swe & swe, const date::time_zone * time_zone, vp::JulDays_UT time) {
    const auto sunset = swe.find_sunset(time);
    if (!sunset) { return tl::make_unexpected(sunset.error()); }

    const auto sunset_local = sunset->as_zoned_time(time_zone).get_local_time();
    return date::floor<date::days>(sunset_local) - date::days{1};
}

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
     * 1-е калпо (первый разсматриваемый варіант соблюденія условій):
     *      на полночь приходятся и крьшн̣āшт̣амӣ, и Рохин̣ӣ.
     * 2-е калпо: в полночь есть Рохин̣ӣ, которая пересѣкается с ашт̣амью
     *      в другое время.
     * 3-е калпо: в полночь есть ашт̣амӣ, которая пересѣкается с Рохин̣ью
     *      в другое время.
     * 4-е калпо: в полночь нѣт ни ашт̣амӣ, ни Рохин̣ӣ, но в этот день есть
     *      ашт̣амӣ и Рохин̣ӣ, и онѣ пересѣкаются. В этом случаѣ предпочтеніе
     *      отдается тому дню, в который происходит пересѣченіе ашт̣амӣ
     *      и Рохин̣ӣ, если таких дня два, то предпочтеніе тому дню,
     *      в котором это сочетаніе приходится на Сӯрйодай.
     */

    const auto midnight1 = calc.swe.first_midnight_after(intersection_start);
    if (!midnight1) {
        return tl::make_unexpected(vp::CalcError{vp::NoRohiniAshtamiIntersectionForJayanti{}});
    }
    if (*midnight1 < intersection_end) {
        /**
         * 1-е калпо (первый разсматриваемый варіант соблюденія условій):
         * на полночь приходятся и крьшн̣āшт̣амӣ, и Рохин̣ӣ.
         */
        return local_sun_date_covering_given_time(calc.swe, vrata.location.time_zone(), *midnight1);
    }

    const auto sunset1 = calc.swe.find_sunset(intersection_start - vp::double_days{1});

    const auto intersection_start_local = intersection_start.as_zoned_time(vrata.location.time_zone()).get_local_time();
    const auto intersection_end_local = intersection_end.as_zoned_time(vrata.location.time_zone()).get_local_time();

    const auto intersection_start_date = date::floor<date::days>(intersection_start_local);
    const auto intersection_end_date = date::floor<date::days>(intersection_end_local);

    if (intersection_start_date < intersection_end_date) {
        return intersection_start_date;
    }
    return tl::make_unexpected(vp::CalcError{vp::NoRohiniAshtamiIntersectionForJayanti{}});
}
#endif

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
            { vp::DiscreteTithi::Shukla_Panchami(), "Vasanta-pañcamī" },
            { vp::DiscreteTithi::Shukla_Saptami(), "Ratha-saptamī" },
            { vp::DiscreteTithi::Shukla_Ashtami(), "Bhīṣmāṣtamī" },
            { vp::DiscreteTithi::Shukla_Navami(), "Madhva-navamī (cāndra)" },
            { vp::DiscreteTithi::Purnima(), "Pūrṇimā, End of Māgha-snāna-vrata" },
        };

        for (const auto & tithi_with_name : tithis_with_name) {
            const auto date = calc.find_exact_tithi_date(base_time, tithi_with_name.tithi, vrata.location.time_zone());
            if (date) {
                dates.emplace(*date, NamedDate{tithi_with_name.name, "", "custom"});
            }
        }
    }
#   ifdef KRISHNA_JAYANTI
    else if ((vrata.masa == vp::Chandra_Masa::Shravana || vrata.masa == vp::Chandra_Masa::Bhadrapada) && vrata.paksha == vp::Paksha::Krishna) {
        // potential Krishna Jayanti. Find out.
        if (const auto date = find_krishna_jayanti(vrata, calc); date) {
            dates.emplace(*date, NamedDate{"Krishna Jayanti", "", "custom"});
        }
    }
#   endif
    return dates;
}

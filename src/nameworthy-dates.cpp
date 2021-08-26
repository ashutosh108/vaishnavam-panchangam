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

vp::NamedDates vp::nameworthy_dates_for_this_paksha(const vp::Vrata &vrata, CalcFlags flags)
{
    vp::NamedDates dates;
    insert_ekadashi_paran_etc(dates, vrata);
    if (vrata.masa == vp::Chandra_Masa::Magha && vrata.paksha == vp::Paksha::Shukla) {
        auto base_time = vrata.sunrise1 - double_days{10};
        auto calc = vp::Calc{Swe{vrata.location, flags}};

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
    return dates;
}

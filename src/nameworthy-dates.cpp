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
    dates.emplace(vrata.date, vp::NamedDate{fmt::format(FMT_STRING("{} {}"), vrata.ekadashi_name(), vrata.type)});
    if (vp::is_atirikta(vrata.type)) {
        dates.emplace(vrata.date + date::days{1}, vp::NamedDate{fmt::format(FMT_STRING("{} {}"), vrata.ekadashi_name(), vrata.type)});
    }
    // 'c' means compact formatting ("*" for standard pAraNam, otherwise something like ">06:45", "<07:45" or "06:45-07.45")
    const auto paran = fmt::format(FMT_STRING("{:c}"), vrata.paran);
    const auto paran_with_href = fmt::format(FMT_STRING(R"(<a href="#{}">{}</a>)"), html::escape_attribute(vrata.location_name()), html::escape_attribute(paran));
    dates.emplace(vrata.local_paran_date(), vp::NamedDate{paran_with_href, paran_title(vrata.paran)});
    // TODO: specify "vrata" class for NamedDate?
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
                dates.emplace(*date, NamedDate{tithi_with_name.name});
            }
        }
    }
    return dates;
}

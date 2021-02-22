#include "nameworthy-dates.h"

#include "calc.h"
#include "masa.h"
#include "tithi.h"

#include <array>
#include <string>

vp::NamedDates vp::nameworthy_dates_for_this_paksha(const vp::Vrata &vrata)
{
    // TODO: complete this function.
    vp::NamedDates dates;
    if (vrata.masa == vp::Chandra_Masa::Magha && vrata.paksha == vp::Paksha::Shukla) {
        auto base_time = vrata.sunrise1 - double_days{10};
        auto calc = vp::Calc{vrata.location};

        struct TithiWithName {
            vp::DiscreteTithi tithi;
            const char * name;
        };
        static TithiWithName tithis_with_name[] = {
            { vp::DiscreteTithi::Shukla_Panchami(), "Vasanta-pañcamī" },
            { vp::DiscreteTithi::Shukla_Saptami(), "Ratha-saptamī" },
            { vp::DiscreteTithi::Shukla_Ashtami(), "Bhīṣmāṣtamī" },
            { vp::DiscreteTithi::Shukla_Navami(), "Madhva-navamī (cāndra)" },
        };

        for (const auto & tithi_with_name : tithis_with_name) {
            const auto date = calc.find_exact_tithi_date(base_time, tithi_with_name.tithi, vrata.location.time_zone());
            if (date) {
                dates[*date] = NamedDate{tithi_with_name.name};
            }
        }
    }
    return dates;
}

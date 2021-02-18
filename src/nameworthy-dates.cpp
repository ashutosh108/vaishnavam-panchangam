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
        auto base_time = *vrata.sunrise0 - double_days{10};
        auto calc = vp::Calc{vrata.location};
        auto date = calc.find_exact_tithi_date(base_time, vp::DiscreteTithi::Shukla_Panchami());
        dates.emplace_back(date, "Vasanta-pañcamī");
    }
    return dates;
}

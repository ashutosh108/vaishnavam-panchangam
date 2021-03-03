#ifndef NAMEWORTHYDATES_H
#define NAMEWORTHYDATES_H

#include "calc-flags.h"
#include "named-dates.h"
#include "vrata.h"

namespace vp {
NamedDates nameworthy_dates_for_this_paksha(const Vrata & vrata, CalcFlags flags);
}

#endif // NAMEWORTHYDATES_H

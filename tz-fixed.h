/* Since we need to include tz from few places and we need to disable warning every time,
 * we define our own include. */

#ifndef TZ_FIXED_H
#define TZ_FIXED_H

#include "disable-warning.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_SHADOW
#include "date/tz.h"
DISABLE_WARNING_POP

#endif

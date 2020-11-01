#ifndef EDIT_CUSTOM_DATES_H
#define EDIT_CUSTOM_DATES_H

#include "table-calendar-generator.h"

class QWidget;
class QString;

namespace vp {

namespace detail {
vp::Custom_Dates qstring_to_custom_dates(const QString & str);
QString custom_dates_to_qstring(const vp::Custom_Dates & custom_dates);
}

vp::Custom_Dates edit_custom_dates(const vp::Custom_Dates & custom_dates, QWidget * parent);


}


#endif // EDIT_CUSTOM_DATES_H

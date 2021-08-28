#include "named-dates.h"

namespace vp {

bool operator ==(const vp::NamedDate &d1, const vp::NamedDate &d2) {
    return d1.name == d2.name && d1.title == d2.title && d1.css_classes == d2.css_classes;
}

}

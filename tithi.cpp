#include <cassert>
#include <ostream>

#include "tithi.h"

std::ostream &operator <<(std::ostream &o, Tithi const & t) {
    assert(t.tithi >= 0 && t.tithi < 30);
    constexpr const static char *tithi_names[] = {
        "Pratipat",
        "Dvitiya",
        "Tritiya",
        "Chaturthi",
        "Panchami",
        "Shashthi",
        "Saptami",
        "Ashtami",
        "Navami",
        "Dashami",
        "Ekadashi",
        "Dvadashi",
        "Trayodashi",
        "Chaturdashi"
    };
    int int_tithi = static_cast<int>(t.tithi);
    if (int_tithi < 15) {
        if (int_tithi < 14) {
            o << "Shukla " << tithi_names[int_tithi];
        } else {
            o << "Purnima";
        }
    } else {
        if (int_tithi < 29) {
            o << "Krishna " << tithi_names[int_tithi-15];
        } else {
            o << "Amavasya";
        }
    }
    o << "(" << (t.tithi - int_tithi) << ")";
    return o;
}

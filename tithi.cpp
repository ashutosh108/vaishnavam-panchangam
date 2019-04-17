#include <cassert>
#include <cmath>
#include <ostream>
#include <sstream>

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

Tithi::Tithi(double _tithi) : tithi(_tithi)
{
    if (tithi < 0 || tithi >= 30) {
        std::stringstream s;
        s << "Wrong tithi value: " << tithi;
        throw std::range_error(s.str());
    }
}

Paksha Tithi::get_paksha()
{
    return tithi < 15 ? Paksha::Shukla : Paksha::Krishna;
}

bool Tithi::is_ekadashi()
{
    return (tithi >= 10 && tithi < 11) || (tithi >= 10+15 && tithi < 11+15);
}

bool Tithi::is_dashami()
{
    return (tithi >= 9 && tithi < 10) || (tithi >= 9+15 && tithi < 10+15);
}

bool operator ==(const Tithi &t1, const Tithi &t2)
{
    constexpr double epsilon = 1e-6;
    return ( fabs(t1.tithi - t2.tithi) < epsilon );
}

bool operator <(const Tithi &t1, const Tithi &t2)
{
    return t1.tithi < t2.tithi;
}

bool operator >(const Tithi &t1, const Tithi &t2)
{
    return t1.tithi > t2.tithi;
}

bool operator !=(const Tithi &t1, const Tithi &t2)
{
    return !(t1==t2);
}

bool operator <=(const Tithi &t1, const Tithi &t2)
{
    return !(t1 > t2);
}

bool operator >=(const Tithi &t1, const Tithi &t2)
{
    return !(t1 < t2);
}

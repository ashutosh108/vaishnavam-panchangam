#include "tithi.h"

#include <cassert>
#include <cmath>
#include <ostream>
#include <sstream>

namespace vp {

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

bool Tithi::is_dvadashi()
{
    return (tithi >= 11 && tithi < 12) || (tithi >= 11+15 && tithi < 12+15);
}

bool Tithi::is_ekadashi()
{
    return (tithi >= 10 && tithi < 11) || (tithi >= 10+15 && tithi < 11+15);
}

bool Tithi::is_dashami()
{
    return (tithi >= 9 && tithi < 10) || (tithi >= 9+15 && tithi < 10+15);
}

Tithi &Tithi::operator +=(const double delta)
{
    tithi += delta;
    return *this;
}

// how many tithis from us to that tithi, counting only forward
double Tithi::positive_delta_until_tithi(Tithi target) const
{
    double delta = target.tithi - this->tithi;
    if (delta < 0) delta += 30.0;
    return delta;
}

// how many tithis to the closest (backward or forward) target tithi
double Tithi::delta_to_nearest_tithi(Tithi target) const
{
    double delta = target.tithi - this->tithi;
    if (delta < -15.0) {
        delta += 30.0;
    } else if (delta > 15.0) {
        delta -= 30.0;
    }
    return delta;
}

bool operator ==(const Tithi &t1, const Tithi &t2)
{
    // 2e-11 is chosen to give better than half of millionth of a second precision:
    // ~1e5 seconds per day, 1e6 millionths per second, *2 to be better than
    // half of millionth of digit.
    constexpr double epsilon = 2e-11;
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

Tithi operator +(const Tithi &t, double delta)
{
    return Tithi{t.tithi+delta};
}

double operator -(const Tithi &t1, const Tithi &t2)
{
    return t1.tithi - t2.tithi;
}

Tithi operator -(const Tithi &t, double delta)
{
    return Tithi{t.tithi - delta};
}

} // namespace vp

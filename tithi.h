#ifndef TITHI_H
#define TITHI_H

#include <ostream>

enum class Paksha {
    Shukla,
    Krishna
};

struct Tithi {
    explicit Tithi(double _tithi);
    double tithi;
    Paksha get_paksha();
    bool is_ekadashi();
    bool is_dashami();
    // Pratipat is 0.00...0.99, Dvitiya is 1.00..1.99, etc
    static constexpr double Dashami = 9.0;
    static constexpr double Ekadashi = 10.0;
    static constexpr double Dvadashi = 11.0;
    static constexpr double Trayodashi = 12.0;
    friend bool operator ==(Tithi const &t1, Tithi const &t2);
    friend bool operator !=(Tithi const &t1, Tithi const &t2);
    friend bool operator <(Tithi const &t1, Tithi const &t2);
    friend bool operator <=(Tithi const &t1, Tithi const &t2);
    friend bool operator >(Tithi const &t1, Tithi const &t2);
    friend bool operator >=(Tithi const &t1, Tithi const &t2);
};

std::ostream &operator <<(std::ostream &o, Tithi const & t);

#endif // TITHI_H

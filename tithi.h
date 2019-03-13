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
};

std::ostream &operator <<(std::ostream &o, Tithi const & t);

#endif // TITHI_H

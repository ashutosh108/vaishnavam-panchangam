#ifndef TITHI_H
#define TITHI_H

#include <ostream>

struct Tithi {
    explicit Tithi(double _tithi) : tithi(_tithi){}
    double tithi;
};

std::ostream &operator <<(std::ostream &o, Tithi const & t);

enum class Paksha {
    Shukla,
    Krishna
};

#endif // TITHI_H

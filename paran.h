#ifndef PARAN_H
#define PARAN_H

#include <optional>
#include <ostream>
#include <tuple>

#include "swe_time.h"

class Paran
{
public:
    enum class Type {
        standard,
        before
    };
    Paran():type(Type::standard){}
    Paran(Type _type, Swe_Time _time):type(_type), time1(_time){}
    bool operator==(Paran const &other) const {
        return std::tie(type, time1) == std::tie(other.type, other.time1);
    }
    bool operator!=(Paran const &other) const {
        return !(*this == other);
    }
    std::ostream& operator<<(std::ostream &o) const;
private:
    Type type;
    std::optional<Swe_Time> time1;
};

std::ostream & operator<<(std::ostream &o, Paran const &p);

#endif // PARAN_H

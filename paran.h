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
        Standard, Until_Dvadashi_End, From_Quarter_Dvadashi
    };
    Paran(Type _type = Type::Standard, std::optional<Swe_Time> _paran_start = std::nullopt, std::optional<Swe_Time> _paran_end = std::nullopt):
        type(_type), paran_start(_paran_start), paran_end(_paran_end){}
    bool operator==(Paran const &other) const {
        return std::tie(type, paran_start, paran_end) == std::tie(other.type, other.paran_start, other.paran_end);
    }
    bool operator!=(Paran const &other) const {
        return !(*this == other);
    }
    std::ostream& operator<<(std::ostream &o) const;

    Type type;
    std::optional<Swe_Time> paran_start{};
    std::optional<Swe_Time> paran_end{};
};

std::ostream & operator<<(std::ostream &o, Paran const &p);
std::ostream & operator<<(std::ostream &o, Paran::Type const &t);


#endif // PARAN_H

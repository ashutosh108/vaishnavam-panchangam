#ifndef PARAN_H
#define PARAN_H

#include <optional>
#include <ostream>
#include <tuple>

#include "swe_time.h"

class Paran
{
public:
    Paran() = default;
    Paran(std::optional<Swe_Time> _paran_start, std::optional<Swe_Time> _paran_end):
        paran_start(_paran_start), paran_end(_paran_end){}
    bool operator==(Paran const &other) const {
        return std::tie(paran_start, paran_end) == std::tie(other.paran_start, other.paran_end);
    }
    bool operator!=(Paran const &other) const {
        return !(*this == other);
    }
    std::ostream& operator<<(std::ostream &o) const;
private:
    std::optional<Swe_Time> paran_start{};
    std::optional<Swe_Time> paran_end{};
};

std::ostream & operator<<(std::ostream &o, Paran const &p);

#endif // PARAN_H

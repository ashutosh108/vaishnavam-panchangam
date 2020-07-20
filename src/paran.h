#ifndef PARAN_H
#define PARAN_H

#include "juldays_ut.h"

#include <fmt/format.h>
#include <optional>
#include <string_view>
#include <tuple>

namespace vp {

class Paran
{
public:
    enum class Type {
        Standard, Until_Dvadashi_End, From_Quarter_Dvadashi, Puccha_Dvadashi
    };
    Paran(Type _type = Type::Standard, std::optional<JulDays_UT> _paran_start = std::nullopt, std::optional<JulDays_UT> _paran_end = std::nullopt):
        type(_type), paran_start(_paran_start), paran_end(_paran_end){}
    bool operator==(Paran const &other) const {
        return std::tie(type, paran_start, paran_end) == std::tie(other.type, other.paran_start, other.paran_end);
    }
    bool operator!=(Paran const &other) const {
        return !(*this == other);
    }

    Type type;
    std::optional<JulDays_UT> paran_start{};
    std::optional<JulDays_UT> paran_end{};
};

class ParanFormatter {
public:
    static std::string format(
            const Paran &paran,
            const char * timezone_name,
            const char * paran_start_format = "%H:%M:%S",
            const char * separator = "-",
            const char * paran_end_format = "%H:%M:%S",
            const char * paran_type_separator = nullptr
            );
};

} // namespace vp

template<>
struct fmt::formatter<vp::Paran::Type> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const vp::Paran::Type & t, FormatContext & ctx) {
        switch (t) {
        case vp::Paran::Type::Standard:
            return fmt::format_to(ctx.out(), "Standard pāraṇam: from sunrise until 1/5 of day-time"); break;
        case vp::Paran::Type::Until_Dvadashi_End:
            return fmt::format_to(ctx.out(), "Pāraṇam before the end of Dvādaśī"); break;
        case vp::Paran::Type::From_Quarter_Dvadashi:
            return fmt::format_to(ctx.out(), "Pāraṇam after the first ¼ of Dvādaśī"); break;
        case vp::Paran::Type::Puccha_Dvadashi:
            return fmt::format_to(ctx.out(), "Pāraṇam within Puccha-Dvādaśī"); break;
        }
        return fmt::format_to(ctx.out(), "Unknown Pāraṇam type: {}", static_cast<int>(t));
    }
};

template<>
struct fmt::formatter<vp::Paran> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const vp::Paran & p, FormatCtx & ctx) {
        fmt::format_to(ctx.out(), "{}{{", p.type);
        if (p.paran_start.has_value()) {
            fmt::format_to(ctx.out(), "{}", *p.paran_start);
        }
        fmt::format_to(ctx.out(), "..");
        if (p.paran_end.has_value()) {
            fmt::format_to(ctx.out(), "{}", *p.paran_end);
        }
        return fmt::format_to(ctx.out(), "}}");
    }
};

#endif // PARAN_H

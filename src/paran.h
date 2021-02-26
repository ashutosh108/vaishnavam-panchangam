#ifndef PARAN_H
#define PARAN_H

#include "juldays_ut.h"

#include "fmt-format-fixed.h"
#include <optional>
#include <string_view>
#include <tuple>
#include "tz-fixed.h"

namespace vp {

namespace {
const date::time_zone * utc() {
    static auto _utc = date::locate_zone("UTC");
    return _utc;
}
}

class Paran
{
public:
    enum class Type {
        Standard, From_Quarter_Dvadashi, Puccha_Dvadashi
    };
    enum class StartType {
        Sunrise, QuarterOfDvadashi, Unspecified
    };
    enum class EndType {
        OneFifthOfDaytime, EndOfDvadashi, Unspecified
    };

    Paran(Type _type = Type::Standard,
          std::optional<JulDays_UT> _paran_start = std::nullopt,
          std::optional<JulDays_UT> _paran_end = std::nullopt,
          const date::time_zone * _time_zone = utc()
          ): type(_type), paran_start(_paran_start), paran_end(_paran_end), time_zone(_time_zone){
        if (paran_start > paran_end && paran_end) {
            throw std::runtime_error(fmt::format("internal error: paran_start({}) is later than paran_end({})", paran_start, paran_end));
        }
    }
    bool operator==(Paran const &other) const {
        return std::tie(type, paran_start, paran_end) == std::tie(other.type, other.paran_start, other.paran_end);
    }
    bool operator!=(Paran const &other) const {
        return !(*this == other);
    }
    std::string start_str() const;
    std::string start_str_seconds() const;
    std::string end_str() const;
    std::string end_str_seconds() const;
    bool is_rounded_to_minutes() const;
    StartType start_type() const;
    EndType end_type() const;

    Type type;
    std::optional<JulDays_UT> paran_start{};
    std::optional<JulDays_UT> paran_end{};
    const date::time_zone * time_zone;
};

class ParanFormatter {
public:
    static std::string format(
            const Paran &paran,
            const date::time_zone * time_zone,
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
        case vp::Paran::Type::Puccha_Dvadashi:
            return fmt::format_to(ctx.out(), "Pāraṇam before the end of Dvādaśī (Puccha-Dvādaśī)"); break;
        case vp::Paran::Type::From_Quarter_Dvadashi:
            return fmt::format_to(ctx.out(), "Pāraṇam after the first ¼ of Dvādaśī"); break;
        }
        return fmt::format_to(ctx.out(), "Unknown Pāraṇam type: {}", static_cast<int>(t));
    }
};

template<>
struct fmt::formatter<vp::Paran> : fmt::formatter<std::string_view> {
    bool use_compact_form = false;

    template<typename ParseCtx>
    auto parse(ParseCtx & ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it == 'c') {
            ++it;
            use_compact_form = true;
        }
        return it;
    }

    template<typename FormatCtx>
    auto format_compact(const vp::Paran & p, FormatCtx & ctx) {
        assert(p.time_zone != nullptr);
        if (p.type == vp::Paran::Type::Standard) {
            return fmt::format_to(ctx.out(), "*");
        } else if (p.paran_start && !p.paran_end) {
            return fmt::format_to(ctx.out(), ">{}", p.start_str());
        } else if (!p.paran_start && p.paran_end) {
            return fmt::format_to(ctx.out(), "<{}", p.end_str());
        } else if (p.paran_start && p.paran_end) {
            return fmt::format_to(ctx.out(), "{}–{}", p.start_str(), p.end_str());
        } else {
            return format_default(p, ctx);
        }
    }

    template<typename FormatCtx>
    auto format_default(const vp::Paran & p, FormatCtx & ctx) {
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

    template<typename FormatCtx>
    auto format(const vp::Paran & p, FormatCtx & ctx) {
        if (use_compact_form) {
            return format_compact(p, ctx);
        } else {
            return format_default(p, ctx);
        }
    }
};

template<>
struct fmt::formatter<vp::Paran::StartType> : fmt::formatter<std::string_view> {
    template <typename FormatCtx>
    auto format(vp::Paran::StartType start_type, FormatCtx & ctx) {
        switch (start_type) {
        case vp::Paran::StartType::Sunrise:
            return fmt::format_to(ctx.out(), "sunrise");
        case vp::Paran::StartType::QuarterOfDvadashi:
            return fmt::format_to(ctx.out(), "1/4th of dvādaśī");
        default:
            return fmt::format_to(ctx.out(), "unspecified");
        }
    }
};

template<>
struct fmt::formatter<vp::Paran::EndType> : fmt::formatter<std::string_view> {
    template <typename FormatCtx>
    auto format(vp::Paran::EndType end_type, FormatCtx & ctx) {
        switch (end_type) {
        case vp::Paran::EndType::OneFifthOfDaytime:
            return fmt::format_to(ctx.out(), "1/5th of daytime");
        case vp::Paran::EndType::EndOfDvadashi:
            return fmt::format_to(ctx.out(), "end of dvādaśī");
        default:
            return fmt::format_to(ctx.out(), "unspecified");
        }
    }
};

#endif // PARAN_H

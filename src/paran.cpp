#include "paran.h"

namespace vp {

std::string ParanFormatter::format(const Paran &paran,
                                   const date::time_zone * time_zone,
                                   const char * paran_start_format,
                                   const char * separator,
                                   const char * paran_end_format,
                                   const char * paran_type_separator
                                   )
{
    fmt::memory_buffer buf;
    if (paran.paran_start.has_value()) {
        auto rounded_up = date::ceil<std::chrono::seconds>(paran.paran_start->as_sys_time());
        auto zoned = date::make_zoned(time_zone, rounded_up);
        fmt::format_to(buf, "{}", date::format(paran_start_format, zoned));
    } else {
        fmt::format_to(buf, "...");
    }
    fmt::format_to(buf, "{}", separator);
    if (paran.paran_end.has_value()) {
        auto rounded_down = date::floor<std::chrono::seconds>(paran.paran_end->as_sys_time());
        auto zoned = date::make_zoned(time_zone, rounded_down);
        fmt::format_to(buf, "{}", date::format(paran_end_format, zoned));
    } else {
        fmt::format_to(buf, "...");
    }
    if (paran_type_separator != nullptr) {
        fmt::format_to(buf, "{}{}", paran_type_separator, paran.type);
    }
    return fmt::to_string(buf);
}

// we round to minutes unless interval is less than 48 minutes long.
bool Paran::is_rounded_to_minutes() const
{
    return paran_start && paran_end && (*paran_end - *paran_start >= std::chrono::minutes{48});
}

std::string Paran::paran_start_str() const
{
    if (!paran_start) return "…";
    auto local = paran_start->as_zoned_time(time_zone).get_local_time();
    if (is_rounded_to_minutes()) {
        return date::format("%H:%M", date::ceil<std::chrono::minutes>(local));
    } else {
        return date::format("%H:%M:%S", date::ceil<std::chrono::seconds>(local));
    }
}

std::string Paran::paran_end_str() const
{
    if (!paran_end) return "…";
    auto local = paran_end->as_zoned_time(time_zone).get_local_time();
    if (is_rounded_to_minutes()) {
        return date::format("%H:%M", date::floor<std::chrono::minutes>(local));
    } else {
        return date::format("%H:%M:%S", date::floor<std::chrono::seconds>(local));
    }
}

} // namespace vp

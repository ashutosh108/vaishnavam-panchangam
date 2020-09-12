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

} // namespace vp

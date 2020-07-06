#include "paran.h"

namespace vp {

std::ostream &Paran::operator<<(std::ostream &o) const
{
    o << type;
    o << "{";
    if (paran_start.has_value()) {
        o << *paran_start;
    }
    o << "..";
    if (paran_end.has_value()) {
        o << *paran_end;
    }
    o << "}";
    return o;
}

std::ostream &operator<<(std::ostream &o, const Paran &p)
{
    return p.operator<<(o);
}

std::ostream &operator<<(std::ostream &s, const Paran::Type &t)
{
    switch (t) {
    case Paran::Type::Standard:
        s << "Standard pāraṇam: from sunrise until 1/5 of day-time"; break;
    case Paran::Type::Until_Dvadashi_End:
        s << "Pāraṇam before the end of Dvādaśī"; break;
    case Paran::Type::From_Quarter_Dvadashi:
        s << "Pāraṇam after the first ¼ of Dvādaśī"; break;
    case Paran::Type::Puccha_Dvadashi:
        s << "Pāraṇam within Puccha-Dvādaśī"; break;
    }
    return s;
}

std::string ParanFormatter::format(const Paran &paran,
                                   const char * timezone_name,
                                   const char * paran_start_format,
                                   const char * separator,
                                   const char * paran_end_format,
                                   const char * paran_type_separator
                                   )
{
    auto timezone = date::locate_zone(timezone_name);
    fmt::memory_buffer buf;
    if (paran.paran_start.has_value()) {
        auto rounded_up = date::ceil<std::chrono::seconds>(paran.paran_start->as_sys_time());
        auto zoned = date::make_zoned(timezone, rounded_up);
        fmt::format_to(buf, "{}", date::format(paran_start_format, zoned));
    } else {
        fmt::format_to(buf, "...");
    }
    fmt::format_to(buf, "{}", separator);
    if (paran.paran_end.has_value()) {
        auto rounded_down = date::floor<std::chrono::seconds>(paran.paran_end->as_sys_time());
        auto zoned = date::make_zoned(timezone, rounded_down);
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

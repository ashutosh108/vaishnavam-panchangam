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
        s << "Pāraṇam before end of dvādaśī"; break;
    case Paran::Type::From_Quarter_Dvadashi:
        s << "Pāraṇam after first ¼ of dvādaśī"; break;
    case Paran::Type::Puccha_Dvadashi:
        s << "Pāraṇam within puccha-dvādaśī"; break;
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
    std::stringstream paran_s;
    if (paran.paran_start.has_value()) {
        auto rounded_up = date::ceil<std::chrono::seconds>(paran.paran_start->as_sys_time());
        auto zoned = date::make_zoned(timezone, rounded_up);
        paran_s << date::format(paran_start_format, zoned);
    } else {
        paran_s << "...";
    }
    paran_s << separator;
    if (paran.paran_end.has_value()) {
        auto rounded_down = date::floor<std::chrono::seconds>(paran.paran_end->as_sys_time());
        auto zoned = date::make_zoned(timezone, rounded_down);
        paran_s << date::format(paran_end_format, zoned);
    } else {
        paran_s << "...";
    }
    if (paran_type_separator != nullptr) {
        paran_s << paran_type_separator << paran.type;
    }
    std::string paran_string = paran_s.str();
    return paran_string;
}

} // namespace vp

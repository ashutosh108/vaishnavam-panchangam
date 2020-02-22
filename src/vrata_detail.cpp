#include "vrata_detail.h"

#include "calc.h"

namespace vp {

Vrata_Detail::Vrata_Detail(Vrata _vrata, Location _location):vrata(_vrata), location(_location), calc(_location) {
    events.push_back({"pAraNam start", vrata.paran.paran_start});
    events.push_back({"pAraNam end", vrata.paran.paran_end});

    JulDays_UT local_midnight = calc.calc_astronomical_midnight(vrata.date);
    auto sunrise = calc.swe.get_sunrise(local_midnight);
    events.push_back({"sunrise1", sunrise});
    if (sunrise) {
        auto arunodaya_pair = calc.get_arunodaya(*sunrise);
        if (arunodaya_pair) {
            events.push_back({"aruNodaya1", arunodaya_pair->first});
            events.push_back({"arddha-ghaTika before aruNodaya1", arunodaya_pair->second});
        }

        auto sunset = calc.swe.get_sunset(*sunrise);
        events.push_back({"sunset1", sunset});
        if (sunset) {
            auto sunrise2 = calc.swe.get_sunrise(*sunset);
            events.push_back({"sunrise2", sunrise2});
            if (sunrise2) {
                auto sunset2 = calc.swe.get_sunset(*sunrise2);
                events.push_back({"sunset2", sunset2});
                if (sunset2) {
                    auto fifth_of_day2 = calc.proportional_time(*sunrise2, *sunset2, 0.2);
                    events.push_back({"1/5 of day2", fifth_of_day2});
                    if (vrata.type == Vrata_Type::With_Atirikta_Dvadashi || vrata.type == Vrata_Type::Sandigdha_With_Atirikta_Dvadashi
                            || vrata.type == Vrata_Type::Atirikta_Ekadashi || vrata.type == Vrata_Type::Sandigdha_Atirikta_Ekadashi) {
                        auto sunrise3 = calc.swe.get_sunrise(*sunset2);
                        events.push_back({"sunrise3", sunrise3});
                        if (sunrise3) {
                            auto sunset3 = calc.swe.get_sunset(*sunrise3);
                            events.push_back({"sunset3", sunset3});
                            if (sunset3) {
                                auto fifth_of_day3 = calc.proportional_time(*sunrise3, *sunset3, 0.2);
                                events.push_back({"1/5 of day3", fifth_of_day3});
                            }
                        }
                    }
                }
            }
        }
    }

    // -1.0 to ensure we actually select start time before Ekadashi.
    // Not 100% sure it's enough, but it's working for all test cases so far.
    ekadashi_start = calc.get_next_tithi_start(local_midnight-double_days{1.0}, Tithi{Tithi::Ekadashi});
    events.push_back({"ekAdashI start", ekadashi_start});
    if (ekadashi_start) {
        auto dvadashi_start = calc.get_next_tithi_start(*ekadashi_start, Tithi{Tithi::Dvadashi});
        events.push_back({"dvAdashI start", dvadashi_start});
        if (dvadashi_start) {
            auto dvadashi_end = calc.get_next_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
            events.push_back({"dvAdashI end", dvadashi_end});
            if (dvadashi_end) {
                events.push_back({"dvAdashI quarter", Calc::proportional_time(*dvadashi_start, *dvadashi_end, 0.25)});
            }
        }
        if (sunrise) {
            auto sunrise0 = calc.swe.get_sunrise(*sunrise - double_days{1.5});
            if (sunrise0 && *sunrise0 >= *ekadashi_start) {
                events.push_back({"sunrise0", sunrise0});
                auto arunodaya_pair = calc.get_arunodaya(*sunrise0);
                if (arunodaya_pair) {
                    events.push_back({"aruNodaya0", arunodaya_pair->first});
                    events.push_back({"arddha-ghaTika before aruNodaya0", arunodaya_pair->second});
                }
            }
        }
    }
}

std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    s << "# " << vd.location.name << "\n";
    s << vd.vrata << ":\n";
    s << "Paran type: " << vd.vrata.paran.type << '\n';
    auto events = vd.events;
    std::sort(events.begin(), events.end(), [](const Vrata_Detail::NamedTimePoint & left, const Vrata_Detail::NamedTimePoint & right) {
        return left.time_point < right.time_point;
    });
    for (const auto & e : events) {
        if (e.time_point.has_value()) {
            s << JulDays_Zoned{vd.location.timezone_name, *e.time_point} << ' ' << e.name << '\n';
        } else {
            s << "(unknown time)" << ' ' << e.name << '\n';
        }
    }
    return s;
}

} // namespace vp

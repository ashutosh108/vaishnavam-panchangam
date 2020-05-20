#include "vrata_detail.h"

#include "calc.h"

#include <iomanip>

namespace vp {

Vrata_Detail::Vrata_Detail(Vrata _vrata, Swe swe):vrata(_vrata), location(swe.coord), calc(std::move(swe)) {
    if (vrata.paran.paran_start) {
        events.push_back({"pAraNam start", vrata.paran.paran_start});
    }
    if (vrata.paran.paran_end) {
        events.push_back({"pAraNam end", vrata.paran.paran_end});
    }

    JulDays_UT local_midnight = calc.calc_astronomical_midnight(vrata.date);
    auto sunrise = calc.swe.find_sunrise(local_midnight);
    events.push_back({"sunrise1", sunrise});
    if (sunrise) {
        auto arunodaya_pair = calc.arunodaya_for_sunrise(*sunrise);
        if (arunodaya_pair) {
            events.push_back({"aruNodaya1", arunodaya_pair->first});
            events.push_back({"arddha-ghaTika before aruNodaya1", arunodaya_pair->second});
        }

        auto sunset = calc.swe.find_sunset(*sunrise);
        events.push_back({"sunset1", sunset});
        if (sunset) {
            auto sunrise2 = calc.swe.find_sunrise(*sunset);
            events.push_back({"sunrise2", sunrise2});
            if (sunrise2) {
                auto sunset2 = calc.swe.find_sunset(*sunrise2);
                events.push_back({"sunset2", sunset2});
                if (sunset2) {
                    auto fifth_of_day2 = calc.proportional_time(*sunrise2, *sunset2, 0.2);
                    events.push_back({"1/5 of day2", fifth_of_day2});
                    if (vrata.type == Vrata_Type::With_Atirikta_Dvadashi || vrata.type == Vrata_Type::Sandigdha_With_Atirikta_Dvadashi
                            || vrata.type == Vrata_Type::Atirikta_Ekadashi || vrata.type == Vrata_Type::Sandigdha_Atirikta_Ekadashi) {
                        auto sunrise3 = calc.swe.find_sunrise(*sunset2);
                        events.push_back({"sunrise3", sunrise3});
                        if (sunrise3) {
                            auto sunset3 = calc.swe.find_sunset(*sunrise3);
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
    ekadashi_start = calc.find_tithi_start(local_midnight-double_days{1.0}, Tithi{Tithi::Ekadashi});
    std::ostringstream ekadashi_descr;
    ekadashi_descr << "ekAdashI start";
    if (ekadashi_start) {
        auto dashami_start = calc.find_tithi_start(*ekadashi_start-double_days{1.5}, Tithi{Tithi::Dashami});
        if (dashami_start) {
            std::ostringstream dashami_descr;
            auto dashami_length = date::round<std::chrono::milliseconds>(*ekadashi_start - *dashami_start);
            double_ghatikas dashami_length_ghatikas = dashami_length;
            dashami_descr << "dashamI start (" << date::format("%Hh %Mm %Ss=", dashami_length)
                          << std::setprecision(3) << std::fixed << dashami_length_ghatikas.count() << "gh long)";
            events.push_back({dashami_descr.str(), dashami_start});
        }

        auto dvadashi_start = calc.find_tithi_start(*ekadashi_start, Tithi{Tithi::Dvadashi});
        std::ostringstream dvadashi_descr;
        dvadashi_descr << "dvAdashI start";
        if (dvadashi_start) {
            auto ekadashi_length = date::round<std::chrono::milliseconds>(*dvadashi_start - *ekadashi_start);
            double_ghatikas ekadashi_length_ghatikas = ekadashi_length;
            ekadashi_descr << " (" << date::format("%Hh %Mm %Ss=", ekadashi_length)
                           << std::setprecision(3) << std::fixed << ekadashi_length_ghatikas.count() << "gh long)";
            auto dvadashi_end = calc.find_tithi_start(*dvadashi_start, Tithi{Tithi::Dvadashi_End});
            events.push_back({"dvAdashI end", dvadashi_end});
            if (dvadashi_end) {
                auto dvadashi_length = date::round<std::chrono::milliseconds>(*dvadashi_end - *dvadashi_start);
                double_ghatikas dvadashi_length_ghatikas = dvadashi_length;
                dvadashi_descr << " (" << date::format("%Hh %Mm %Ss=", dvadashi_length)
                               << std::setprecision(3) << std::fixed << dvadashi_length_ghatikas.count() << "gh long)";
                events.push_back({"dvAdashI's first quarter ends", Calc::proportional_time(*dvadashi_start, *dvadashi_end, 0.25)});
            }
        }
        events.push_back({dvadashi_descr.str(), dvadashi_start});
        if (sunrise) {
            auto sunrise0 = calc.swe.find_sunrise(*sunrise - double_days{1.5});
            if (sunrise0 && *sunrise0 >= *ekadashi_start) {
                events.push_back({"sunrise0", sunrise0});
                auto arunodaya_pair = calc.arunodaya_for_sunrise(*sunrise0);
                if (arunodaya_pair) {
                    events.push_back({"aruNodaya0", arunodaya_pair->first});
                    events.push_back({"arddha-ghaTika before aruNodaya0", arunodaya_pair->second});
                }
            }
        }
    }
    events.push_back({ekadashi_descr.str(), ekadashi_start});

    if (vrata.ativrddhatvam.has_value()) {
        events.push_back({"sunset0", vrata.ativrddhatvam->prev_sunset});
        events.push_back({"54gh_40vigh (ativRddha)", vrata.ativrddhatvam->time_point_ativrddha_54gh_40vigh});
        events.push_back({"55gh (vRddha)", vrata.ativrddhatvam->time_point_vrddha_55gh});
        events.push_back({"55gh_50vigh (samyam)", vrata.ativrddhatvam->time_point_samyam_55gh_50vigh});
        events.push_back({"55gh_55vigh (hrasva)", vrata.ativrddhatvam->time_point_hrasva_55gh_55vigh});
    }
}

std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    s << "# " << vd.location.name << "\n";
    s << vd.vrata;
    if (vd.vrata.ativrddhatvam.has_value()) {
        s << "(" << vd.vrata.ativrddhatvam->ativrddhaadi() << ")";
    }
    s << ":\n";
    s << vd.vrata.paran.type << '\n';
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

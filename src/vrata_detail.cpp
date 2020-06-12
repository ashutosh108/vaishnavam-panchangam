#include "vrata_detail.h"

#include "calc.h"

#include <iomanip>

namespace vp {

Vrata_Detail::Vrata_Detail(Vrata _vrata, Swe swe):vrata(_vrata), location(swe.location), calc(std::move(swe)),
      ekadashi_start(calc.find_tithi_start(calc.calc_astronomical_midnight(vrata.date)-double_days{1.0}, Tithi{Tithi::Ekadashi})) {
    JulDays_UT local_midnight = calc.calc_astronomical_midnight(vrata.date);

    auto sunrise = calc.swe.find_sunrise(local_midnight);
    if (sunrise) {
        auto sunrise0 = calc.swe.find_sunrise(*sunrise - double_days{1.5});
        if (sunrise0 && *sunrise0 >= ekadashi_start) {
            events.push_back({"sunrise0", *sunrise0});
            auto arunodaya = calc.arunodaya_for_sunrise(*sunrise0);
            if (arunodaya) {
                events.push_back({"aruṇodaya0", *arunodaya});
            }
        }

        events.push_back({"**sunrise1**", *sunrise});
        auto arunodaya = calc.arunodaya_for_sunrise(*sunrise);
        if (arunodaya) {
            events.push_back({"aruṇodaya1", *arunodaya});
        }

        auto sunset = calc.swe.find_sunset(*sunrise);
        if (sunset) {
            events.push_back({"sunset1", *sunset});
            auto sunrise2 = calc.swe.find_sunrise(*sunset);
            if (sunrise2) {
                events.push_back({"sunrise2", *sunrise2});
                auto sunset2 = calc.swe.find_sunset(*sunrise2);
                if (sunset2) {
                    events.push_back({"sunset2", *sunset2});
                    auto fifth_of_day2 = calc.proportional_time(*sunrise2, *sunset2, 0.2);
                    events.push_back({"1/5 of day2", fifth_of_day2});
                    if (vrata.type == Vrata_Type::With_Atirikta_Dvadashi
                        || vrata.type == Vrata_Type::Atirikta_Ekadashi) {
                        auto sunrise3 = calc.swe.find_sunrise(*sunset2);
                        if (sunrise3) {
                            events.push_back({"sunrise3", *sunrise3});
                            auto sunset3 = calc.swe.find_sunset(*sunrise3);
                            if (sunset3) {
                                events.push_back({"sunset3", *sunset3});
                                auto fifth_of_day3 = calc.proportional_time(*sunrise3, *sunset3, 0.2);
                                events.push_back({"1/5 of day3", fifth_of_day3});
                            }
                        }
                    }
                }
            }
        }
    }

    const double_ghatikas dashami_length_ghatikas = vrata.ativrddhatvam.ekadashi_start - vrata.ativrddhatvam.dashami_start;
    std::ostringstream dashami_descr;
    dashami_descr << "daśamī start ("
                  << std::setprecision(3) << std::fixed << dashami_length_ghatikas.count() << "gh long)";
    events.push_back({dashami_descr.str(), vrata.ativrddhatvam.dashami_start});

    const double_ghatikas ekadashi_length_ghatikas = vrata.ativrddhatvam.dvadashi_start - vrata.ativrddhatvam.ekadashi_start;
    const double_ghatikas ekadashi_delta = ekadashi_length_ghatikas - dashami_length_ghatikas;
    std::ostringstream ekadashi_descr;
    ekadashi_descr << "**ekādaśī start** ("
                   << std::setprecision(3) << std::fixed << ekadashi_length_ghatikas.count() << "gh long; "
                   << std::showpos << "**" << ekadashi_delta.count() << "gh**)";
    events.push_back({ekadashi_descr.str(), ekadashi_start});

    const double_ghatikas dvadashi_length_ghatikas = vrata.ativrddhatvam.trayodashi_start - vrata.ativrddhatvam.dvadashi_start;
    const double_ghatikas dvadashi_delta = dvadashi_length_ghatikas - ekadashi_length_ghatikas;
    std::ostringstream dvadashi_descr;
    dvadashi_descr << "dvādaśī start ("
                   << std::setprecision(3) << std::fixed << dvadashi_length_ghatikas.count() << "gh long; "
                   << std::showpos << "**" << dvadashi_delta.count() << "gh**)";
    events.push_back({dvadashi_descr.str(), vrata.ativrddhatvam.dvadashi_start});

    events.push_back({"dvādaśī's first quarter ends", Calc::proportional_time(vrata.ativrddhatvam.dvadashi_start, vrata.ativrddhatvam.trayodashi_start, 0.25)});

    events.push_back({"dvādaśī end", vrata.ativrddhatvam.trayodashi_start});

    events.push_back({"sunset0", vrata.ativrddhatvam.prev_sunset});
    std::string star;
    auto status = vrata.ativrddhatvam.ativrddhaadi();
    star = (status == Ativrddhatvam::Ativrddhaadi::ativrddha) ? "**" : "";
    events.push_back({"54gh_40vigh (" + star + "ativṛddhiḥ" + star + ")", vrata.ativrddhatvam.time_point_ativrddha_54gh_40vigh});
    star = status == Ativrddhatvam::Ativrddhaadi::vrddha ? "**" : "";
    events.push_back({"55gh (" + star + "vṛddhiḥ" + star + ")", vrata.ativrddhatvam.time_point_vrddha_55gh});
    star = status == Ativrddhatvam::Ativrddhaadi::samyam ? "**" : "";
    events.push_back({"55gh_50vigh (" + star + "sāmyam" + star + ")", vrata.ativrddhatvam.time_point_samyam_55gh_50vigh});
    star = status == Ativrddhatvam::Ativrddhaadi::hrasva ? "**" : "";
    events.push_back({"55gh_55vigh (" + star + "hrāsaḥ" + star + ")", vrata.ativrddhatvam.time_point_hrasva_55gh_55vigh});

    // push them at the end so after joining duplicate events the "paranam" text would come after "sunrise" etc.
    // e.g. we want to have: yyyy-mm-dd hh:mm:ss.ssss sunrise1, pAraNam start
    // and we don't want to have: yyyy-mm-dd hh:mm:ss.ssss pAraNam start, sunrise1
    if (vrata.paran.paran_start) {
        events.push_back({"**pāraṇam start**", *vrata.paran.paran_start});
    }
    if (vrata.paran.paran_end) {
        events.push_back({"**pāraṇam end**", *vrata.paran.paran_end});
    }
}

static void merge_consequent_events_with_same_time(std::vector<Vrata_Detail::NamedTimePoint> & events) {
    for (std::size_t i = 1; i < events.size(); ++i) {
        auto time1 = events[i-1].time_point;
        auto time2 = events[i].time_point;
        // same time for both events. Merge them by appending descriptions via comma.
        if (time1 == time2) {
            events[i-1].name += ", " + events[i].name;
            events.erase(events.begin() + i);
        }
    }
}

std::ostream &operator<<(std::ostream &s, const Vrata_Detail &vd)
{
    s << "# " << vd.vrata.location_name() << "\n";
    s << vd.vrata << ":\n";
    s << vd.vrata.paran.type << '\n';
    auto events = vd.events;
    // stable sort to keep "pAraNam start/end" after corresponding sunrise+ events.
    std::stable_sort(events.begin(), events.end(), [](const Vrata_Detail::NamedTimePoint & left, const Vrata_Detail::NamedTimePoint & right) {
        return left.time_point < right.time_point;
    });
    merge_consequent_events_with_same_time(events);
    for (const auto & e : events) {
        s << JulDays_Zoned{vd.location.timezone_name, e.time_point} << ' ' << e.name << '\n';
    }
    return s;
}

} // namespace vp

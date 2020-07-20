#include "vrata_detail_printer.h"

#include "calc.h"

#include <iomanip>

namespace vp {

Vrata_Detail_Printer::Vrata_Detail_Printer(Vrata _vrata):vrata(_vrata) {
    events.push_back({"**sunrise1**", vrata.sunrise1});
    events.push_back({"aruṇodaya1", vrata.times.arunodaya});

    events.push_back({"sunrise2", vrata.sunrise2});
    events.push_back({"sunset2", vrata.sunset2});
    auto fifth_of_day2 = Calc::proportional_time(vrata.sunrise2, vrata.sunset2, 0.2);
    events.push_back({"1/5 of day2", fifth_of_day2});
    if (is_atirikta(vrata.type)) {
        events.push_back({"sunrise3", vrata.sunrise3});
        events.push_back({"sunset3", vrata.sunset3});
        auto fifth_of_day3 = Calc::proportional_time(vrata.sunrise3, vrata.sunset3, 0.2);
        events.push_back({"1/5 of day3", fifth_of_day3});
    }

    const double_ghatikas dashami_length_ghatikas = vrata.times.ekadashi_start - vrata.times.dashami_start;
    std::ostringstream dashami_descr;
    dashami_descr << "daśamī start ("
                  << std::setprecision(3) << std::fixed << dashami_length_ghatikas.count() << "gh long)";
    events.push_back({dashami_descr.str(), vrata.times.dashami_start});

    const double_ghatikas ekadashi_length_ghatikas = vrata.times.dvadashi_start - vrata.times.ekadashi_start;
    const double_ghatikas ekadashi_delta = ekadashi_length_ghatikas - dashami_length_ghatikas;
    std::ostringstream ekadashi_descr;
    ekadashi_descr << "**Ekādaśī start** ("
                   << std::setprecision(3) << std::fixed << ekadashi_length_ghatikas.count() << "gh long; "
                   << std::showpos << "**" << ekadashi_delta.count() << "gh**)";
    events.push_back({ekadashi_descr.str(), vrata.times.ekadashi_start});

    const double_ghatikas dvadashi_length_ghatikas = vrata.times.trayodashi_start - vrata.times.dvadashi_start;
    const double_ghatikas dvadashi_delta = dvadashi_length_ghatikas - ekadashi_length_ghatikas;
    std::ostringstream dvadashi_descr;
    dvadashi_descr << "Dvādaśī start ("
                   << std::setprecision(3) << std::fixed << dvadashi_length_ghatikas.count() << "gh long; "
                   << std::showpos << "**" << dvadashi_delta.count() << "gh**)";
    events.push_back({dvadashi_descr.str(), vrata.times.dvadashi_start});

    events.push_back({"Dvādaśī's first quarter ends", Calc::proportional_time(vrata.times.dvadashi_start, vrata.times.trayodashi_start, 0.25)});

    events.push_back({"Dvādaśī end", vrata.times.trayodashi_start});

    events.push_back({"sunset0", vrata.sunset0});
    std::string star;
    const auto status = vrata.times.ativrddhaadi();
    star = (status == Vrata_Time_Points::Ativrddhaadi::ativrddha) ? "**" : "";
    events.push_back({"54gh_40vigh (" + star + "ativṛddhiḥ" + star + ")", vrata.times.ativrddha_54gh_40vigh});
    star = status == Vrata_Time_Points::Ativrddhaadi::vrddha ? "**" : "";
    events.push_back({"55gh (" + star + "vṛddhiḥ" + star + ")", vrata.times.vrddha_55gh});
    star = status == Vrata_Time_Points::Ativrddhaadi::samyam ? "**" : "";
    events.push_back({"55gh_50vigh (" + star + "sāmyam" + star + ")", vrata.times.samyam_55gh_50vigh});
    star = status == Vrata_Time_Points::Ativrddhaadi::hrasva ? "**" : "";
    events.push_back({"55gh_55vigh (" + star + "hrāsaḥ" + star + ")", vrata.times.hrasva_55gh_55vigh});

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

} // namespace vp

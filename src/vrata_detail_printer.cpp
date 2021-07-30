#include "vrata_detail_printer.h"

#include "calc.h"

namespace vp {

Vrata_Detail_Printer::Vrata_Detail_Printer(const Vrata & _vrata):vrata(_vrata) {
    if (vrata.sunrise0) {
        events.push_back({"sunrise0", *vrata.sunrise0});
    }
    events.push_back({"**sunrise1**", vrata.sunrise1});

    // sunrise0 being set means we have moved vrata one day ahead because of
    // dAshamI viddha, so aruṇodaya belongs to day0 (before vrata), not to day1 as usual.
    events.push_back({vrata.sunrise0 ? "aruṇodaya0" : "aruṇodaya1", vrata.times.arunodaya});

    events.push_back({"sunrise2", vrata.sunrise2});
    events.push_back({"sunset2", vrata.sunset2});
    auto fifth_of_day2 = proportional_time(vrata.sunrise2, vrata.sunset2, 0.2);
    events.push_back({"1/5 of day2", fifth_of_day2});
    if (is_atirikta(vrata.type)) {
        events.push_back({"sunrise3", vrata.sunrise3});
        events.push_back({"sunset3", vrata.sunset3});
        auto fifth_of_day3 = proportional_time(vrata.sunrise3, vrata.sunset3, 0.2);
        events.push_back({"1/5 of day3", fifth_of_day3});
    }

    const double_ghatikas dashami_length_ghatikas = vrata.times.ekadashi_start - vrata.times.dashami_start;
    events.push_back({fmt::format("daśamī start ({:.3f}gh long)", dashami_length_ghatikas.count()), vrata.times.dashami_start});

    const double_ghatikas ekadashi_length_ghatikas = vrata.times.dvadashi_start - vrata.times.ekadashi_start;
    const double_ghatikas ekadashi_delta = ekadashi_length_ghatikas - dashami_length_ghatikas;
    events.push_back({fmt::format(
                          "**Ekādaśī start** ({:.3f}gh long; **{:+.3f}gh**)",
                          ekadashi_length_ghatikas.count(),
                          ekadashi_delta.count()),
                      vrata.times.ekadashi_start});

    const double_ghatikas dvadashi_length_ghatikas = vrata.times.trayodashi_start - vrata.times.dvadashi_start;
    const double_ghatikas dvadashi_delta = dvadashi_length_ghatikas - ekadashi_length_ghatikas;
    events.push_back({fmt::format(
                          "Dvādaśī start ({:.3f}gh long; **{:+.3f}gh**)",
                          dvadashi_length_ghatikas.count(),
                          dvadashi_delta.count()),
                      vrata.times.dvadashi_start});

    const auto harivasara = proportional_time(vrata.times.ekadashi_start, vrata.times.dvadashi_start, 0.75);
    if (harivasara < vrata.sunrise1) {
        events.push_back({"Harivāsara starts (Ekādaśī's last quarter)", harivasara});
    } else {
        events.push_back({"Ekādaśī's last quarter starts", harivasara});
    }
    events.push_back({"Dvādaśī's first quarter ends", proportional_time(vrata.times.dvadashi_start, vrata.times.trayodashi_start, 0.25)});

    events.push_back({"Dvādaśī end", vrata.times.trayodashi_start});

    events.push_back({"sunset0", vrata.sunset0});
    const char * star{};
    const auto status = vrata.times.ativrddhaadi();
    star = status == Vrata_Time_Points::Ativrddhaadi::ativrddha ? "**" : "";
    events.push_back({fmt::format("54gh_40vigh ({0}ativṛddhiḥ{0})", star), vrata.times.ativrddha_54gh_40vigh});
    star = status == Vrata_Time_Points::Ativrddhaadi::vrddha ? "**" : "";
    events.push_back({fmt::format("55gh ({0}vṛddhiḥ{0})", star), vrata.times.vrddha_55gh});
    star = status == Vrata_Time_Points::Ativrddhaadi::samyam ? "**" : "";
    events.push_back({fmt::format("55gh_50vigh ({0}sāmyam{0})", star), vrata.times.samyam_55gh_50vigh});
    star = status == Vrata_Time_Points::Ativrddhaadi::hrasva ? "**" : "";
    events.push_back({fmt::format("55gh_55vigh ({0}hrāsaḥ{0})", star), vrata.times.hrasva_55gh_55vigh});

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

#include "table-calendar-generator.h"

#include <set>
#include <utility>

static std::set<date::year_month_day> get_vrata_dates(const vp::VratasForDate & vratas) {
    std::set<date::year_month_day> dates;
    for (const auto & vrata : vratas) {
        if (vrata) {
            dates.insert(vrata->date);
            dates.insert(vrata->local_paran_date());
            // insert "date + 1 day" in atirikta case, since paran is "date + 2 days"
            if (vp::is_atirikta(vrata->type)) {
                dates.insert(date::year_month_day{date::sys_days{vrata->date} + date::days{1}});
            }
        }
    }
    return dates;
}

static void add_header(vp::Table & table, const std::set<date::year_month_day> & vrata_dates) {
    table.add_header_cell("॥ श्रीः ॥");
    table.add_header_cell("॥ श्रीः ॥");
    table.add_header_cell("॥ श्रीः ॥");

    for (auto date : vrata_dates) {
        table.add_header_cell(fmt::format("{:l}", date));
    }
}

static std::string get_timezone_text(const vp::MaybeVrata & vrata) {
    if (!vrata) return "-";
    if (!vrata->paran.paran_start) return "-";
    auto paran_start_sys = vrata->paran.paran_start->as_sys_time();
    auto timezone = vrata->location.time_zone();
    auto info = timezone->get_info(paran_start_sys);

    long long seconds = info.offset.count();
    char sign = seconds < 0 ? '-' : '+';
    seconds = std::abs(seconds);
    auto hours = seconds / 3600;
    seconds -= hours * 3600;
    auto minutes = seconds / 60;
    seconds -= minutes * 60;
    std::string dst = info.save != std::chrono::seconds{0} ? " (DST)" : "";

    if (seconds != 0) {
        return fmt::format("{}{}:{:02}:{:02}{}", sign, hours, minutes, seconds, dst);
    }
    return fmt::format("{}{}:{:02}{}", sign, hours, minutes, dst);
}

static void add_vrata(vp::Table & table, const vp::MaybeVrata & vrata, const std::set<date::year_month_day> & vrata_dates, std::string tr_classes) {
    table.start_new_row(std::move(tr_classes));
    table.add_cell(get_timezone_text(vrata));
    table.add_cell(vrata->location.country);
    table.add_cell(vrata->location_name());
    if (!vrata) {
        for (std::size_t i=0; i < vrata_dates.size(); ++i) {
            table.add_cell("calculation error");
        }
        return;
    }
    for (auto date : vrata_dates) {
        std::string classes = "mainpart";
        if (date >= vrata->date && date < vrata->local_paran_date()) {
            table.add_cell(fmt::to_string(vrata->type), classes + " vrata");
        } else if (date == vrata->local_paran_date()) {
            // 'c' means compact formatting
            table.add_unmergeable_cell(fmt::format("{:c}", vrata->paran), classes);
        } else {
            table.add_unmergeable_cell("", classes);
        }
    }
}

vp::Table vp::Table_Calendar_Generator::generate(const vp::VratasForDate & vratas)
{
    vp::Table table;
    auto vrata_dates = get_vrata_dates(vratas);
    add_header(table, vrata_dates);
    int row = 0;
    for (const auto & vrata : vratas) {
        add_vrata(table, vrata, vrata_dates, ++row % 2 ? "odd" : "even");
    }
    table.merge_cells_into_rowspans();
    table.merge_cells_into_colspans();
    return table;
}

#include "table-calendar-generator.h"

#include <set>

std::set<date::year_month_day> get_vrata_dates(const vp::MaybeVratas & vratas) {
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

vp::Table vp::Table_Calendar_Generator::generate(const vp::MaybeVratas & vratas)
{
    vp::Table table;
    table.add_header_cell("॥ श्रीः ॥");
    table.add_header_cell("॥ श्रीः ॥");
    table.add_header_cell("॥ श्रीः ॥");

    auto vrata_dates = get_vrata_dates(vratas);
    for (auto date : vrata_dates) {
        table.add_header_cell(fmt::format("{:l}", date));
    }
    return table;
}

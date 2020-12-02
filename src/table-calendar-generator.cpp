#include "table-calendar-generator.h"

#include "html-util.h"

#include <set>
#include <utility>

namespace {

std::set<date::year_month_day> get_vrata_dates(const vp::VratasForDate & vratas, const vp::Custom_Dates & custom_dates) {
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
    for (const auto & date_desc_pair : custom_dates) {
        dates.insert(date_desc_pair.first);
    }
    return dates;
}

void add_header(vp::Table & table, const std::set<date::year_month_day> & vrata_dates, date::year default_year, std::string text) {
    table.start_new_row();
    table.add_header_cell(text);
    table.add_header_cell(text);
    table.add_header_cell(text);

    for (auto date : vrata_dates) {
        if (default_year != date::year::min() && date.year() != default_year) {
            table.add_header_cell(fmt::format("{:lhy}", date), "mainpart");
        } else {
            table.add_header_cell(fmt::format("{:lh}", date), "mainpart");
        }
    }
}

std::string get_timezone_text(const vp::MaybeVrata & vrata) {
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

namespace {
std::string paran_title(const vp::Paran & paran) {
    std::string title;

    if (paran.paran_start) {
        title += fmt::format("{} ({})", paran.start_str_seconds(), paran.start_type());
    }
    title += "…";
    if (paran.paran_end) {
        title += fmt::format("{} ({})", paran.end_str_seconds(), paran.end_type());
    }
    return title;
}
}

void add_vrata(vp::Table & table, const vp::MaybeVrata & vrata, const std::set<date::year_month_day> & vrata_dates, std::string tr_classes, const vp::Custom_Dates & custom_dates) {
    table.start_new_row(std::move(tr_classes));
    table.add_cell(get_timezone_text(vrata));
    table.add_cell(vrata->location.country);
    {
        auto location_with_href = fmt::format(R"(<a href="#{}">{}</a>)", html::escape_attribute(vrata->location_name()), vrata->location_name());
        table.add_cell(location_with_href).set_title(fmt::format("Timezone: {}", vrata->location.time_zone_name));
    }
    if (!vrata) {
        for (std::size_t i=0; i < vrata_dates.size(); ++i) {
            table.add_cell("calculation error");
        }
        return;
    }
    for (auto date : vrata_dates) {
        std::string classes = "mainpart";
        if (date >= vrata->date && date < vrata->local_paran_date()) {
            table.add_cell(fmt::format(FMT_STRING("{} {}"), vrata->ekadashi_name(), vrata->type), classes + " vrata");
        } else if (date == vrata->local_paran_date()) {
            // 'c' means compact formatting ("*" for standard pAraNam, otherwise something like ">06:45", "<07:45" or "06:45-07.45")
            const auto paran_with_href = fmt::format(R"(<a href="#{}">{:c}</a>)", html::escape_attribute(vrata->location_name()), vrata->paran);
            table.add_unmergeable_cell(paran_with_href, classes).set_title(paran_title(vrata->paran));
        } else if (auto found_it = custom_dates.find(date); found_it != custom_dates.end()) {
            table.add_cell(found_it->second, "custom");
        } else {
            table.add_unmergeable_cell("", classes);
        }
    }
}

std::vector<double> calc_column_widths(const vp::Table & table) {
    size_t width = table.width();

    constexpr double timezone_col_width = 8.0;
    constexpr double country_col_width = 16.0;
    constexpr double city_col_width = 16.0;
    std::vector<double> col_widths{timezone_col_width, country_col_width, city_col_width};
    col_widths.resize(width);
    const size_t mainpart_col_count = width - 3;
    const double mainpart_col_width = (100.0 - timezone_col_width - country_col_width - city_col_width) / mainpart_col_count;
    for (size_t col=3; col < width; ++col) {
        col_widths[col] = mainpart_col_width;
    }
    return col_widths;
}

std::chrono::seconds utc_offset_for_vrata(const vp::Vrata & vrata) {
    if (!vrata.paran.paran_start) return {};
    auto paran_start_sys = vrata.paran.paran_start->as_sys_time();
    auto timezone = vrata.location.time_zone();
    auto info = timezone->get_info(paran_start_sys);

    return info.offset;
}

} // anonymous namespace

vp::Table vp::Table_Calendar_Generator::generate(const vp::VratasForDate & vratas, date::year default_year, const Custom_Dates & custom_dates)
{
    vp::Table table;
    auto vrata_dates = get_vrata_dates(vratas, custom_dates);
    add_header(table, vrata_dates, default_year, "॥ श्रीः ॥");
    int row = 1;
    const vp::MaybeVrata * prev_vrata{};
    std::chrono::seconds prev_vrata_utc_offset;
    using namespace std::chrono_literals;
    constexpr std::chrono::seconds min_utc_offset_for_separator = 7h;
    for (const auto & vrata : vratas) {
        if (vrata) {
            auto vrata_utc_offset = utc_offset_for_vrata(*vrata);
            if (prev_vrata) {
                if (abs(vrata_utc_offset - prev_vrata_utc_offset) >= min_utc_offset_for_separator) {
                    table.start_new_row("separator");
                }
            }
            prev_vrata = &vrata;
            prev_vrata_utc_offset = vrata_utc_offset;
        }
        add_vrata(table, vrata, vrata_dates, ++row % 2 ? "odd" : "even", custom_dates);
    }
    add_header(table, vrata_dates, default_year, "॥ ॐ तत्सत् ॥");
    table.merge_cells_into_rowspans();
    table.merge_cells_into_colspans();
    table.set_column_widths(calc_column_widths(table));
    table.add_even_odd_classes_for_col(0);
    table.add_even_odd_classes_for_col(1, Table::StartFrom::Even);
    return table;
}

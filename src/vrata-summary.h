#ifndef VP_VRATA_SUMMARY_H
#define VP_VRATA_SUMMARY_H

#include "vrata.h"

#include "fmt-format-fixed.h"

namespace vp {

class Vrata_Summary
{
public:
    Vrata_Summary(const Vrata * _vrata);
    const Vrata * vrata;
};

} // namespace vp

template<>
struct fmt::formatter<vp::Vrata_Summary> : fmt::formatter<std::string_view> {
    template<typename FormatCtx>
    auto format(const vp::Vrata_Summary & vs, FormatCtx & ctx) {
        if (vs.vrata == nullptr) {
            return fmt::format_to(ctx.out(), FMT_STRING("(null)"));
        }
        fmt::format_to(ctx.out(), FMT_STRING("<h1>{}</h1>\n"), vs.vrata->location_name());
        fmt::format_to(ctx.out(), FMT_STRING("<p>{} māsa (amānta), {} pakṣa</p>\n"), vs.vrata->masa, vs.vrata->paksha);
        const date::year_month_day date{vs.vrata->date}; // year_month_day is better formatted by default than local_days
        if (vp::is_atirikta(vs.vrata->type)) {
            date::year_month_day next_day{vs.vrata->date + date::days{1}};
            fmt::format_to(ctx.out(), FMT_STRING("<p>{} {} on <span class=\"date-range\"><span class=\"date\">{}</span> and <span class=\"date\">{}</span></span></big></p>\n"), vs.vrata->ekadashi_name(), vs.vrata->type, date, next_day);
        } else {
            fmt::format_to(ctx.out(), FMT_STRING("<p>{} {} on <span class=\"date paran\">{}</span></p>\n"), vs.vrata->ekadashi_name(), vs.vrata->type, date);
        }
        const auto paran_date = date::year_month_day{vs.vrata->local_paran_date()}; // year_month_day to ensure proper formatting, wihout hours, minutes and seconds
        fmt::format_to(ctx.out(), FMT_STRING(R"(<p class="paran">Pāraṇam: {} <span class="paran-range">{}–{})"), paran_date, vs.vrata->paran.start_str(), vs.vrata->paran.end_str());
        if (vs.vrata->paran.paran_limit) {
            const auto limit_str = date::format("%H:%M", date::floor<std::chrono::minutes>(vs.vrata->paran.paran_limit->as_zoned_time(vs.vrata->paran.time_zone).get_local_time()));
            fmt::format_to(ctx.out(), FMT_STRING(" (&lt;{})"), limit_str);
        }
        fmt::format_to(ctx.out(), FMT_STRING("</span><br>"));
        fmt::format_to(ctx.out(), FMT_STRING(R"(<span class="paran-type">{}</span></p>)"), vs.vrata->paran.type);
        return ctx.out();
    }
};

#endif // VP_VRATA_SUMMARY_H

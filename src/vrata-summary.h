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
            return fmt::format_to(ctx.out(), "(null)");
        }
        fmt::format_to(ctx.out(), "<h1>{}</h1>\n", vs.vrata->location_name());
        if (vp::is_atirikta(vs.vrata->type)) {
            date::year_month_day next_day{date::sys_days{vs.vrata->date} + date::days{1}};
            fmt::format_to(ctx.out(), "<p>{} on <span class=\"date-range\"><span class=\"date\">{}</span> and <span class=\"date\">{}</span></span></big></p>\n", vs.vrata->type, vs.vrata->date, next_day);
        } else {
            fmt::format_to(ctx.out(), "<p>{} on <span class=\"date paran\">{}</span></p>\n", vs.vrata->type, vs.vrata->date);
        }
        fmt::format_to(ctx.out(), R"(<p class="paran">Pāraṇam: {} <span class="paran-range">{}–{}</span><br>)", vs.vrata->local_paran_date(), vs.vrata->paran.start_str(), vs.vrata->paran.end_str());
        fmt::format_to(ctx.out(), R"(<span class="paran-type">{}</span></p>)", vs.vrata->paran.type);
        return ctx.out();
    }
};

#endif // VP_VRATA_SUMMARY_H

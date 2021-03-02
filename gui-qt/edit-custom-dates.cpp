#include "edit-custom-dates.h"

#include <QInputDialog>
#include <QRegularExpression>

namespace vp {

namespace detail {

namespace  {
void parse_and_add_line(const QString & line, vp::Custom_Dates & out) {
    QRegularExpression exp{R"(^(\d+)[-/ .]+(\d+)[-/ .]+(\d+)(?:\s+(.*))?)"};
    if (auto match = exp.match(line); match.hasMatch()) {
        date::local_days date{date::year{match.captured(1).toInt()} / match.captured(2).toInt() / match.captured(3).toInt()};
        out[date] = match.captured(4).toStdString();
    }
}
}
vp::Custom_Dates qstring_to_custom_dates(const QString & str)
{
    auto lines = str.split(QRegularExpression(R"([\r\n]+)"));
    vp::Custom_Dates dates;
    for (const auto & line : lines) {
        parse_and_add_line(line, dates);
    }
    return dates;
}

QString custom_dates_to_qstring(const vp::Custom_Dates & custom_dates) {
    // remember to end the default value with \n to keep the comments separated from the first real data line
    QString str{
        "# format for each line is:\n# YYYY-MM-DD description\n# lines starting from \"#\" are ignored as comments\n\n"
        "# Example (remove # symbols from needed lines):\n#2020-01-01 Extra day\n#2020-01-10 Another extra day\n"};
    for (const auto & date_it : custom_dates) {
        const auto date = date::year_month_day{date_it.first};
        if (date_it.second.empty()) {
            str += QString::fromStdString(fmt::format(FMT_STRING("{}\n"), date));
        } else {
            str += QString::fromStdString(fmt::format(FMT_STRING("{} {}\n"), date, date_it.second));
        }

    }
    return str;
}
}

vp::Custom_Dates edit_custom_dates(const vp::Custom_Dates & custom_dates, QWidget * parent)
{
    const auto old_text = detail::custom_dates_to_qstring(custom_dates);
    bool ok;
    auto text = QInputDialog::getMultiLineText(parent, "Add custom dates", "", old_text, &ok);
    if (!ok) { text = old_text; }
    return detail::qstring_to_custom_dates(text);
}

} // namespace vp

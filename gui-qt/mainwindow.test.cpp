#include "catch-formatters.h"

using Catch::Matchers::Matches;

#include "mainwindow.h"

TEST_CASE("htmlify_line marks 'with Atiriktā Ekādaśī' (resumably in red)") {
    std::string s = htmlify_line("Ekādaśī with Atiriktā Ekādaśī (two days fast) on 2020-06-16 & 2020-06-17:").toStdString();
    REQUIRE_THAT(s, Matches(".*<span[^>]*>with Atiriktā Ekādaśī</span.*"));
}

TEST_CASE("htmlify_line marks 'with Atiriktā Dvādaśī' (presumably in red)") {
    std::string s = htmlify_line("Ekādaśī with Atiriktā Dvādaśī' (two days fast) on 2020-06-16 & 2020-06-17:").toStdString();
    REQUIRE_THAT(s, Matches(".*<span[^>]*>with Atiriktā Dvādaśī</span.*"));
}

TEST_CASE("daybyday header contains weekday") {
    using Catch::Matchers::Contains;
    using namespace date;
    vp::text_ui::DayByDayInfo di;

    di.date = 2023_y/9/21;

    std::string s = html_for_daybyday(di).toStdString();
    REQUIRE_THAT(s, Contains("(Thu/Gu)"));
}

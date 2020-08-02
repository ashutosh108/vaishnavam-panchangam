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

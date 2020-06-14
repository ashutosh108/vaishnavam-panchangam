#include "catch.hpp"

using Catch::Matchers::Matches;

#include "mainwindow.h"

TEST_CASE("htmlify_line marks 'with AtiriktA ekAdashI' (resumably in red)") {
    std::string s = htmlify_line("Ekādaśī with Atiriktā ekādaśī (two days fast) on 2020-06-16 & 2020-06-17:").toStdString();
    REQUIRE_THAT(s, Matches(".*<span[^>]*>with Atiriktā ekādaśī</span.*"));
}

TEST_CASE("htmlify_line marks 'with Atiriktā dvādaśī' (presumably in red)") {
    std::string s = htmlify_line("Ekādaśī with Atiriktā dvādaśī' (two days fast) on 2020-06-16 & 2020-06-17:").toStdString();
    REQUIRE_THAT(s, Matches(".*<span[^>]*>with Atiriktā dvādaśī</span.*"));
}

#include "catch-formatters.h"

#include "html-util.h"

TEST_CASE("html::escape_attribute does proper escaping") {
    REQUIRE(html::escape_attribute(R"(qwe&'"<>)") == R"(qwe&amp;&#039;&quot;&lt;&gt;)");
}

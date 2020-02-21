#include "text-interface.h"

#include <array>
#include "catch.hpp"

TEST_CASE("get_exe_dir() exists and returns something") {
    auto exe_dir = vp::text_ui::detail::determine_exe_dir("imaginary/path/to/exe/file");

    // ensure that path ends with what we expect it to
    auto end_iterator = std::end(exe_dir);
    constexpr std::array<const char*, 4> expected_parts{"imaginary", "path", "to", "exe"};
    for (auto component=expected_parts.rbegin(); component != expected_parts.rend(); ++component) {
        REQUIRE(*component == *--end_iterator);
    }
}

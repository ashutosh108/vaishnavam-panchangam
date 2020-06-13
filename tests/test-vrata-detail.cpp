#include "date-fixed.h"
#include "location.h"
#include "vrata.h"
#include "vrata_detail.h"

#include "catch.hpp"
#include <sstream>


using namespace date;
using namespace vp;

TEST_CASE("Print result is not empty") {
    std::stringstream s;
    Vrata v{2019_y/March/1};
    Location c{50.0_N, 50.0_E};
    Vrata_Detail_Printer vd{v, c};
    s << vd;
    REQUIRE(s.str() != "");
}

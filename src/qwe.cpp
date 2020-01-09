#include "tz-fixed.h"

#include <iostream>

int main() {
    using namespace date;
    using namespace std::chrono;
    set_install("tzdata");
    auto t = make_zoned(current_zone(), system_clock::now());
    std::cout << t << '\n';
}

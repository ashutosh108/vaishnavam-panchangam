#include "date/date.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "date/tz.h"
#pragma GCC diagnostic pop

#include <iostream>

int main() {
	using namespace date;
	using namespace std::chrono;
	set_install("../vendor/tzdata");
	auto t = make_zoned(current_zone(), system_clock::now());
	std::cout << t << '\n';
}

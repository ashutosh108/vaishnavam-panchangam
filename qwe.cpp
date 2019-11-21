#include "date/date.h"
#include "date/tz.h"
#include <iostream>

int main() {
	using namespace date;
	using namespace std::chrono;
	set_install("../vendor/tzdata");
	auto t = make_zoned(current_zone(), system_clock::now());
	std::cout << t << '\n';
}

#include <cmath>
#include <iomanip>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "date/tz.h"
#pragma GCC diagnostic pop

#include "date/date.h"
#include "swe_time.h"
#include <swephexp.h>

Swe_Time::Swe_Time(double jd) : jd_(jd)
{
    int year, month, day;
    swe_revjul(jd_, SE_GREG_CAL, &year, &month, &day, &hours_);
    year_ = date::year{year};
    month_ = date::month{static_cast<unsigned int>(month)};
    day_ = date::day{static_cast<unsigned int>(day)};

}

Swe_Time::Swe_Time(date::year year, date::month month, date::day day, double hours)
    : year_(year), month_(month), day_(day), hours_(hours)
{
    jd_ = swe_julday(static_cast<int>(year),
                     static_cast<int>(static_cast<unsigned>(month)),
                     static_cast<int>(static_cast<unsigned>(day)),
                     hours, SE_GREG_CAL);
}

Swe_Time::Swe_Time(date::year year, date::month month, date::day day, int hours, int minutes, double seconds)
    : Swe_Time(year, month, day, hours+minutes/60.0+seconds/3600.0) {
}

date::year Swe_Time::year() const
{
    return year_;
}

date::month Swe_Time::month() const
{
    return month_;
}

date::day Swe_Time::day() const
{
    return day_;
}

double Swe_Time::hours() const
{
    return hours_;
}

bool Swe_Time::operator==(const Swe_Time &to) const
{
    const double epsilon = 1e-6;
    return year_ == to.year_ &&
            month_ == to.month_ &&
            day_ == to.day_ &&
            std::fabs(hours_ - to.hours_) <= epsilon;
}

date::year_month_day Swe_Time::as_date() const
{
    return date::year_month_day{year_, month_, day_};
}

std::ostream &operator<<(std::ostream &os, Swe_Time const &t) {
    int hours = static_cast<int>(t.hours_);
    double minutes_remain = (t.hours_ - hours) * 60;
    int minutes = static_cast<int>(minutes_remain);
    double seconds = (minutes_remain - minutes) * 60;

    os << date::year_month_day{t.year_, t.month_, t.day_} << ' ';
    os.fill('0');
    os.width(2);
    os << hours << ':';
    os.width(2);
    os << minutes << ':' << std::fixed << std::setprecision(6) << std::setw(9) << seconds << " UTC";
    return os;
}

std::ostream &operator<<(std::ostream &os, Swe_Zoned_Time const &t) {
    using namespace date;
    using namespace std::chrono_literals;
    int hours = static_cast<int>(t.t.hours());
    double minutes_remain = (t.t.hours() - hours) * 60;
    int minutes = static_cast<int>(minutes_remain);
    double seconds = (minutes_remain - minutes) * 60;
    int seconds_int = static_cast<int>(seconds);
    int microseconds = static_cast<int>((seconds-seconds_int)*1'000'000);
    date::local_days l{t.t.as_date()};
    auto utc = make_zoned("UTC",
                          l + std::chrono::hours{hours} + std::chrono::minutes{minutes} +
                          std::chrono::seconds{seconds_int} + std::chrono::microseconds{microseconds});
    auto z = make_zoned(t.timezone_name, utc);
    return os << z;
}

Swe_Time operator +(const Swe_Time &t, double delta)
{
    return Swe_Time{t.as_julian_days() + delta};
}

Swe_Time operator -(const Swe_Time &t, double delta)
{
    return t + (-delta);
}

Swe_Time Swe_Time::operator +=(double delta)
{
    *this = *this+delta;
    return *this;
}

Swe_Time Swe_Time::operator -=(double delta)
{
    *this = *this-delta;
    return *this;
}

bool Swe_Time::operator <(const Swe_Time &other) const
{
    return as_julian_days() < other.as_julian_days();
}

bool Swe_Time::operator >(const Swe_Time &other) const
{
    return as_julian_days() > other.as_julian_days();
}

double operator -(const Swe_Time &t1, const Swe_Time &t2)
{
    return t1.as_julian_days() - t2.as_julian_days();
}

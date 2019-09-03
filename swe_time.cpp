#include <cmath>
#include <iomanip>

#include "swe_time.h"
#include <swephexp.h>

Swe_Time::Swe_Time(double jd) : jd_(jd)
{
    swe_revjul(jd_, SE_GREG_CAL, &year_, &month_, &day_, &hours_);
}

Swe_Time::Swe_Time(int year, int month, int day, double hours)
    : year_(year), month_(month), day_(day), hours_(hours)
{
    jd_ = swe_julday(year, month, day, hours, SE_GREG_CAL);
}

Swe_Time::Swe_Time(int year, int month, int day, int hours, int minutes, double seconds)
    : Swe_Time(year, month, day, hours+minutes/60.0+seconds/3600.0) {
}

int Swe_Time::year()
{
    return year_;
}

int Swe_Time::month()
{
    return month_;
}

int Swe_Time::day()
{
    return day_;
}

double Swe_Time::hours()
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

Date Swe_Time::as_date()
{
    return Date{year_, month_, day_};
}

std::ostream &operator<<(std::ostream &os, Swe_Time const &t) {
    int hours = static_cast<int>(t.hours_);
    double minutes_remain = (t.hours_ - hours) * 60;
    int minutes = static_cast<int>(minutes_remain);
    double seconds = (minutes_remain - minutes) * 60;

    os.width(4);
    os.fill('0');
    os << t.year_ << '-';
    os.width(2);
    os << t.month_ << '-' << std::setw(2) << t.day_ << ' ';
    os.width(2);
    os << hours << ':';
    os.width(2);
    os << minutes << ':' << std::fixed << std::setprecision(6) << std::setw(9) << seconds << " UTC";
    return os;
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

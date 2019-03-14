#ifndef DATE_H
#define DATE_H

#include <ostream>

struct Date
{
    int year, month, day;
    explicit Date(int _year, int _month, int _day)
        :year(_year), month(_month), day(_day){}
};

std::ostream &operator <<(std::ostream &o, Date const &d);
bool operator ==(Date const &, Date const &);
bool operator !=(Date const &, Date const &);

#endif // DATE_H

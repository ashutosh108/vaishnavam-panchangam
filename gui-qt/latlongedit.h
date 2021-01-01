#ifndef LATLONGEDIT_H
#define LATLONGEDIT_H

#include "location.h"

#include <QLineEdit>

class LatLongEdit : public QLineEdit
{
public:
    explicit LatLongEdit(vp::Coord c, QWidget * parent);
    void setCoord(vp::Coord c);
    vp::Coord coord;
};

#endif // LATLONGEDIT_H

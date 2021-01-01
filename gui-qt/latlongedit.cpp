#include "latlongedit.h"

LatLongEdit::LatLongEdit(vp::Coord c, QWidget * parent)
    :QLineEdit(parent),coord{c}
{
}

void LatLongEdit::setCoord(vp::Coord c)
{
    coord = c;
}

#include "latlongedit.h"

LatLongEdit::LatLongEdit(QWidget * parent)
    :QLineEdit(parent)
{
    updateText();
}

void LatLongEdit::setCoord(vp::Coord c)
{
    coord = c;
    updateText();
}

void LatLongEdit::updateText()
{
    setText(QString::fromStdString(fmt::format("{}, {}", coord.latitude, coord.longitude)));
}

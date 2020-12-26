#include "latlongedit.h"

LatLongEdit::LatLongEdit(vp::Latitude lat, vp::Longitude lng, QWidget * parent)
    :QLineEdit(parent),latitude{lat},longitude{lng}
{
}

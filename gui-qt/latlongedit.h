#ifndef LATLONGEDIT_H
#define LATLONGEDIT_H

#include "location.h"

#include <QLineEdit>

class LatLongEdit : public QLineEdit
{
public:
    explicit LatLongEdit(vp::Latitude lat, vp::Longitude lng, QWidget * parent);
private:
    vp::Latitude latitude;
    vp::Longitude longitude;
};

#endif // LATLONGEDIT_H

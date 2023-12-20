#include "latlongedit.h"

#ifdef VP_ARBITRARY_LOCATION_SELECTOR
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#endif /* VP_ARBITRARY_LOCATION_SELECTOR */

LatLongEdit::LatLongEdit(QWidget * parent)
    :QLineEdit(parent)
{
#ifdef VP_ARBITRARY_LOCATION_SELECTOR
    QRegularExpression re("-?\\d{1,3}(\\.\\d{0,20})?[, ]+-?\\d{1,3}(\\.\\d{0,20})?");
    QValidator * validator = new QRegularExpressionValidator(re, this);
    this->setValidator(validator);
#endif /* VP_ARBITRARY_LOCATION_SELECTOR */
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

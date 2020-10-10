#include "latitude-edit.h"

#include "location.h"

#include "fmt-format-fixed.h"

void LatitudeEdit::focusInEvent(QFocusEvent * event)
{
    setText(QString::fromStdString(fmt::format("{:.6f}", value)));
    QLineEdit::focusInEvent(event);
}

void LatitudeEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    value = text().toDouble();
    setText(QString::fromStdString(fmt::to_string(getValue())));
}

void LatitudeEdit::setValue(double _value)
{
    value = _value;
    vp::Latitude latitude {value};
    setText(QString::fromStdString(fmt::to_string(latitude)));
}

vp::Latitude LatitudeEdit::getValue()
{
    return vp::Latitude{value};
}

LatitudeEdit::LatitudeEdit(QWidget *parent) : QLineEdit(parent) {
}

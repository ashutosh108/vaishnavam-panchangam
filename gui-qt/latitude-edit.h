#ifndef LATITUDEEDIT_H
#define LATITUDEEDIT_H

#include "location.h"

#include <QLineEdit>

class LatitudeEdit : public QLineEdit
{
private:
    double value{};
protected:
    void focusInEvent(QFocusEvent * event) override;
    void focusOutEvent(QFocusEvent * event) override;
public:
    void setValue(double _value);
    vp::Latitude getValue();
    LatitudeEdit(QWidget * parent);
};

#endif // LATITUDEEDIT_H

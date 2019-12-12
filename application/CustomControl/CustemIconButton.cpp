#include "CustemIconButton.h"

CustemIconButton::CustemIconButton(QWidget *parent)
    : DIconButton(parent)
{
    setFlat(true);
}

CustemIconButton::~CustemIconButton() {}

void CustemIconButton::setStatus(bool checked)
{
    if (checked) {
        setFlat(false);
    } else {
        setFlat(true);
    }
    update();
}

void CustemIconButton::enterEvent(QEvent *event)
{
    DIconButton::enterEvent(event);
    setFlat(false);
    update();
}

void CustemIconButton::leaveEvent(QEvent *event)
{
    DIconButton::leaveEvent(event);
    if (isChecked()) {
        setFlat(false);
    } else {
        setFlat(true);
    }
    update();
}

void CustemIconButton::mousePressEvent(QMouseEvent *event)
{
    DIconButton::mousePressEvent(event);
    setFlat(false);
    update();
}

void CustemIconButton::mouseReleaseEvent(QMouseEvent *event)
{
    DIconButton::mouseReleaseEvent(event);
    update();
}

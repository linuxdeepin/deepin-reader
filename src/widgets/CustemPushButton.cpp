#include "CustemPushButton.h"
#include <QDebug>

CustemPushButton::CustemPushButton(DWidget *parent):
    DPushButton(parent)
{
    setFocusPolicy(Qt::TabFocus);
}

void CustemPushButton::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        setChecked(!(this->isChecked()));
        emit clicked();
    }
}

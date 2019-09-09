#include "MagnifyLabel.h"
#include <QDebug>

MagnifyLabel::MagnifyLabel(DWidget *parent)
    : DLabel (parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(0, 0, 0, 0);
    initBcakLabel();

    this->setVisible(false);
}

void MagnifyLabel::updatePixmap(const QPixmap &pixmap)
{
    setPixmap(pixmap);
}

void MagnifyLabel::initBcakLabel()
{
    DLabel *backLabel = new DLabel(this);
    backLabel->setMouseTracking(true);
    QPixmap back(":/resources/image/maganifier.svg");

    this->setFixedSize(back.size());

    backLabel->setPixmap(back);
}

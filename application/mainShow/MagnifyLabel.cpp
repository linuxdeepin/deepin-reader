#include "MagnifyLabel.h"

MagnifyLabel::MagnifyLabel(DWidget *parent)
    : DLabel (parent)
{
    setContentsMargins(0, 0, 0, 0);

    setPixmap(QPixmap(":/resources/image/logo/logo_big.svg"));

    initBcakLabel();
}

void MagnifyLabel::initBcakLabel()
{
    DLabel *backLabel = new DLabel(this);
    QPixmap back(":/resources/image/maganifier.svg");

    this->setFixedSize(back.size());

    backLabel->setPixmap(back);
}

#include "LightedWidget.h"
#include <QButtonGroup>

LightedWidget::LightedWidget(CustomWidget *parent)
    : CustomWidget ("LightedWidget", parent)
{
    setFixedHeight(50);

    initWidget();
}

void LightedWidget::SlotOnBtnGroupClicked(int nId)
{
    if (m_nOldId != nId) {
        m_nOldId = nId;

        sendMsg(MSG_OPERATION_TEXT_ADD_HIGHLIGHTED, QString::number(nId));
    }
}

int LightedWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void LightedWidget::initWidget()
{
    DLabel *titleLabel = new DLabel(tr("high lighted"));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    topLayout->addWidget(titleLabel);
    topLayout->addStretch(1);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setContentsMargins(0, 6, 0, 6);
    bottomLayout->setSpacing(8);

    QButtonGroup *btnGroup = new QButtonGroup;
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(SlotOnBtnGroupClicked(int)));

    for (int i = 0; i < 7; i ++ ) {
        DPushButton *btn = new DPushButton(QString::number(i));
        btn->setFixedSize(QSize(16, 16));
        btnGroup->addButton(btn, i);
        bottomLayout->addWidget(btn);
    }

    bottomLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 6, 0, 6);
    mainLayout->setSpacing(3);

    mainLayout->addItem(topLayout);
    mainLayout->addItem(bottomLayout);

    this->setLayout(mainLayout);
}

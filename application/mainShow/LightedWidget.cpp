#include "LightedWidget.h"
#include <QButtonGroup>
#include <DIconButton>
#include "translator/MainShow.h"

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

int LightedWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}

void LightedWidget::initWidget()
{
    DLabel *titleLabel = new DLabel(MainShow::ADD_HIGH_LIGHTED);

    auto topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    topLayout->addWidget(titleLabel);
    topLayout->addStretch(1);

    auto bottomLayout = new QHBoxLayout;
    bottomLayout->setContentsMargins(0, 6, 0, 6);
    bottomLayout->setSpacing(8);

    QButtonGroup *btnGroup = new QButtonGroup;
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(SlotOnBtnGroupClicked(int)));

    for (int i = 0; i < 7; i ++ ) {
        DIconButton *btn = new DIconButton(this);
        btn->setFixedSize(QSize(16, 16));
        btnGroup->addButton(btn, i);
        bottomLayout->addWidget(btn);
    }

    bottomLayout->addStretch(1);

    auto mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 6, 0, 6);
    mainLayout->setSpacing(3);

    mainLayout->addItem(topLayout);
    mainLayout->addItem(bottomLayout);

    this->setLayout(mainLayout);
}

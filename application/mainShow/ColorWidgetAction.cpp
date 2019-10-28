#include "ColorWidgetAction.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DPushButton>
#include <DLabel>
#include <QButtonGroup>
#include "controller/DataManager.h"
#include <DFloatingButton>

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
}

void ColorWidgetAction::initWidget(DWidget *pParent)
{
    auto pWidget = new DWidget(pParent);
    setDefaultWidget(pWidget);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(20, 6, 20, 6);
    buttonLayout->setSpacing(8);

    auto btnGroup = new QButtonGroup;
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(sigBtnGroupClicked(int)));

    auto colorList = DataManager::instance()->getLightColorList();
    for (int iLoop = 0; iLoop < colorList.size(); iLoop++ ) {
        auto btn = new DFloatingButton(pWidget);
        btn->setBackgroundRole(QPalette::Button);
        btn->setFixedSize(QSize(16, 16));
        QPalette pa = btn->palette();
        pa.setBrush(QPalette::Button, colorList.at(iLoop));
        btn->setPalette(pa);
        btnGroup->addButton(btn, iLoop);
        buttonLayout->addWidget(btn);
    }

    buttonLayout->addStretch(1);

    pWidget->setLayout(buttonLayout);
}

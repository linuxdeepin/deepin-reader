#include "ColorWidgetAction.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QButtonGroup>
#include "controller/DataManager.h"
#include <DFloatingButton>
#include <DFontSizeManager>

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
}

void ColorWidgetAction::setBtnAddLightState(const bool &bState)
{
    m_pClickLabel->setEnabled(!bState);
}

void ColorWidgetAction::initWidget(DWidget *pParent)
{
    auto pWidget = new DWidget(pParent);
    setDefaultWidget(pWidget);

    m_pClickLabel = new CustomClickLabel(QString("        %1").arg(tr("add high light")));
    DFontSizeManager::instance()->bind(m_pClickLabel, DFontSizeManager::T6);
    connect(m_pClickLabel, SIGNAL(clicked()), this, SIGNAL(sigBtnDefaultClicked()));

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(25, 6, 20, 6);
    buttonLayout->setSpacing(12);

    auto btnGroup = new QButtonGroup(this);
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(sigBtnGroupClicked(int)));

    auto colorList = DataManager::instance()->getLightColorList();
    for (int iLoop = 0; iLoop < colorList.size(); iLoop++) {
        auto btn = new DFloatingButton(pWidget);
        btn->setBackgroundRole(QPalette::Button);
        btn->setFixedSize(QSize(20, 20));

        QPalette pa = btn->palette();
        pa.setBrush(QPalette::Button, colorList.at(iLoop));
        btn->setPalette(pa);

        btnGroup->addButton(btn, iLoop);
        buttonLayout->addWidget(btn);
    }

    buttonLayout->addStretch(1);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pClickLabel);
    mainLayout->addItem(buttonLayout);

    pWidget->setLayout(mainLayout);
}

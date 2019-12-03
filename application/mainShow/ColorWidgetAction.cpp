#include "ColorWidgetAction.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <DFloatingButton>
#include <DFontSizeManager>
#include <QButtonGroup>
#include "controller/DataManager.h"

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
}

void ColorWidgetAction::setBtnAddLightState(const bool &bState)
{
    m_pClickLabel->setEnabled(!bState);
}

void ColorWidgetAction::slotSetButtonFocus(int index)
{
    auto btnGroup = this->findChild<QButtonGroup *>();
    if (btnGroup) {
        auto btn = btnGroup->button(index);
        if (btn) {
            btn->setFocus(Qt::ActiveWindowFocusReason);
        }
    }
}

void ColorWidgetAction::initWidget(DWidget *pParent)
{
    auto pWidget = new DWidget(pParent);
    setDefaultWidget(pWidget);

    m_pClickLabel = new CustomClickLabel(tr("add high light"));
    DFontSizeManager::instance()->bind(m_pClickLabel, DFontSizeManager::T6);
    connect(m_pClickLabel, SIGNAL(clicked()), this, SIGNAL(sigBtnDefaultClicked()));

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(20, 6, 20, 6);
    buttonLayout->setSpacing(12);

    auto btnGroup = new QButtonGroup(this);
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(sigBtnGroupClicked(int)));
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotSetButtonFocus(int)));

    auto colorList = DataManager::instance()->getLightColorList();
    for (int iLoop = 0; iLoop < colorList.size(); iLoop++) {
        auto btn = new DFloatingButton(pWidget);
        btn->setBackgroundRole(DPalette::Button);
        btn->setFixedSize(QSize(20, 20));

        QPalette pa = btn->palette();
        pa.setBrush(DPalette::Button, colorList.at(iLoop));
        btn->setPalette(pa);

        btnGroup->addButton(btn, iLoop);
        buttonLayout->addWidget(btn);
    }

    buttonLayout->addStretch(1);

    QHBoxLayout *playout = new QHBoxLayout;
    playout->setContentsMargins(23, 0, 0, 0);
    playout->addWidget(m_pClickLabel);

    auto mainLayout = new QVBoxLayout;
    // mainLayout->addWidget(m_pClickLabel);
    mainLayout->addItem(playout);
    mainLayout->addItem(buttonLayout);

    pWidget->setLayout(mainLayout);
}

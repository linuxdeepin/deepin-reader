#include "ColorWidgetAction.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <DFontSizeManager>
#include <QSignalMapper>
#include <QDebug>

#include "controller/DataManager.h"
#include "CustomControl/RoundColorWidget.h"

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
}

void ColorWidgetAction::setBtnAddLightState(const bool &bState)
{
    m_pClickLabel->setEnabled(!bState);
}

void ColorWidgetAction::setcolor(const QColor &color)
{
    QPalette plt = m_pClickLabel->palette();
    plt.setColor(QPalette::ToolTipText, color);
    m_pClickLabel->setPalette(plt);

}

void ColorWidgetAction::setPalette(const QPalette &pal)
{
    QPalette plt = m_pClickLabel->palette();
    plt.setColor(QPalette::ToolTipText, pal.color(QPalette::ToolTipText));
    plt.setColor(QPalette::WindowText, pal.color(QPalette::WindowText));
    // plt.setColor(QPalette::Foreground, DPalette::TextTips);
    m_pClickLabel->setPalette(plt);



    // m_pClickLabel->setForegroundRole(DPalette::TextTips);
}

void ColorWidgetAction::slotBtnClicked(int index)
{
    auto btnList = this->defaultWidget()->findChildren<RoundColorWidget *>();
    foreach (auto btn, btnList) {
        int btnIndex = btn->objectName().toInt();
        if (btnIndex == index) {
            btn->setSelected(true);

            emit sigBtnGroupClicked(index);
        } else {
            btn->setSelected(false);
        }
    }
}

void ColorWidgetAction::slotBtnDefaultClicked()
{
    int iIndex = -1;

    auto btnList = this->defaultWidget()->findChildren<RoundColorWidget *>();
    foreach (auto btn, btnList) {
        if (btn->isSelected()) {
            iIndex = btn->objectName().toInt();
            break;
        }
    }

    if (iIndex == -1) {
        iIndex = 0;
    }

    emit sigBtnGroupClicked(iIndex);
}

void ColorWidgetAction::initWidget(DWidget *pParent)
{
    auto pWidget = new DWidget(pParent);
    setDefaultWidget(pWidget);

    m_pClickLabel = new CustomClickLabel(tr("Highlight"));
    DFontSizeManager::instance()->bind(m_pClickLabel, DFontSizeManager::T6);
    connect(m_pClickLabel, SIGNAL(clicked()), SLOT(slotBtnDefaultClicked()));

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(20, 6, 20, 6);
    buttonLayout->setSpacing(12);

    auto sigMap = new QSignalMapper(this);

    auto colorList = DataManager::instance()->getLightColorList();
    for (int iLoop = 0; iLoop < colorList.size(); iLoop++) {
        auto btn = new RoundColorWidget(colorList.at(iLoop), pWidget);
        btn->setObjectName(QString("%1").arg(iLoop));
        btn->setFixedSize(QSize(25, 25));

        connect(btn, SIGNAL(clicked()), sigMap, SLOT(map()));
        sigMap->setMapping(btn, iLoop);

        buttonLayout->addWidget(btn);
    }
    connect(sigMap, SIGNAL(mapped(int)), SLOT(slotBtnClicked(int)));

    buttonLayout->addStretch(1);

    QHBoxLayout *playout = new QHBoxLayout;
    playout->setContentsMargins(23, 0, 0, 0);
    playout->addWidget(m_pClickLabel);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addItem(playout);
    mainLayout->addItem(buttonLayout);

    pWidget->setLayout(mainLayout);
}

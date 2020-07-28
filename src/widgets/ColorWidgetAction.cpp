#include "ColorWidgetAction.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DFontSizeManager>
#include <QSignalMapper>
#include <QDebug>

#include "Application.h"
#include "Utils.h"
#include "widgets/RoundColorWidget.h"

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
}

void ColorWidgetAction::setBtnAddLightState(const bool &bState)
{
    m_pClickLabel->setEnabled(!bState);
    setcolor(!bState);
}

void ColorWidgetAction::setcolor(bool benable)
{
    QPalette plt = m_pClickLabel->palette();
    if (benable) {
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            plt.setColor(QPalette::WindowText, QColor("#E0E6F4"));
        } else {
            plt.setColor(QPalette::WindowText, QColor(0, 0, 0));
        }
    } else {
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            QColor color("#CECEDE");
            color.setAlpha(100);
            plt.setColor(QPalette::WindowText, color);
        } else {
            QColor color("#000000");
            color.setAlpha(100);
            plt.setColor(QPalette::WindowText, color);
        }
    }

    m_pClickLabel->setPalette(plt);
}


void ColorWidgetAction::slotBtnClicked(int index)
{
    auto btnList = this->defaultWidget()->findChildren<RoundColorWidget *>();
    foreach (auto btn, btnList) {
        int btnIndex = btn->objectName().toInt();
        if (btnIndex == index) {
            btn->setSelected(true);

            Utils::setHiglightColorIndex(index);
            emit sigBtnGroupClicked();
        } else {
            btn->setSelected(false);
        }
    }
}

void ColorWidgetAction::slotBtnDefaultClicked()
{
    Utils::setHiglightColorIndex(getIndex());
    emit sigBtnGroupClicked();
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


    int tW = 25;
    int tH = 25;

    const QList<QColor> &colorlst =  Utils::getHiglightColorList();
    for (int iLoop = 0; iLoop < colorlst.size(); iLoop++) {
        auto btn = new RoundColorWidget(colorlst.at(iLoop), pWidget);
        btn->setAllClickNotify(true);
        btn->setObjectName(QString("%1").arg(iLoop));
        btn->setFixedSize(QSize(tW, tH));
        if (colorlst.at(iLoop) == Utils::getCurHiglightColor())
            btn->setSelected(true);
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

int ColorWidgetAction::getIndex()
{
    int iIndex = 0;

    auto btnList = this->defaultWidget()->findChildren<RoundColorWidget *>();
    foreach (auto btn, btnList) {
        if (btn->isSelected()) {
            iIndex = btn->objectName().toInt();
            break;
        }
    }
    return  iIndex;
}

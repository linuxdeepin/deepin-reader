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
    DWidget *pWidget = new DWidget(pParent);
    setDefaultWidget(pWidget);

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

    QVBoxLayout *mainLayout = new QVBoxLayout;
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

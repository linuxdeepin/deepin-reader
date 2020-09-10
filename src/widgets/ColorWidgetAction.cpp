/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui<duanxiaohui@uniontech.com>
 *
 * Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ColorWidgetAction.h"
#include "Application.h"
#include "Utils.h"
#include "widgets/RoundColorWidget.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QDebug>

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    initWidget(pParent);
    setSeparator(true);
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

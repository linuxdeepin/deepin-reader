// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ColorWidgetAction.h"
#include "Application.h"
#include "Utils.h"
#include "RoundColorWidget.h"
#include "ddlog.h"
#include <QDebug>

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSignalMapper>

ColorWidgetAction::ColorWidgetAction(DWidget *pParent)
    : QWidgetAction(pParent)
{
    qCDebug(appLog) << "ColorWidgetAction created, parent:" << pParent;
    initWidget(pParent);
    setSeparator(true);
}

void ColorWidgetAction::slotBtnClicked(int index)
{
    qCDebug(appLog) << "Color button clicked, index:" << index;
    auto btnList = this->defaultWidget()->findChildren<RoundColorWidget *>();
    foreach (auto btn, btnList) {
        int btnIndex = btn->objectName().toInt();
        if (btnIndex == index) {
            // qCDebug(appLog) << "Color button clicked, index:" << index << "is selected";
            btn->setSelected(true);

            Utils::setHiglightColorIndex(index);
            emit sigBtnGroupClicked();
        } else {
            // qCDebug(appLog) << "Color button clicked, index:" << index << "is not selected";
            btn->setSelected(false);
        }
    }
    qCDebug(appLog) << "Highlight color changed to index:" << index;
}

void ColorWidgetAction::initWidget(DWidget *pParent)
{
    qCDebug(appLog) << "Initializing color widget with" << Utils::getHiglightColorList().size() << "colors";
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        connect(btn, SIGNAL(clicked()), sigMap, SLOT(map()));
#else
        connect(btn, &RoundColorWidget::clicked, this, [this, iLoop]() {
            slotBtnClicked(iLoop);
        });
#endif
        sigMap->setMapping(btn, iLoop);

        buttonLayout->addWidget(btn);
        qCDebug(appLog) << "Color widget initialization completed";
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(sigMap, SIGNAL(mapped(int)), SLOT(slotBtnClicked(int)));
#else
    connect(sigMap, &QSignalMapper::mappedInt,
            this, &ColorWidgetAction::slotBtnClicked);
#endif

    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(buttonLayout);
    pWidget->setLayout(mainLayout);
}

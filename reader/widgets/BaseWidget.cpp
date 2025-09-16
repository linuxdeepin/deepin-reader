// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BaseWidget.h"
#include "ddlog.h"
#include <QDebug>

#include <DGuiApplicationHelper>

BaseWidget::BaseWidget(DWidget *parent)
    : DWidget(parent)
{
    qCDebug(appLog) << "BaseWidget created, parent:" << parent;
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::CustomContextMenu);
    qCDebug(appLog) << "Widget theme updated";
}

BaseWidget::~BaseWidget()
{
    qCDebug(appLog) << "BaseWidget destroyed";
}


void BaseWidget::updateWidgetTheme()
{
    qCDebug(appLog) << "Updating widget theme";
    Dtk::Gui::DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Window, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}

void BaseWidget::adaptWindowSize(const double &ratio)
{
    qCDebug(appLog) << "adaptWindowSize called with ratio:" << ratio;
}

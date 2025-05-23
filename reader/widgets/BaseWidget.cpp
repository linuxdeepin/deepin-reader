// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BaseWidget.h"
#include <QDebug>

#include <DGuiApplicationHelper>

BaseWidget::BaseWidget(DWidget *parent)
    : DWidget(parent)
{
    qDebug() << "BaseWidget created, parent:" << parent;
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::CustomContextMenu);
    qDebug() << "Widget theme updated";
}

BaseWidget::~BaseWidget()
{
    qDebug() << "BaseWidget destroyed";
}


void BaseWidget::updateWidgetTheme()
{
    qDebug() << "Updating widget theme";
    Dtk::Gui::DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Window, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}

void BaseWidget::adaptWindowSize(const double &ratio)
{
    qDebug() << "adaptWindowSize called with ratio:" << ratio;
}

/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "MainTabWidgetEx.h"

#include <QVBoxLayout>
#include <QStackedLayout>

#include <DSplitter>

#include "MainTabBar.h"
#include "MainSplitter.h"

MainTabWidgetEx::MainTabWidgetEx(DWidget *parent)
    : CustomWidget("MainTabWidgetEx", parent)
{
    initWidget();
}

int MainTabWidgetEx::dealWithData(const int &, const QString &)
{
    return 0;
}

void MainTabWidgetEx::initWidget()
{
    MainTabBar *bar = new MainTabBar;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(bar);

    QStackedLayout *sLayout = new QStackedLayout;

    auto pSplitter = new MainSplitter;
    sLayout->addWidget(pSplitter);

    mainLayout->addItem(sLayout);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
}

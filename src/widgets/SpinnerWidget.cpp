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
#include "SpinnerWidget.h"
#include <QVBoxLayout>

#include <DSpinner>

SpinnerWidget::SpinnerWidget(DWidget *parent)
    : DWidget(parent)
{
    InitSpinner();
}

void SpinnerWidget::startSpinner()
{
    auto pSpinner = this->findChild<DSpinner *>();
    if (pSpinner && !pSpinner->isPlaying()) {
        pSpinner->start();
    }
}

void SpinnerWidget::stopSpinner()
{
    auto pSpinner = this->findChild<DSpinner *>();
    if (pSpinner && pSpinner->isPlaying()) {
        pSpinner->stop();
    }
}

void SpinnerWidget::setSpinnerSize(const QSize &s)
{
    auto pSpinner = this->findChild<DSpinner *>();
    if (pSpinner) {
        pSpinner->setFixedSize(s);
    }
}

void SpinnerWidget::InitSpinner()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();

    auto pSpinner = new DSpinner(this);
    pSpinner->stop();

    vLayout->addWidget(pSpinner);
    vLayout->addStretch();

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addItem(vLayout);
    hLayout->addStretch();

    this->setLayout(hLayout);
}

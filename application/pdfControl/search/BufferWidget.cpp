/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#include "BufferWidget.h"

BufferWidget::BufferWidget(DWidget *parent)
    : CustomWidget(QString("BufferWidget"), parent)
{
    initWidget();

    dApp->m_pModelService->addObserver(this);
}

BufferWidget::~BufferWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

int BufferWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void BufferWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;  // 承载spinner的垂直布局
    auto m_pHLayout = new QHBoxLayout();      // 承载spinner的水平布局
    m_pSpinner = new DSpinner(this);     // 缓冲动画

    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pHLayout->setContentsMargins(0, 0, 0, 0);

    int tW = 50;
    int tH = 50;
    dApp->adaptScreenView(tW, tH);
    m_pSpinner->setFixedSize(tW, tW);
    m_pSpinner->stop();

    m_pHLayout->addStretch(1);
    m_pHLayout->addWidget(m_pSpinner);
    m_pHLayout->addStretch(1);

    m_pVLayout->addStretch(1);
    m_pVLayout->addItem(m_pHLayout);
    m_pVLayout->addStretch(1);

    this->setLayout(m_pVLayout);
}

void BufferWidget::SlotSetSpinnerState(const int &iState)
{
    if (m_pSpinner) {
//        if (iState == WIDGET_BUFFER) {
//            m_pSpinner->start();
//        } else {
//            m_pSpinner->stop();
//        }
    }
}

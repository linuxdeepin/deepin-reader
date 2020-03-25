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
#include "BookMarkItemWidget.h"

#include <DLabel>
#include <QHBoxLayout>

BookMarkItemWidget::BookMarkItemWidget(DWidget *parent)
    : CustomItemWidget(BOOKMARK_ITEM_WIDGET, parent)
{
    initWidget();
}

/**
 * @brief BookMarkItemWidget::dealWithData
 * 处理全局消息接口
 * @return
 */
int BookMarkItemWidget::dealWithData(const int &, const QString &)
{
    return MSG_NO_OK;
}

void BookMarkItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }
    m_bPaint = paint;
    update();
}

/**
 * @brief BookMarkItemWidget::initWidget
 * 初始化界面
 */
void BookMarkItemWidget::initWidget()
{
    m_pPageNumber = new PageNumberLabel(this);
    int tW = 31;
    int tH = 18;
    dApp->adaptScreenView(tW, tH);
    m_pPageNumber->setMinimumWidth(tW);
    m_pPageNumber->setFixedHeight(tH);
    m_pPageNumber->setForegroundRole(DPalette::WindowText);
    DFontSizeManager::instance()->bind(m_pPageNumber, DFontSizeManager::T8);

    auto hLine = new DHorizontalLine(this);
    auto m_pRightVLayout = new QVBoxLayout;

    auto m_pPageVLayout = new QHBoxLayout;
    m_pPageVLayout->setContentsMargins(0, 18, 0, 44);
    m_pPageVLayout->addWidget(m_pPageNumber);

    m_pRightVLayout->addItem(m_pPageVLayout);
    m_pRightVLayout->addWidget(hLine);
    m_pRightVLayout->setContentsMargins(20, 0, 10, 0);

    m_pPicture = new ImageLabel(this);
    tW = 48;
    tH = 68;
    dApp->adaptScreenView(tW, tH);
    m_pPicture->setFixedSize(QSize(tW, tH));
    m_pPicture->setSize(QSize(tW, tH));
    m_pPicture->setAlignment(Qt::AlignCenter);

    auto m_pHLayout = new QHBoxLayout;
    m_pHLayout->setContentsMargins(0, 0, 10, 0);
    m_pHLayout->setSpacing(1);
    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addItem(m_pRightVLayout);

    this->setLayout(m_pHLayout);
}

void BookMarkItemWidget::paintEvent(QPaintEvent *event)
{
    CustomItemWidget::paintEvent(event);

    //  涉及到 主题颜色
    if (m_bPaint) {
        m_pPicture->setForegroundRole(DPalette::Highlight);
    } else {
        m_pPicture->setForegroundRole(QPalette::Shadow);
    }
}

/**
 * @brief BookMarkItemWidget::adaptWindowSize
 * 书签缩略图自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void BookMarkItemWidget::adaptWindowSize(const double &scale)
{
//    double width = 1.0;
//    double height = 1.0;

//    width = static_cast<double>(this->width()) * scale;
//    height = static_cast<double>(this->height()) * scale;

//    this->resize(QSize(static_cast<int>(width), static_cast<int>(height)));

    if (m_pPicture) {
        m_pPicture->scaleImage(scale);
    }
}

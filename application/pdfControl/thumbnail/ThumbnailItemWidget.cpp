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
#include "ThumbnailItemWidget.h"

#include <QVBoxLayout>
#include "CustomControl/RotateImageLabel.h"

#include "business/AppInfo.h"

ThumbnailItemWidget::ThumbnailItemWidget(DWidget *parent)
    : CustomItemWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    initWidget();
}

void ThumbnailItemWidget::setLabelImage(const QImage &image)
{
    if (m_label != nullptr) {
        QPixmap pixmap = QPixmap::fromImage(image);
        m_label->setBackgroundPix(pixmap);
    }
}

// 是否被选中，选中就就填充颜色
void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    if (m_label) {
        m_label->setSelect(paint);
    }

    if (m_pPageNumber) {
        m_pPageNumber->setSelect(paint);
    }
}

void ThumbnailItemWidget::rotateThumbnail(int angle)
{
    auto imageLabel = this->findChild<RotateImageLabel *>();
    if (imageLabel) {
        imageLabel->setRotateAngle(angle);
        if (imageLabel->hasThumbnail()) {
            imageLabel->rotateImage();
        }
    }
}

void ThumbnailItemWidget::qSetBookMarkShowStatus(const bool &bshow)
{
    m_label->setBookMarkStatus(bshow);
    m_label->update();
}

// 初始化界面
void ThumbnailItemWidget::initWidget()
{
    m_label = new RotateImageLabel(this);
    m_label->setRadius(ICON_BIG);
    auto labelLayout = new QHBoxLayout;
    labelLayout->addStretch();
    labelLayout->addWidget(m_label);
    labelLayout->addStretch();

    m_pPageNumber = new PageNumberLabel(this);
    m_pPageNumber->setAlignment(Qt::AlignCenter);
    auto pageLayout = new QHBoxLayout;
    pageLayout->addStretch();
    pageLayout->addWidget(m_pPageNumber);
    pageLayout->addStretch();

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(0, 0, 36, 0);
    t_vLayout->setSpacing(0);

    t_vLayout->addStretch();
    t_vLayout->addLayout(labelLayout);
    t_vLayout->addLayout(pageLayout);
    t_vLayout->addStretch();

    this->setLayout(t_vLayout);
}
/**
 * @brief ThumbnailItemWidget::adaptWindowSize
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void ThumbnailItemWidget::adaptWindowSize(const double &scale)
{
    if (m_label) {
        m_label->scaleImage(scale);
    }
}


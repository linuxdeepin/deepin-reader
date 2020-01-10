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
#include "controller/DataManager.h"

ThumbnailItemWidget::ThumbnailItemWidget(DWidget *parent)
    : CustomItemWidget("ThumbnailItemWidget", parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    initWidget();
    connect(this, SIGNAL(sigBookMarkStatusChanged(bool)), SLOT(slotBookMarkShowStatus(bool)));

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

ThumbnailItemWidget::~ThumbnailItemWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

// 处理消息接口
int ThumbnailItemWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_BOOKMARK_DLTITEM == msgType || MSG_OPERATION_DELETE_BOOKMARK == msgType) {   //  删除书签消息
        if (m_nPageIndex == msgContent.toInt()) {
            emit sigBookMarkStatusChanged(false);
        }
    } else if (MSG_OPERATION_ADD_BOOKMARK == msgType || MSG_OPERATION_TEXT_ADD_BOOKMARK == msgType) { //  增加书签消息
        if (m_nPageIndex == msgContent.toInt()) {
            emit sigBookMarkStatusChanged(true);
        }
    }
    return 0;
}

// 是否被选中，选中就就填充颜色
void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }

    if (m_pPageNumber) {
        m_pPageNumber->setSelect(paint);
    }
}

void ThumbnailItemWidget::rotateThumbnail(int angle)
{
    auto imageLabel = this->findChild<ImageLabel *>();
    if (imageLabel) {
        imageLabel->setRotateAngle(angle);
        if (imageLabel->hasThumbnail()) {
            imageLabel->rotateImage();
        }
    }
}

void ThumbnailItemWidget::slotBookMarkShowStatus(bool bshow)
{
    m_pPicture->setBookMarkStatus(bshow);
    m_pPicture->update();
}

// 初始化界面
void ThumbnailItemWidget::initWidget()
{
    m_pPageNumber = new PageNumberLabel(this);
    m_pPageNumber->setMinimumSize(QSize(146, 18));
    m_pPageNumber->setAlignment(Qt::AlignCenter);

    m_pPicture = new ImageLabel(this);
    m_pPicture->setFixedSize(QSize(146, 174));
    m_pPicture->setAlignment(Qt::AlignCenter);
    m_pPicture->setRadius(ICON_BIG);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(0, 0, 0, 0);
    t_vLayout->setSpacing(0);

    t_vLayout->addWidget(m_pPicture);
    t_vLayout->addWidget(m_pPageNumber);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 32, 0);
//    t_hLayout->setSpacing(0);
    t_hLayout->addStretch(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addStretch(1);

    this->setLayout(t_hLayout);
}

void ThumbnailItemWidget::resizeEvent(QResizeEvent *event)
{
    CustomItemWidget::resizeEvent(event);

    auto parentWidget = reinterpret_cast<QWidget *>(this->parent());
    if (parentWidget) {
        resize(parentWidget->width(), this->height());
    }
}

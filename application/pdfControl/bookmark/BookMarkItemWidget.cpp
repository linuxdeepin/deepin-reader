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
#include <DMenu>

#include "controller/DataManager.h"

BookMarkItemWidget::BookMarkItemWidget(QWidget *parent)
    : CustomItemWidget("BookMarkItemWidget", parent)
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(slotShowContextMenu(const QPoint &)));
    initWidget();

    dApp->m_pModelService->addObserver(this);
}

BookMarkItemWidget::~BookMarkItemWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

/**
 * @brief BookMarkItemWidget::dealWithData
 * 处理全局消息接口
 * @return
 */
int BookMarkItemWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    return 0;
}

bool BookMarkItemWidget::bSelect()
{
    if (m_pPicture) {
        return m_pPicture->bSelect();
    }

    return false;
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
 * @brief BookMarkItemWidget::slotDltBookMark
 * 删除当前书签，并发送删除书签全局消息
 */
void BookMarkItemWidget::slotDltBookMark()
{
    sendMsg(MSG_BOOKMARK_DLTITEM, QString::number(m_nPageIndex));
}

/**
 * @brief BookMarkItemWidget::slotShowContextMenu
 * 显示书签右键菜单
 */
void BookMarkItemWidget::slotShowContextMenu(const QPoint &)
{
    sendMsg(MSG_OPERATION_RIGHT_SELECT_BOOKMARK, QString::number(m_nPageIndex));

    if (m_menu == nullptr) {
        m_menu = new DMenu(this);
        DFontSizeManager::instance()->bind(m_menu, DFontSizeManager::T6);
        QAction *dltBookMarkAction =
            m_menu->addAction(tr("Remove bookmark"));
        connect(dltBookMarkAction, SIGNAL(triggered()), this, SLOT(slotDltBookMark()));
    }

    if (m_menu) {
        m_menu->exec(QCursor::pos());
    }
}

/**
 * @brief BookMarkItemWidget::initWidget
 * 初始化界面
 */
void BookMarkItemWidget::initWidget()
{
    m_pPageNumber = new PageNumberLabel(this);
    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
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
    m_pPicture->setFixedSize(QSize(48, 68));
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

//bool BookMarkItemWidget::bOperationBK()
//{
//    int leftShow = 0;
//    int widgetIndex = 0;
//    leftShow = DataManager::instance()->getShowLeft().toInt();
//    widgetIndex = DataManager::instance()->getListIndex().toInt();

//    if (leftShow == 1 && widgetIndex == 2) {
//        return true;
//    }

//    return false;
//}

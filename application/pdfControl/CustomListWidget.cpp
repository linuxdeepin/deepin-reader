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
#include "CustomListWidget.h"

#include <DScrollBar>

#include "application.h"
#include "CustomItemWidget.h"
#include "DocSheet.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

CustomListWidget::CustomListWidget(DocSheet *sheet, DWidget *parent)
    : DListWidget(parent), m_sheet(sheet)
{
    setSpacing(3);

    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListWidget::Adjust);
    setViewMode(QListView::ListMode);
    setSelectionMode(QAbstractItemView::SingleSelection);
    sortItems(Qt::AscendingOrder);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotShowSelectItem(QListWidgetItem *)));
    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SIGNAL(sigValueChanged(int)));
}

//  插入 实现排序
QListWidgetItem *CustomListWidget::insertWidgetItem(const int &iData)
{
    int iInsertIndex = 0;

    //  计算 新增的书签  插入位置
    int nCount = this->count();
    while (nCount > 0) {
        nCount--;

        auto nextItem = this->item(nCount);
        if (nextItem) {
            int nextItemData = nextItem->data(Qt::UserRole + 1).toInt();
            if (nextItemData <= iData) {
                nCount++;
                break;
            }
        }
    }

    iInsertIndex = nCount;
    auto item = new QListWidgetItem;
    item->setData(Qt::UserRole + 1, iData);
    item->setFlags(Qt::NoItemFlags);
    int tW = 230;
    int tH = 80;
//    dApp->adaptScreenView(tW, tH);
    item->setSizeHint(QSize(tW, tH));

    this->insertItem(iInsertIndex, item);

    return item;
}

//  显示 右键菜单
void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButton btn = event->button();
    if (btn == Qt::RightButton) {
        QListWidgetItem *item = this->itemAt(event->pos());
        if (item != nullptr) {
            emit itemClicked(item);

            if (m_nListType == E_NOTE_WIDGET) {
                showNoteMenu();
            } else if (m_nListType == E_BOOKMARK_WIDGET) {
                showBookMarkMenu();
            }
        }
    }

    DListWidget::mousePressEvent(event);
}

/**
 * @brief CustomListWidget::slotShowSelectItem
 * 鼠标点击当前缩略图item，右边fileview中同步到当前页
 * @param item
 */
void CustomListWidget::slotShowSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    emit sigSelectItem(item);

    auto t_ItemWidget = qobject_cast<CustomItemWidget *>(this->itemWidget(item));
    if (t_ItemWidget) {
        int nJumpPage = t_ItemWidget->nPageIndex();
        //  页跳转

        m_sheet->pageJump(nJumpPage);
    }
}

//  显示 注释菜单
void CustomListWidget::showNoteMenu()
{
    if (pNoteMenu == nullptr) {
        pNoteMenu = new NoteMenu(this);
        connect(pNoteMenu, SIGNAL(sigClickAction(const int &)), this, SIGNAL(sigListMenuClick(const int &)));
    }
    pNoteMenu->exec(QCursor::pos());
}

//  显示 书签菜单
void CustomListWidget::showBookMarkMenu()
{
    if (m_pBookMarkMenu == nullptr) {
        m_pBookMarkMenu = new BookMarkMenu(this);
        connect(m_pBookMarkMenu, SIGNAL(sigClickAction(const int &)), this, SIGNAL(sigListMenuClick(const int &)));
    }
    m_pBookMarkMenu->exec(QCursor::pos());
}

void CustomListWidget::setListType(const int &nListType)
{
    m_nListType = nListType;
}


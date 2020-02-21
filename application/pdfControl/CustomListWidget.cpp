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

CustomListWidget::CustomListWidget(DWidget *parent)
    : DListWidget(parent)
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
    item->setSizeHint(QSize(230, 80));

    this->insertItem(iInsertIndex, item);

    return item;
}

/**
 * @brief CustomListWidget::setItemImage
 * 填充缩略图
 * @param row
 * @param image
 */
//void CustomListWidget::slot_loadImage(const int &row, const QImage &image)
//{
//    auto item = this->item(row);
//    if (item) {
//        auto t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
//        if (t_ItemWidget) {
//            t_ItemWidget->setLabelImage(image);
//        }
//    }
//}

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

    auto t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
    if (t_ItemWidget) {
        int nJumpPage = t_ItemWidget->nPageIndex();
        //  页跳转
        dApp->m_pModelService->notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(nJumpPage));
    }
}

//void CustomListWidget::resizeEvent(QResizeEvent *event)
//{
//    DListWidget::resizeEvent(event);

//    auto parentWidget = reinterpret_cast<DWidget *>(this->parent());
//    if (parentWidget) {
//        resize(parentWidget->width(), this->height());
//        qDebug() << "  CustomListWidget  width:" << this->width() << "  height:" << this->height();
//    }
//}

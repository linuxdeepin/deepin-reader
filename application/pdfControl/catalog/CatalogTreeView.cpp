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
#include "CatalogTreeView.h"

#include <QHeaderView>
#include <QVariant>

//#include "CatalogDelegate.h"
//#include "TreeViewItemModel.h"
//#include "TreeItemData.h"


CatalogTreeView::CatalogTreeView(int expansionRole, DWidget *parent)
    : DTreeView(parent), m_expansionRole(expansionRole)
{
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->header()->setHidden(true);

    auto model = new QStandardItemModel();
    this->setModel(model);

    model->setHorizontalHeaderLabels(QStringList() << "" << "");

    QStandardItem *itemProject = new QStandardItem("treeItem_Project");
    QStandardItem *i1 = new QStandardItem("1");
    model->appendRow(itemProject);
    model->setItem(0, 1, i1);

    QStandardItem *itemProject1 = new QStandardItem("2");
    itemProject->appendRow(itemProject1);

    QStandardItem *itemProject2 = new QStandardItem("3");
    itemProject1->appendRow(itemProject2);

    //以下作用同appendRow
    //model->setItem(0,0,itemProject);
    //model->setItem(0,itemProject);
//    代码中m_publicIconMap是定义好的图标其在之前进行初始化，初始化代码如下：
//    m_publicIconMap[QStringLiteral("treeItem_Project")] =QIcon(QStringLiteral(":/treeItemIcon/res_treeItemIcon/Project.png"));
//    m_publicIconMap[QStringLiteral("treeItem_folder")] =QIcon(QStringLiteral(":/treeItemIcon/res_treeItemIcon/folder.png"));
//    m_publicIconMap[QStringLiteral("treeItem_folder-ansys")] =QIcon(QStringLiteral(":/treeItemIcon/res_treeItemIcon/folder-ansys.png"));
//    m_publicIconMap[QStringLiteral("treeItem_group")] =QIcon(QStringLiteral(":/treeItemIcon/res_treeItemIcon/group.png"));
//    m_publicIconMap[QStringLiteral("treeItem_channel")] =QIcon(QStringLiteral(":/treeItemIcon/res_treeItemIcon/channel.png"));


//    connect(this, SIGNAL(expanded(QModelIndex)), SLOT(on_expanded(QModelIndex)));
//    connect(this, SIGNAL(collapsed(QModelIndex)), SLOT(on_collapsed(QModelIndex)));
}

void CatalogTreeView::expandAbove(const QModelIndex &child)
{
    for (QModelIndex index = child.parent(); index.isValid(); index = index.parent()) {
        expand(index);
    }
}

void CatalogTreeView::expandAll(const QModelIndex &index)
{
    if (index.isValid()) {
        if (!isExpanded(index)) {
            expand(index);
        }

        for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
            expandAll(index.child(row, 0));
        }
    } else {
        QTreeView::expandAll();
    }
}

void CatalogTreeView::collapseAll(const QModelIndex &index)
{
    if (index.isValid()) {
        if (isExpanded(index)) {
            collapse(index);
        }

        for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
            collapseAll(index.child(row, 0));
        }
    } else {
        QTreeView::collapseAll();
    }
}

int CatalogTreeView::expandedDepth(const QModelIndex &index)
{
    if (index.isValid()) {
        if (!isExpanded(index) || !model()->hasChildren(index)) {
            return 0;
        }

        int depth = 0;

        for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
            depth = qMax(depth, expandedDepth(index.child(row, 0)));
        }

        return 1 + depth;
    } else {
        int depth = 0;

        for (int row = 0, rowCount = model()->rowCount(); row < rowCount; ++row) {
            depth = qMax(depth, expandedDepth(model()->index(row, 0)));
        }

        return depth;
    }
}

void CatalogTreeView::expandToDepth(const QModelIndex &index, int depth)
{
    if (index.isValid()) {
        if (depth > 0) {
            if (!isExpanded(index)) {
                expand(index);
            }
        }

        if (depth > 1) {
            for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
                expandToDepth(index.child(row, 0), depth - 1);
            }
        }
    } else {
        for (int row = 0, rowCount = model()->rowCount(); row < rowCount; ++row) {
            expandToDepth(model()->index(row, 0), depth);
        }
    }
}

void CatalogTreeView::collapseFromDepth(const QModelIndex &index, int depth)
{
    if (index.isValid()) {
        if (depth <= 0) {
            if (isExpanded(index)) {
                collapse(index);
            }
        }

        for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
            collapseFromDepth(index.child(row, 0), depth - 1);
        }
    } else {
        for (int row = 0, rowCount = model()->rowCount(); row < rowCount; ++row) {
            collapseFromDepth(model()->index(row, 0), depth);
        }
    }
}

void CatalogTreeView::restoreExpansion(const QModelIndex &index)
{
    if (index.isValid()) {
        const bool expanded = index.data(m_expansionRole).toBool();

        if (isExpanded(index) != expanded) {
            setExpanded(index, expanded);
        }
    }

    for (int row = 0, rowCount = model()->rowCount(index); row < rowCount; ++row) {
        restoreExpansion(model()->index(row, 0, index));
    }
}

/*
void CatalogTreeView::keyPressEvent(QKeyEvent *event)
{
    const bool verticalKeys = event->key() == Qt::Key_Up || event->key() == Qt::Key_Down;
    const bool horizontalKeys = event->key() == Qt::Key_Left || event->key() == Qt::Key_Right;

    const QModelIndex selection = firstIndex(selectedIndexes());

    // If Shift is pressed, the view is scrolled up or down.
    if (event->modifiers().testFlag(Qt::ShiftModifier) && verticalKeys) {
        QScrollBar *scrollBar = verticalScrollBar();

        if (event->key() == Qt::Key_Up && scrollBar->value() > scrollBar->minimum()) {
            scrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);

            event->accept();
            return;
        } else if (event->key() == Qt::Key_Down && scrollBar->value() < scrollBar->maximum()) {
            scrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);

            event->accept();
            return;
        }
    }

    // If Control is pressed, all children of the selected item are expanded or collapsed.
    if (event->modifiers().testFlag(Qt::ControlModifier) && horizontalKeys) {
        if (event->key() == Qt::Key_Left) {
            collapseAll(selection);
        } else if (event->key() == Qt::Key_Right) {
            expandAll(selection);
        }

        event->accept();
        return;
    }

    // If Shift is pressed, one level of children of the selected item are expanded or collapsed.
    if (event->modifiers().testFlag(Qt::ShiftModifier) && horizontalKeys) {
        const int depth = expandedDepth(selection);

        if (event->key() == Qt::Key_Left) {
            collapseFromDepth(selection, depth - 1);
        } else if (event->key() == Qt::Key_Right) {
            expandToDepth(selection, depth + 1);
        }

        event->accept();
        return;
    }

    QTreeView::keyPressEvent(event);
}

void CatalogTreeView::wheelEvent(QWheelEvent *event)
{
    const QModelIndex selection = firstIndex(selectedIndexes());

    // If Control is pressed, expand or collapse the selected entry.
    if (event->modifiers().testFlag(Qt::ControlModifier) && selection.isValid()) {
        if (event->delta() > 0) {
            collapse(selection);
        } else {
            expand(selection);
        }

        // Fall through when Shift is also pressed.
        if (!event->modifiers().testFlag(Qt::ShiftModifier)) {
            event->accept();
            return;
        }
    }

    // If Shift is pressed, move the selected entry up and down.
    if (event->modifiers().testFlag(Qt::ShiftModifier) && selection.isValid()) {
        QModelIndex sibling;

        if (event->delta() > 0) {
            sibling = indexAbove(selection);
        } else {
            sibling = indexBelow(selection);
        }

        if (sibling.isValid()) {
            setCurrentIndex(sibling);
        }

        event->accept();
        return;
    }

    QTreeView::wheelEvent(event);
}

void CatalogTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QTreeView::contextMenuEvent(event);

    if (!event->isAccepted()) {
        QMenu menu;

        const QAction *expandAllAction = menu.addAction(tr("&Expand all"));
        const QAction *collapseAllAction = menu.addAction(tr("&Collapse all"));

        const QAction *action = menu.exec(event->globalPos());

        if (action == expandAllAction) {
            expandAll(indexAt(event->pos()));
        } else if (action == collapseAllAction) {
            collapseAll(indexAt(event->pos()));
        }
    }
}
*/

void CatalogTreeView::on_expanded(const QModelIndex &index)
{
    model()->setData(index, true, m_expansionRole);
}

void CatalogTreeView::on_collapsed(const QModelIndex &index)
{
    model()->setData(index, false, m_expansionRole);
}

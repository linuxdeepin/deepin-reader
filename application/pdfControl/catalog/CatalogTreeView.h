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
#ifndef CATALOGTREEVIEW_H
#define CATALOGTREEVIEW_H

#include <DTreeView>
#include <QStandardItemModel>
#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

class CatalogDelegate;
class TreeViewItemModel;

// tree view

class CatalogTreeView : public DTreeView
{
    Q_OBJECT
    Q_DISABLE_COPY(CatalogTreeView)

public:
    explicit CatalogTreeView(int expansionRole, DWidget *parent = nullptr);

public slots:
    void expandAbove(const QModelIndex &child);

    void expandAll(const QModelIndex &index = QModelIndex());
    void collapseAll(const QModelIndex &index = QModelIndex());

    int expandedDepth(const QModelIndex &index);

    void expandToDepth(const QModelIndex &index, int depth);
    void collapseFromDepth(const QModelIndex &index, int depth);

    void restoreExpansion(const QModelIndex &index = QModelIndex());

protected:
//    void keyPressEvent(QKeyEvent *event);
//    void wheelEvent(QWheelEvent *event);

//    void contextMenuEvent(QContextMenuEvent *event);

protected slots:
    void on_expanded(const QModelIndex &index);
    void on_collapsed(const QModelIndex &index);

private:
    int m_expansionRole = -1;
};

#endif // CATALOGTREEVIEW_H

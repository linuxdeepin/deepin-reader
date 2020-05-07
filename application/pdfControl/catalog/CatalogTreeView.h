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

#include "pdfControl/docview/pagebase.h"

DWIDGET_USE_NAMESPACE

class DocSheet;
class CatalogTreeView : public DTreeView
{
    Q_OBJECT
    Q_DISABLE_COPY(CatalogTreeView)

public:
    explicit CatalogTreeView(DocSheet *sheet, DWidget *parent = nullptr);

    ~CatalogTreeView() override;

    void setPage(int page);

    void setRightControl(bool hasControl);

public:
    void handleOpenSuccess();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

    void onItemClicked(const QModelIndex &current);

    void slotThemeChanged();

private:
    void parseCatalogData(const Section &, QStandardItem *);

    QList<QStandardItem *> getItemList(const QString &, const int &, const qreal  &realleft, const qreal &realtop);

private slots:
    void SlotCollapsed(const QModelIndex &);

    void SlotExpanded(const QModelIndex &);

private:
    bool rightnotifypagechanged = false;
    DocSheet *m_sheet;
    QList<QStandardItem *> m_listTitle;
    QList<QStandardItem *> m_listPage;
    int m_page;
};

#endif // CATALOGTREEVIEW_H

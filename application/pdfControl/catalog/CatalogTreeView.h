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

#include "subjectObserver/IObserver.h"
#include "docview/pagebase.h"

class SubjectThread;

DWIDGET_USE_NAMESPACE

class CatalogTreeView : public DTreeView, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(CatalogTreeView)

public:
    explicit CatalogTreeView(DWidget *parent = nullptr);
    ~CatalogTreeView() Q_DECL_OVERRIDE;

signals:
    void sigOpenFileOk();
    void sigFilePageChanged(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void initConnections();
    void parseCatalogData(const Section &, QStandardItem *);

    QList<QStandardItem *>   getItemList(const QString &, const int &, const qreal  &realleft, const qreal &realtop);

private slots:
    void SlotOpenFileOk();

    void SlotClicked(const QModelIndex &);
    void SlotFilePageChanged(const QString &);

    void SlotCollapsed(const QModelIndex &);
    void SlotExpanded(const QModelIndex &);

private:
    SubjectThread       *m_pSubjectThread = nullptr;
};

#endif // CATALOGTREEVIEW_H

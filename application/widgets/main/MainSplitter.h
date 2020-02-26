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
#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include <DSplitter>

#include "CustomControl/CustomWidget.h"

#include "application.h"

DWIDGET_USE_NAMESPACE

class MainSplitter : public DSplitter, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitter)

public:
    explicit MainSplitter(CustomWidget *parent = nullptr);
    ~MainSplitter() Q_DECL_OVERRIDE;

signals:
    void sigTitleMsgData(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

    QString qGetPath() const;
    void qSetPath(const QString &strPath);

private:
    void InitWidget();
    void InitConnections();

private slots:
    void SlotTitleMsg(const QString &);

private:
    QString         m_strPath = "";
};

#endif // MAINSPLITTER_H

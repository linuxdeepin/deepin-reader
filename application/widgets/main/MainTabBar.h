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
#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <DTabBar>
#include <DTabWidget>
#include "application.h"

DWIDGET_USE_NAMESPACE

class MainTabBar : public DTabBar, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(MainTabBar)

public:
    explicit MainTabBar(DWidget *parent = nullptr);
    ~MainTabBar() override;

signals:
    void sigDealWithData(const int &, const QString &);
    void sigTabBarIndexChange(const QString &);
    void sigAddTab(const QString &);
    void sigCloseTab(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &, const QString &) override;
    void notifyMsg(const int &, const QString &s = "") override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void __InitConnection();

    void __SetTabMiniWidth();
    void AddFileTab(const QString &);
    QString getFileName(const QString &strFilePath);

private slots:
    void SlotTabBarClicked(int);
    void SlotTabAddRequested();
    void SlotTabCloseRequested(int index);

    void SlotRemoveFileTab(const QString &);
    void SlotDealWithData(const int &, const QString &);

private:
    int     m_nOldMiniWidth = 0;
    QList <int> m_pMsgList;
};

#endif // MAINTABWIDGET_H

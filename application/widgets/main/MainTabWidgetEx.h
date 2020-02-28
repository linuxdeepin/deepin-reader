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
#ifndef MAINTABWIDGETEX_H
#define MAINTABWIDGETEX_H

#include "CustomControl/CustomWidget.h"

class QStackedLayout;

class MainTabWidgetEx : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainTabWidgetEx)

public:
    explicit MainTabWidgetEx(DWidget *parent = nullptr);
    ~MainTabWidgetEx() override;

signals:
    void sigCloseFile(const QString &);
    void sigRemoveFileTab(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private:
    void InitConnections();

private slots:
    void SlotSetCurrentIndexFile(const QString &);
    void SlotAddTab(const QString &);

    void SlotCloseFile(const QString &);
//    void SlotTabRemoveFile(const int &, const QString &);

private:
    QStackedLayout  *m_pStackedLayout = nullptr;
};

#endif // MAINTABWIDGETEX_H

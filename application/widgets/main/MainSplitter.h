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
#include "application.h"

DWIDGET_USE_NAMESPACE

class DocShowShellWidget;
class LeftSidebarWidget;
class MainSplitterPrivate;

class MainSplitter : public DSplitter, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitter)

public:
    explicit MainSplitter(DWidget *parent = nullptr);
    ~MainSplitter() override;

signals:
    void sigDealWithDataMsg(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &, const QString &) override;
    void notifyMsg(const int &, const QString &) override;

public:
    QString qGetPath() const;
    void qSetPath(const QString &strPath);

    void qSetFileChange(const int &);
    int qGetFileChange();

    void saveData();

    FileDataModel qGetFileData() const;
    void setFileData(const FileDataModel &) const;

private:
    void InitWidget();
    void InitConnections();

private slots:
    void SlotDealWithDataMsg(const int &, const QString &);
    void SlotSplitterMsg(const int &, const QString &);
    void SlotNotifyMsg(const int &, const QString &);

    void SlotMainTabWidgetExMsg();

private:
    LeftSidebarWidget   *m_pLeftWidget = nullptr;
    DocShowShellWidget  *m_pDocWidget   = nullptr;

    MainSplitterPrivate     *d_ptr = nullptr;
};

#endif // MAINSPLITTER_H

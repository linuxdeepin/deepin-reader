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
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QMap>

#include "ModuleHeader.h"

class FileDataModel;

class DocummentProxy;
class QStackedLayout;
class DocTabBar;
class PlayControlWidget;
class CentralDocPage : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CentralDocPage)

public:
    explicit CentralDocPage(DWidget *parent = nullptr);
    ~CentralDocPage() override;

    friend class DocTabBar;
public:
    static CentralDocPage *Instance();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

public:
    QStringList qGetAllPath();

    QString qGetCurPath();

    int getFileChanged();

    int GetFileChange(const QString &sPath);

    FileDataModel qGetFileData(const QString &sPath = "") ;

    DocummentProxy *getCurFileAndProxy(const QString &sPath = "");

    void showPlayControlWidget() const;

    int getCurrentState();

    void setCurrentState(const int &nCurrentState);

    void SetFileChange();

    void OnExitSliderShow();

    void OnExitMagnifer();

    void setCurrentTabByFilePath(const QString &filePath);

private:
    void OpenCurFileFolder();

    QString GetCurPath();

    QStringList GetAllPath();

    void SetFileChange(const QString &sPath, const int &iState);

protected:
    void initWidget() override;

private:
    void BlockShutdown();
    void UnBlockShutdown();
    void InitConnections();
    void onShowFileAttr();
    void OnAppMsgData(const QString &);
    void OnAppExit();
    void OnTabBarMsg(const QString &);
    void OnTabFileChangeMsg(const QString &);
    void SaveFile(const int &nSaveType, const QString &);
    void OnSaveAsFile();
    void OnAppShortCut(const QString &);
    void OnSaveFile();
    void OnPrintFile();
    void ShowFindWidget();
    void OnOpenSliderShow();
    void OnOpenMagnifer();
    void OnShortCutKey_Esc();
    void OnKeyPress(const QString &);

signals:
    void sigDealNotifyMsg(const int &, const QString &);
    void sigOpenFileResult(const QString &, const bool &);
    void sigRemoveFileTab(const QString &);
    void sigTabBarIndexChange(const QString &);
    void sigAddTab(const QString &);

public slots:
    void slotfilechanged(bool bchanged);
private slots:
    void SlotSetCurrentIndexFile(const QString &);
    void SlotAddTab(const QString &);
    void SlotCloseTab(const QString &);
    void SlotOpenFileResult(const QString &, const bool &);

private:
    QStackedLayout      *m_pStackedLayout = nullptr;
    DocTabBar           *m_pTabBar = nullptr;
    PlayControlWidget   *m_pctrlwidget = nullptr;
    QDBusInterface      *m_pLoginManager = nullptr;

    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QList<QVariant> m_arg;
    bool m_bBlockShutdown = false;
    static CentralDocPage *g_onlyApp;

private:
    QList<int>          m_pMsgList;
    QString             m_strSliderPath = "";
    QString             m_strMagniferPath = "";
    int                 m_nCurrentState = Default_State;
};

#endif // MAINTABWIDGETEX_H

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
#include "app/ModuleHeader.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QMap>

class FileDataModel;
class DocummentProxy;
class QStackedLayout;
class DocTabBar;
class PlayControlWidget;
class DocSheet;
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

public:
    int dealWithData(const int &, const QString &) override;

public:
    QStringList qGetAllPath();

    QString qGetCurPath();

    int getCurFileChanged();

    int GetFileChange(const QString &sPath);

    FileDataModel qGetFileData(const QString &sPath = "") ;

    DocummentProxy *getCurFileAndProxy(const QString &sPath = "");

    DocSheet *getCurSheet();

    DocSheet *getSheet(const QString &filePath);

    void showPlayControlWidget() const;

    int getCurrentState();

    void setCurrentState(const int &nCurrentState);

    void OnExitSliderShow();

    void OnExitMagnifer();

    void setCurrentTabByFilePath(const QString &filePath);

public:
    void OpenCurFileFolder();

    QStringList GetAllPath();

public:
    void pageJump(const int &pagenum);

    void pageJumpByMsg(const int &, const QString &param);

    void notifyMsg(const int &msgType, const QString &msgContent = "") ;

    void CloseFile(const int &iType, const QString &sPath);

    QString qDealWithData(const int &msgType, const QString &msgContent);

protected:
    void initWidget() override;

public:
    void BlockShutdown();

    void UnBlockShutdown();

    void InitConnections();

    void onShowFileAttr();

    void OnAppMsgData(const QString &);

    void OnAppExit();

    void OnTabBarMsg(const QString &);

    void OnTabFileChangeMsg(const QString &);

    void SaveFile(const int &nSaveType, const QString &);

    void OnAppShortCut(const QString &);

    void onSaveFile();

    void OnPrintFile();

    void ShowFindWidget();

    void OnOpenSliderShow();

    bool OnOpenMagnifer();

    void OnShortCutKey_Esc();

    void OnKeyPress(const QString &);

public:
    void saveCurFile();

    void saveAsCurFile();

signals:
    void sigDealNotifyMsg(const int &, const QString &);

    void sigOpenFileResult(const QString &, const bool &);

    void sigRemoveFileTab(const QString &);

    void sigTabBarIndexChange(const QString &);

    void sigCurSheetChanged(DocSheet *);        //当前的文档被 切换 改动 保存 等

    void sigTitleShortCut(QString shortcut);

    void sigNeedShowTip(const QString &msg);

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
    QList<int>          m_pMsgList2;
    QString             m_strSliderPath = "";
    QString             m_strMagniferPath = "";
    int                 m_nCurrentState = Default_State;
};

#endif // MAINTABWIDGETEX_H

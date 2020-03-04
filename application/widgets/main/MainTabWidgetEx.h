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

class DocummentProxy;
class FindWidget;
class QStackedLayout;
class MainTabBar;
class PlayControlWidget;

//  当前的操作状态
enum E_CUR_STATE {
    SLIDER_SHOW
};

class MainTabWidgetEx : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainTabWidgetEx)

public:
    explicit MainTabWidgetEx(DWidget *parent = nullptr);
    ~MainTabWidgetEx() override;

private:
    static MainTabWidgetEx *g_onlyApp;

public:
    static MainTabWidgetEx *Instance();

signals:
    void sigDealNotifyMsg(const int &, const QString &);

    void sigRemoveFileTab(const QString &);

    void sigTabBarIndexChange(const QString &);
    void sigAddTab(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

public:
    QStringList qGetAllPath() const;
    QString qGetCurPath() const;
    int GetCurFileState(const QString &);
    FileDataModel qGetFileData(const QString &) const;
    void SetFileData(const QString &, const FileDataModel &);

    void InsertPathProxy(const QString &, DocummentProxy *);

    // CustomWidget interface
    DocummentProxy *getCurFileAndProxy(const QString &sPath = "") const;

    void showPlayControlWidget() const;
    int getCurrentState() const;

protected:
    void initWidget() override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void InitConnections();
    void onShowFileAttr();

    void OnAppMsgData(const QString &);
    void OnAppExit();

    void OnTabBarMsg(const QString &);
    void OnTabFileChangeMsg(const QString &);
    void SetFileChange(const QString &sPath, const int &iState);

    void SaveFile(const int &nSaveType, const QString &);
    void OnSaveAsFile();

    void OnAppShortCut(const QString &);
    void OnSaveFile();
    void OnPrintFile();
    void ShowFindWidget();
    void OnOpenSliderShow();
    void OnExitSliderShow();

    void OpenCurFileFolder();
    void OnShortCutKey_Esc();

    void OnKeyPress(const QString &);

private slots:
    void SlotSetCurrentIndexFile(const QString &);
    void SlotAddTab(const QString &);
    void SlotCloseTab(const QString &);

private:
    QStackedLayout  *m_pStackedLayout = nullptr;
    MainTabBar      *m_pTabBar = nullptr;

    QMap<QString, DocummentProxy *> m_strOpenFileAndProxy;

    FindWidget          *m_pFindWidget = nullptr;
    PlayControlWidget   *m_pctrlwidget = nullptr;
    QString             m_strSliderPath = "";

    int                 m_nCurrentState = -1;
};

#endif // MAINTABWIDGETEX_H

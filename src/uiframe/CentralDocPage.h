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
#include "DocSheet.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QMap>
#include <QPointer>

class FileDataModel;
class DocummentProxy;
class QStackedLayout;
class DocTabBar;
class CentralDocPage : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CentralDocPage)

public:
    explicit CentralDocPage(DWidget *parent = nullptr);

    ~CentralDocPage() override;

    friend class DocTabBar;

public:
    void openFile(QString &filePath);

    void addSheet(DocSheet *);      //直接添加sheet

    bool hasSheet(DocSheet *sheet);

    void showSheet(DocSheet *sheet);

    bool saveAll();

    bool saveCurrent();

    bool saveAsCurrent();

    void handleShortcut(const QString &);

    DocSheet *getCurSheet();

    DocSheet *getSheet(const QString &filePath);

    void showFileAttr();

    void openMagnifer();

    void quitMagnifer();

    void openSlide();

    void quitSlide();

    void openCurFileFolder();

    void BlockShutdown();

    void UnBlockShutdown();

public slots:
    void onOpened(DocSheet *, bool);

    void onTabChanged(DocSheet *);          //切换

    void onTabMoveIn(DocSheet *);           //移入添加

    void onTabClosed(DocSheet *);           //被关闭

    void onTabMoveOut(DocSheet *);          //被移走

    void onTabNewWindow(DocSheet *);        //移出程序 新建窗口

    void onCentralMoveIn(DocSheet *);       //从正文部分移入

    void onSheetChanged(DocSheet *);        //文档被修改

signals:
    void sigSheetCountChanged(int);

    void sigNeedOpenFilesExec();

    void sigCurSheetChanged(DocSheet *);        //当前的文档被 切换 改动 保存 等

    void sigFindOperation(const int &);

signals:
    void sigRemoveFileTab(const QString &);

    void sigTabBarIndexChange(const QString &);

    void sigNeedClose();

    void sigNeedActivateWindow();

private:
    QStackedLayout      *m_pStackedLayout = nullptr;
    DocTabBar           *m_pTabBar = nullptr;
    QDBusInterface      *m_pLoginManager = nullptr;

    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QList<QVariant> m_arg;
    bool m_bBlockShutdown = false;

    QPointer<DocSheet>  m_magniferSheet = nullptr;
    QPointer<DocSheet>  m_slideSheet = nullptr;
};

#endif // MAINTABWIDGETEX_H

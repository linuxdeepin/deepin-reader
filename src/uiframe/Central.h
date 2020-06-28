/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#ifndef CENTRAL_H
#define CENTRAL_H

#include "CustomControl/CustomWidget.h"

class CentralDocPage;
class CentralNavPage;
class TitleMenu;
class TitleWidget;
class DocSheet;
class QStackedLayout;
class Central : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Central)

public:
    explicit Central(QWidget *parent = nullptr);

    ~Central() override;

    TitleMenu *titleMenu();

    TitleWidget *titleWidget();

    void openFilesExec();

    void openFiles(QStringList filePathList);

    void addSheet(DocSheet *sheet);

    bool hasSheet(DocSheet *sheet);

    void showSheet(DocSheet *sheet);

    bool saveAll();

    void handleShortcut(QString shortcut);

    void doOpenFile(QString filePath);

signals:
    void sigOpenFiles(const QString &);

    void sigNeedClose();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

public slots:
    void onSheetCountChanged(int count);

    void onMenuTriggered(const QString &action);

    void onOpenFilesExec();

    void onNeedActivateWindow();

    void onShowTips(const QString &text, int iconIndex = 0);

    void slotScaleChanged(double);
private:
    QStackedLayout      *m_layout = nullptr;
    CentralDocPage      *m_docPage = nullptr;
    CentralNavPage      *m_navPage = nullptr;
    TitleMenu           *m_menu = nullptr;
    TitleWidget         *m_widget = nullptr;
};

#endif  // CENTRAL_H

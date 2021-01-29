/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
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
#include "Application.h"
#include "Utils.h"
#include "MainWindow.h"
#include "PageRenderThread.h"
#include "DocSheet.h"
#include "SaveDialog.h"
#include "DBusObject.h"

#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QMouseEvent>

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    loadTranslator();
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    setApplicationName("deepin-reader");
    setOrganizationName("deepin");
    //setWindowIcon(QIcon::fromTheme("deepin-reader"));     //耗时40ms
    setApplicationVersion(DApplication::buildVersion("1.0.0"));
    setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin_reader");
    setApplicationDisplayName(tr("Document Viewer"));
    setApplicationDescription(tr("Document Viewer is a tool for reading document files, supporting PDF, DJVU, etc."));
    setProductIcon(QIcon::fromTheme("deepin-reader"));
}

Application::~Application()
{
    PageRenderThread::destroyForever();
    DBusObject::destory();
}

void Application::emitSheetChanged()
{
    emit sigSetPasswdFocus();
}

void Application::handleQuitAction()
{
    QList<DocSheet *> changedList = DocSheet::getChangedList();

    if (changedList.size() > 0) {   //需要提示保存
        SaveDialog sd;

        int nRes = sd.showExitDialog();

        if (nRes <= 0)
            return;

        if (nRes == 2)
            DocSheet::saveList(changedList);
    }

    foreach (MainWindow *window, MainWindow::m_list) {
        window->closeWithoutSave();
    }
}


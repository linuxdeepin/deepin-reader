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
#include "widgets/SaveDialog.h"
#include "PageViewportThread.h"

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
    setApplicationVersion(DApplication::buildVersion("1.0"));
    setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin_reader");
    setApplicationDisplayName(tr("Document Viewer"));
    setApplicationDescription(tr("Document Viewer is a tool for reading document files, supporting PDF, DJVU, etc."));
    setProductIcon(QIcon::fromTheme("deepin-reader"));
}

void Application::blockShutdown()
{
    if (isBlockShutdown)
        return;

    if (blockShutdownReply.value().isValid()) {
        return;
    }

    if (blockShutdownInterface == nullptr)
        blockShutdownInterface = new QDBusInterface("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus());

    QList<QVariant> args;
    args << QString("shutdown") // what
         << qApp->applicationDisplayName() // who
         << QObject::tr("Document not saved") // why
         << QString("delay"); // mode

    blockShutdownReply = blockShutdownInterface->callWithArgumentList(QDBus::Block, "Inhibit", args);
    if (blockShutdownReply.isValid()) {
        blockShutdownReply.value().fileDescriptor();
        isBlockShutdown = true;
    } else {
        qDebug() << blockShutdownReply.error();
    }
}

void Application::unBlockShutdown()
{
    if (blockShutdownReply.isValid()) {
        blockShutdownReply = QDBusReply<QDBusUnixFileDescriptor>();
        isBlockShutdown = false;
    }
}

void Application::showAnnotTextWidgetSig()
{
    emit sigShowAnnotTextWidget();
}

void Application::emitSheetChanged()
{
    emit sigSetPasswdFocus();
}

void Application::handleFiles(QStringList filePathList)
{
    QList<DocSheet *> sheets = DocSheet::g_map.values();

    if (filePathList.count() <= 0) {
        MainWindow::createWindow()->show();
        return;
    }

    foreach (QString filePath, filePathList) {
        //如果存在则活跃该窗口
        bool hasFind = false;
        foreach (DocSheet *sheet, sheets) {
            if (sheet->filePath() == filePath) {
                MainWindow *window = MainWindow::windowContainSheet(sheet);
                if (nullptr != window) {
                    window->activateSheet(sheet);
                    hasFind = true;
                    break;
                }
            }
        }

        if (!hasFind) {
            //如果不存在则打开
            if (MainWindow::m_list.count() > 0) {
                MainWindow::m_list[0]->setWindowState((MainWindow::m_list[0]->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                MainWindow::m_list[0]->doOpenFile(filePath);
                continue;
            } else {
                MainWindow::createWindow()->doOpenFile(filePath);
            }
        }
    }
}

void Application::handleQuitAction()
{
    QList<DocSheet *> changedList;

    foreach (auto sheet, DocSheet::g_map.values()) {
        if (sheet->fileChanged())
            changedList.append(sheet);
    }

    if (changedList.size() > 0) {   //需要提示保存
        SaveDialog sd;

        int nRes = sd.showExitDialog();

        if (nRes <= 0) {
            return;
        }

        if (nRes == 2) {
            foreach (auto sheet, changedList) {
                sheet->saveData();
            }
        }
    }

    foreach (MainWindow *window, MainWindow::m_list) {
        window->closeWithoutSave();
    }
}

bool Application::notify(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyevent = static_cast<QKeyEvent *>(event);
        if ((object->inherits("QAbstractButton")) && (keyevent->key() == Qt::Key_Return || keyevent->key() == Qt::Key_Enter)) {
            QAbstractButton *pushButton = dynamic_cast<QAbstractButton *>(object);
            if (pushButton) {
                emit pushButton->clicked(!pushButton->isChecked());
                return true;
            }
        }

        if ((keyevent->modifiers() == Qt::AltModifier) && keyevent->key() == Qt::Key_M) {
            // 光标中心点
            QPoint pos = QPoint(static_cast<int>(qApp->inputMethod()->cursorRectangle().x() + qApp->inputMethod()->cursorRectangle().width() / 2),
                                static_cast<int>(qApp->inputMethod()->cursorRectangle().y() + qApp->inputMethod()->cursorRectangle().height() / 2));

            // QPoint(0,0) 表示无法获取光标位置
            if (pos != QPoint(0, 0)) {
                QMouseEvent event1(QEvent::MouseButtonPress, pos, Qt::RightButton, Qt::NoButton, Qt::NoModifier);
                QCoreApplication::sendEvent(object, &event1);
            }
        }
    }
    return DApplication::notify(object, event);
}


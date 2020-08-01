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
    args << QString("shutdown")             // what
         << qApp->applicationDisplayName()           // who
         << QObject::tr("Document not saved") // why
         << QString("block");                        // mode

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
                MainWindow::m_list[0]->activateWindow();
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

    //线程退出
    PageViewportThread::destroyForever();
    PageRenderThread::destroyForever();

    foreach (MainWindow *window, MainWindow::m_list) {
        window->closeWithoutSave();
    }
}


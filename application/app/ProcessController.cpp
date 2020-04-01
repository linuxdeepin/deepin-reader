#include "ProcessController.h"
#include "json.h"
#include "CentralDocPage.h"
#include "MainWindow.h"
#include "MsgHeader.h"

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QProcess>

#include <QLocalServer>
#include <QLocalSocket>
#include <QApplication>
#include <QJsonDocument>
#include <DFileDialog>

ProcessController::ProcessController(QObject *parent) : QObject(parent)
{

}

ProcessController::~ProcessController()
{
    if (m_localServer)
        m_localServer->close();
}

bool ProcessController::checkFilePathOpened(const QString &filePath)
{
    QStringList list = findReaderPids();
    foreach (QString pid, list) {

        Json json;
        json.set("command", "checkFilePath");
        json.set("message", filePath);

        QString result = request(pid, json.toString());

        if ("contains" == result)
            return true;
    }

    return false;
}

bool ProcessController::openIfAppExist(const QStringList &filePathList)
{
    QStringList list = findReaderPids();
    if (list.count() <= 0)
        return false;

    Json json;
    json.set("command", "openNewFile");
    json.set("message", filePathList);

    QString result = request(list.value(0), json.toString());

    return "exist" == result;
}

bool ProcessController::existFilePath(const QString &filePath)
{
    QStringList list = findReaderPids();
    foreach (QString pid, list) {

        Json json;
        json.set("command", "existFilePath");
        json.set("message", filePath);

        QString result = request(pid, json.toString());

        if ("contains" == result)
            return true;
    }

    return false;
}

bool ProcessController::listen()
{
    pid_t pid = getpid();

    m_localServer = new QLocalServer(this);

    connect(m_localServer, SIGNAL(newConnection()), this, SLOT(onReceiveMessage()));

    return m_localServer->listen(QString::number(pid));
}

bool ProcessController::execOpenFiles()
{
    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter(Utils::getSuffixList());
    dialog.setDirectory(QDir::homePath());

    if (QDialog::Accepted != dialog.exec()) {
        return false;
    }

    QStringList files = dialog.selectedFiles();

    bool opened = false;
    if (files.size() > 0) {
        foreach (auto filePath, files) {
            if (!ProcessController::existFilePath(filePath)) {
                dApp->m_pModelService->notifyMsg(MSG_TAB_ADD, filePath);
                opened = true;
            }
        }
    }

    return opened;
}

void ProcessController::processOpenFile(const QString &filePath)
{
    QProcess app;
    app.startDetached(QString("%1 \"%2\"").arg(qApp->applicationDirPath() + "/deepin-reader").arg(filePath));
}

QString ProcessController::request(const QString &pid, const QString &message)
{
    QString result;
    QLocalSocket localSocket;
    localSocket.connectToServer(pid, QIODevice::ReadWrite);
    if (!localSocket.waitForConnected(100)) {
        qDebug() << localSocket.error();
        return result;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(100)) {
        return result;
    }

    if (!localSocket.waitForReadyRead(100))
        return result;

    result  = localSocket.readAll();

    localSocket.disconnectFromServer();

    return result;
}

void ProcessController::onReceiveMessage()
{
    //此处存在指令未接收全风险
    QLocalSocket *localSocket = m_localServer->nextPendingConnection();

    if (!localSocket->waitForReadyRead(1000)) {
        return;
    }

    QByteArray byteArray = localSocket->readAll();

    Json json(QString::fromUtf8(byteArray.constData()));

    if ("checkFilePath" == json.getString("command")) {

        CentralDocPage *ex = CentralDocPage::Instance();
        MainWindow *window = MainWindow::Instance();
        QString filePath = json.getString("message");

        if (ex && window && ex->qGetAllPath().contains(filePath)) {
            localSocket->write("contains");

        } else {
            localSocket->write("none");
        }

        localSocket->waitForBytesWritten(1000);
    } else if ("existFilePath" == json.getString("command")) {

        CentralDocPage *ex = CentralDocPage::Instance();
        MainWindow *window = MainWindow::Instance();
        QString filePath = json.getString("message");

        if (ex && window && ex->qGetAllPath().contains(filePath)) {
            localSocket->write("contains");
            ex->setCurrentTabByFilePath(filePath);
            window->activateWindow();

        } else {
            localSocket->write("none");
        }

        localSocket->waitForBytesWritten(1000);
    } else if ("openNewFile" == json.getString("command")) {
        QStringList fileOpenList = json.getStringList("message");

        CentralDocPage *ex = CentralDocPage::Instance();
        MainWindow *window = MainWindow::Instance();

        if (ex && window ) {
            localSocket->write("exist");

            for (int i = 0 ; i < fileOpenList.count(); ++i) {
                MainWindow::Instance()->openfile(fileOpenList.value(i));
                if (i == fileOpenList.count() - 1)
                    ex->setCurrentTabByFilePath(fileOpenList.value(i));
            }

            window->activateWindow();

        } else {
            localSocket->write("none");
        }
    }

    localSocket->disconnectFromServer();
}

QStringList ProcessController::findReaderPids()
{
    //此函数未验证内存释放

    QStringList list;
    QString ProcName = qApp->applicationName();

    DIR             *dir;
    struct dirent   *d;
    int             pid;
    char            *s;
    int pnlen = ProcName.length();

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir) {
        printf("cannot open /proc");
        return list;
    }

    /* Walk through the directory. */
    while ((d = readdir(dir)) != nullptr) {

        char exe [PATH_MAX + 1];
        char path[PATH_MAX + 1];
        int len;
        int namelen;

        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0)       continue;

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = static_cast<int>(readlink(exe, path, PATH_MAX))) < 0)
            continue;
        path[len] = '\0';

        /* Find ProcName */
        s = strrchr(path, '/');
        if (s == nullptr) continue;
        s++;

        /* we don't need small name len */
        namelen = static_cast<int>(strlen(s));
        if (namelen < pnlen)     continue;

        if (!strncmp(QByteArray().append(ProcName).data(), s, static_cast<unsigned long>(pnlen))) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if (s[pnlen] == ' ' || s[pnlen] == '\0') {
                if (getpid() != pid)
                    list.append(QString::number(pid));
            }
        }
    }

    closedir(dir);

    return  list;
}


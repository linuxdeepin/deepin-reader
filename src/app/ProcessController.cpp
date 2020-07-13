#include "ProcessController.h"
#include "json.h"
#include "CentralDocPage.h"
#include "MainWindow.h"
#include "MsgHeader.h"
#include "DocSheet.h"

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
#include <QStandardPaths>
#include <QUuid>
#include <QTimer>

ProcessController::ProcessController(QObject *parent) : QObject(parent)
{
//    start();
//    moveToThread(this);
}

ProcessController::~ProcessController()
{
    if (m_localServer)
        m_localServer->close();

    if (m_localServer)
        delete m_localServer;
    if (m_timer)
        delete m_timer;

    //quit();
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
    Json json;
    json.set("command", "openNewFile");
    json.set("message", filePathList);

    QString result = request(json.toString());

    return "done" == result;
}

void ProcessController::handleFiles(QStringList filePathList)
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
                    window->showNormal();
                    window->activateSheet(sheet);
                    hasFind = true;
                    break;
                }
            }
        }

        if (!hasFind) {
            //如果不存在则打开
            if (MainWindow::m_list.count() > 0) {
                MainWindow::m_list[0]->showNormal();
                MainWindow::m_list[0]->activateWindow();
                MainWindow::m_list[0]->doOpenFile(filePath);
                continue;
            } else {
                MainWindow::createWindow()->doOpenFile(filePath);
            }
        }
    }
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
    m_localServer = new QLocalServer;

    connect(m_localServer, SIGNAL(newConnection()), this, SLOT(onReceiveMessage()));

    m_timer = new QTimer;

    connect(m_timer, SIGNAL(timeout()), this, SLOT(onHeart()));

    m_timer->start(5000);

    m_listenText = QUuid::createUuid().toString();

    writeListenText(m_listenText);

    return m_localServer->listen(m_listenText);
}

QString ProcessController::request(const QString &message)
{
    QString listenText = readListenText();

    if (listenText.isEmpty())
        return "";

    QString result;
    QLocalSocket localSocket;
    localSocket.connectToServer(listenText, QIODevice::ReadWrite);
    if (!localSocket.waitForConnected(10000)) {
        qDebug() << localSocket.error();
        return result;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(10000)) {
        return result;
    }

    if (!localSocket.waitForReadyRead(10000))
        return result;

    result  = localSocket.readAll();

    localSocket.disconnectFromServer();

    return result;
}


void ProcessController::writeListenText(QString listenText)
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/.listenText");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    file.write(QByteArray().append(listenText));

    file.close();
}

QString ProcessController::readListenText()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/.listenText");

    QString listenText;

    if (file.open(QIODevice::ReadOnly))
        listenText = file.readAll();

    file.close();

    return listenText;
}

QString ProcessController::request(const QString &pid, const QString &message)
{
    QString result;
    QLocalSocket localSocket;
    localSocket.connectToServer(pid, QIODevice::ReadWrite);
    if (!localSocket.waitForConnected(10000)) {
        qDebug() << localSocket.error();
        return result;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(10000)) {
        return result;
    }

    if (!localSocket.waitForReadyRead(10000))
        return result;

    result  = localSocket.readAll();

    localSocket.disconnectFromServer();

    return result;
}

void ProcessController::onReceiveMessage()
{
    QLocalSocket *localSocket = m_localServer->nextPendingConnection();

    qDebug() << "reveive";
    if (!localSocket->waitForReadyRead(10000)) {
        return;
    }
    qDebug() << "reveiveF";
    QByteArray byteArray = localSocket->readAll();

    Json json(QString::fromUtf8(byteArray.constData()));

    if ("openNewFile" == json.getString("command")) {

        qDebug() << json.getStringList("message");

        localSocket->write("done");
        localSocket->disconnectFromServer();

        QStringList filePathList = json.getStringList("message");

        handleFiles(filePathList);

        return;
    }

    localSocket->write("none");
    localSocket->disconnectFromServer();
}

void ProcessController::onHeart()
{
    if (!m_localServer->isListening()) {
        listen();
    }

    QString listenText = readListenText();

    if (listenText != m_listenText)
        writeListenText(m_listenText);
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


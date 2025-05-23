// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DBusObject.h"
#include "MainWindow.h"
#include "Application.h"
#include "Global.h"

#include <QDBusConnection>
#include <QUrl>
#include <QDebug>

#include <DSysInfo>

// gesture 触控板手势
#define DBUS_SERVER             "com.deepin.Reader"
#define DBUS_SERVER_PATH        "/com/deepin/Reader"

#define DBUS_IM                 "com.deepin.im"
#define DBUS_IM_PATH            "/com/deepin/im"
#define DBUS_IM_INTERFACE       "com.deepin.im"
#define DBUS_IM_SIGNAL          "imActiveChanged"

DCORE_USE_NAMESPACE

DBusObject *DBusObject::s_instance = nullptr;

DBusObject *DBusObject::instance()
{
    if (nullptr == s_instance) {
        qDebug() << "Creating new DBusObject instance";
        s_instance = new DBusObject;
    }

    qDebug() << "Returning DBusObject instance";
    return s_instance;
}

void DBusObject::destory()
{
    if (nullptr != s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

bool DBusObject::registerOrNotify(QStringList arguments)
{
    qDebug() << "Registering DBus service:" << DBUS_SERVER;
    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerService(DBUS_SERVER)) {
        qDebug() << "Service already registered, sending notification";
        QDBusInterface notification(DBUS_SERVER, DBUS_SERVER_PATH, DBUS_SERVER, QDBusConnection::sessionBus());
        QList<QVariant> args;
        args.append(arguments);
        QString error = notification.callWithArgumentList(QDBus::Block, "handleFiles", args).errorMessage();
        if (!error.isEmpty())
            qWarning() << "DBus notification error:" << error;
        return false;
    }

    qDebug() << "Registering DBus object path:" << DBUS_SERVER_PATH;
    dbus.registerObject(DBUS_SERVER_PATH, this, QDBusConnection::ExportScriptableSlots);

    const QString gestureSignal = "Event";
    QString gestureName = "com.deepin.daemon.Gesture";
    QString gesturePath = "/com/deepin/daemon/Gesture";
    // check if os edition on v23 or later
    const int osMajor = DSysInfo::majorVersion().toInt();
    if (osMajor >= 23) {
        gestureName = "org.deepin.dde.Gesture1";
        gesturePath = "/org/deepin/dde/Gesture1";
    }

    QDBusConnection::systemBus().connect(gestureName, gesturePath, gestureName, gestureSignal, this, SIGNAL(sigTouchPadEventSignal(QString, QString, int)));

    return true;
}

void DBusObject::unRegister()
{
    qDebug() << "Unregistering DBus service:" << DBUS_SERVER;
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(DBUS_SERVER);
}

void DBusObject::blockShutdown()
{
    if (m_isBlockShutdown) {
        qDebug() << "Shutdown already blocked";
        return;
    }

    if (m_blockShutdownReply.value().isValid()) {
        qDebug() << "Valid shutdown block already exists";
        return;
    }

    if (m_blockShutdownInterface == nullptr) {
        qDebug() << "Creating shutdown block interface";
        m_blockShutdownInterface = new QDBusInterface("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus());
    }

    qDebug() << "Blocking system shutdown";
    QList<QVariant> args;
    args << QString("shutdown") // what
         << qApp->applicationDisplayName() // who
         << QObject::tr("Document not saved") // why
         << QString("delay"); // mode

    m_blockShutdownReply = m_blockShutdownInterface->callWithArgumentList(QDBus::Block, "Inhibit", args);
    if (m_blockShutdownReply.isValid()) {
        qDebug() << "Shutdown blocked successfully";
        m_blockShutdownReply.value().fileDescriptor();
        m_isBlockShutdown = true;
    } else {
        qWarning() << "Failed to block shutdown:" << m_blockShutdownReply.error();
    }
}

void DBusObject::unBlockShutdown()
{
    if (m_blockShutdownReply.isValid()) {
        m_blockShutdownReply = QDBusReply<QDBusUnixFileDescriptor>();
        m_isBlockShutdown = false;
    }
}

void DBusObject::handleFiles(QStringList filePathList)
{
    qDebug() << "Handling files via DBus, count:" << filePathList.count();
    if (filePathList.count() <= 0) {
        qDebug() << "No files provided, creating empty window";
        MainWindow::createWindow()->show();
        return;
    }

    MainWindow *mainwindow = MainWindow::m_list.count() > 0 ? MainWindow::m_list[0] : MainWindow::createWindow();
    qDebug() << "Using main window:" << mainwindow;
    mainwindow->setProperty("loading", true);
    
    foreach (QString filePath, filePathList) {
        qDebug() << "Processing file:" << filePath;
        QUrl url(filePath);
        if (url.isLocalFile()) {
            filePath = url.toLocalFile();
            qDebug() << "Converted to local path:" << filePath;
        }

        if (mainwindow->property("windowClosed").toBool()) {
            qDebug() << "Window closed, stopping file processing";
            break;
        }

        if (!MainWindow::activateSheetIfExist(filePath)) {
            qDebug() << "Adding new file to window";
            mainwindow->setWindowState((MainWindow::m_list[0]->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            mainwindow->addFile(filePath);
        } else {
            qDebug() << "File already open in existing window";
        }
    }

    mainwindow->setProperty("loading", false);
    qDebug() << "Finished processing all files";
}

DBusObject::DBusObject(QObject *parent) : QObject(parent)
{

}

DBusObject::~DBusObject()
{
    qDebug() << "Destroying DBusObject";
    m_blockShutdownReply = QDBusReply<QDBusUnixFileDescriptor>();
    m_isBlockShutdown = false;

    if (nullptr != m_blockShutdownInterface) {
        qDebug() << "Cleaning up shutdown block interface";
        delete m_blockShutdownInterface;
    }
}

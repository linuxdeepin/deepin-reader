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
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <DApplication>

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>

class AppInfo;

#if defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Reader")
public:
    Application(int &argc, char **argv);

    void blockShutdown();

    void unBlockShutdown();

    void addRenderTask();

    void showAnnotTextWidgetSig();

signals:
    // 触控板事件 name 事件名称 direction 手势方向 fingers 手指个数
    void sigTouchPadEventSignal(QString name, QString direction, int fingers);

    void sigShowAnnotTextWidget();

public slots:
    Q_SCRIPTABLE void handleFiles(QStringList filePathList);

protected:
    void handleQuitAction() override;

    bool notify(QObject *object, QEvent *event) override;

public:
    bool isBlockShutdown = false;

    QDBusInterface *blockShutdownInterface = nullptr;

    QDBusReply<QDBusUnixFileDescriptor> blockShutdownReply;

};

#endif  // APPLICATION_H_

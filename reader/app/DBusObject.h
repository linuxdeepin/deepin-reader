/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef DBUSOBJECT_H
#define DBUSOBJECT_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>

class DBusObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Reader")
public:
    static DBusObject *instance();

    static void destory();

    /**
     * @brief registerOrNotify
     * 注册服务，成功返回true,如果失败则通知已经存在的服务并返回false
     * @param arguments 注册失败时通知已存在服务的参数
     * @return
     */
    bool registerOrNotify(QStringList arguments);

    /**
     * @brief unRegister
     * 反注册，当应用准备销毁时调用
     */
    void unRegister();

    /**
     * @brief blockShutdown
     * 文档变化阻塞关闭
     */
    void blockShutdown();

    /**
     * @brief unBlockShutdown
     * 文档未变化不阻塞关闭
     */
    void unBlockShutdown();

signals:
    /**
     * @brief sigTouchPadEventSignal
     * 系统分发的触摸事件
     * @param name  事件名称
     * @param direction  手势方向
     * @param fingers  手指个数
     */
    void sigTouchPadEventSignal(QString name, QString direction, int fingers);

public slots:
    /**
     * @brief handleFiles
     * 接收DBus打开文件响应接口
     * @param filePathList
     */
    Q_SCRIPTABLE void handleFiles(QStringList filePathList);

private:
    Q_DISABLE_COPY(DBusObject)

    static DBusObject *s_instance;

    explicit DBusObject(QObject *parent = nullptr);

    ~DBusObject();

    bool m_isBlockShutdown = false;

    QDBusInterface *m_blockShutdownInterface = nullptr;

    QDBusReply<QDBusUnixFileDescriptor> m_blockShutdownReply;
};

#endif // DBUSOBJECT_H

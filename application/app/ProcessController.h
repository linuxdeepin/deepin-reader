/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:zhangsong
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
#ifndef PROCESSCONTROLLER_H
#define PROCESSCONTROLLER_H

#include <QObject>

class QLocalServer;
class QTimer;
class ProcessController : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "com.deepin.Reader")
public:
    explicit ProcessController(QObject *parent = nullptr);

    virtual ~ProcessController();

    bool listen();

public slots:
    void onReceiveMessage();

    void onHeart();

    Q_SCRIPTABLE void handleFiles(QStringList filePathList);

public:
    static bool checkFilePathOpened(const QString &filePath);   //仅仅查看是否被打开

    static bool existFilePath(const QString &filePath);   //获取打开对应文档程序的pid，为空则文档未打开

    static bool openIfAppExist(const QStringList &filePathList);

private:
    static void writeListenText(QString listenText);

    static QString readListenText();

private:
    static QString request(const QString &message);

    static QString request(const QString &pid, const QString &message);

    static QStringList findReaderPids();

private:
    QLocalServer *m_localServer = nullptr;
    QString m_listenText;
    QTimer *m_timer = nullptr;
};

#endif // PROCESSCONTROLLER_H

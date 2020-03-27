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
class ProcessController : public QObject
{
    Q_OBJECT
public:
    explicit ProcessController(QObject *parent = nullptr);

    virtual ~ProcessController();

    static bool existFilePath(const QString &filePath);   //获取打开对应文档程序的pid，为空则文档未打开

    bool openIfAppExist(const QStringList &filePathList);

    bool listen();

    static void execOpenFiles();    //此方法会考虑是否已存在打开此文件进程

    static void processOpenFile(const QString &filePath);  //

private slots:
    void onReceiveMessage();

private:
    static QString request(const QString &pid, const QString &message);

    static QStringList findReaderPids();

private:
    QLocalServer *m_localServer = nullptr;

};

#endif // PROCESSCONTROLLER_H

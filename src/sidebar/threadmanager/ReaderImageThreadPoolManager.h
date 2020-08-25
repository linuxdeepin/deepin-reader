/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef READERIMAGETHREADPOOLMANAGER_H
#define READERIMAGETHREADPOOLMANAGER_H

#include <QThreadPool>
#include <QPixmap>
#include <QRunnable>
#include <QMap>
#include <QMutex>

class DocSheet;
typedef struct ReaderImageParam_t {
    bool bForceUpdate = false;

    int pageIndex = 0;
    int maxPixel = 174;
    DocSheet *sheet = nullptr;

    QObject *receiver = nullptr;
    QString slotFun = "";

    bool operator == (const ReaderImageParam_t &other) const
    {
        return (this->pageIndex == other.pageIndex && this->maxPixel == other.maxPixel && this->sheet == other.sheet);
    }

    bool operator < (const ReaderImageParam_t &other) const
    {
        return (this->pageIndex < other.pageIndex);
    }

    bool operator > (const ReaderImageParam_t &other) const
    {
        return (this->pageIndex > other.pageIndex);
    }

private:
    friend class ReaderImageThreadPoolManager;
    QRunnable *task = nullptr;
} ReaderImageParam_t;
Q_DECLARE_METATYPE(ReaderImageParam_t);

class ReadImageTask: public QRunnable
{
public:
    void addgetDocImageTask(const ReaderImageParam_t &readImageParam);
    void setThreadPoolManager(QObject *object);
    void run() override;

private:
    QObject *m_threadpoolManager;
    ReaderImageParam_t m_docParam;
};

class ReaderImageThreadPoolManager: public QThreadPool
{
    Q_OBJECT

public:
    explicit ReaderImageThreadPoolManager();
    static ReaderImageThreadPoolManager *getInstance();

public:
    void addgetDocImageTask(const ReaderImageParam_t &readImageParam);
    QPixmap getImageForDocSheet(DocSheet *sheet, int pageIndex);

private slots:
    void onTaskFinished(const ReaderImageParam_t &task, const QImage &image);
    void onDocProxyDestroyed(QObject *obj);
    void onReceiverDestroyed(QObject *obj);

private:
    QMutex m_runMutex;
    QList<QObject *> m_docProxylst;
    QList<ReaderImageParam_t> m_taskList;
    QMap<QObject *, QVector<QPixmap>> m_docSheetImgMap;
};

#endif // READERIMAGETHREADPOOLMANAGER_H;

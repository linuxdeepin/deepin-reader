/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
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

class DocummentProxy;
typedef struct ReaderImageParam_t{
    int pageNum = 0;
    int maxPixel = 174;
    DocummentProxy* docProxy = nullptr;

    QObject *receiver = nullptr;
    const char* slotFun = "";

    bool operator == (const ReaderImageParam_t& other) const{
        return (this->pageNum == other.pageNum && this->maxPixel == other.maxPixel && this->docProxy == other.docProxy);
    }

    bool operator < (const ReaderImageParam_t& other) const{
        return (this->pageNum < other.pageNum);
    }

    bool operator > (const ReaderImageParam_t& other) const{
        return (this->pageNum > other.pageNum);
    }

private:
    friend class ReaderImageThreadPoolManager;
    QRunnable *task = nullptr;
}ReaderImageParam_t;
Q_DECLARE_METATYPE(ReaderImageParam_t);

class ReadImageTask: public QRunnable{
public:
    void addgetDocImageTask(const ReaderImageParam_t &readImageParam);
    void setThreadPoolManager(QObject* object);
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
    static ReaderImageThreadPoolManager* getInstance();

public:
    void addgetDocImageTask(const ReaderImageParam_t &readImageParam);
    QPixmap getImageForDocSheet(DocummentProxy* docproxy, int pageIndex);

private slots:
    void onTaskFinished(const ReaderImageParam_t& task, const QImage& image);
    void onDocProxyDestroyed(QObject *obj);

private:
    QMutex m_runMutex;
    QList<QObject *> m_docProxylst;
    QList<ReaderImageParam_t> m_taskList;
    QMap<QObject*, QVector<QPixmap>> m_docSheetImgMap;
};

#endif // READERIMAGETHREADPOOLMANAGER_H;

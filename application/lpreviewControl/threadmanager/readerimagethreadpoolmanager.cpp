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
#include "readerimagethreadpoolmanager.h"
#include "DocSheet.h"

#include <QDebug>

const int maxThreadCnt = 5;
const int maxTaskList = 5;
const char *threadPoolSlotFun = "onTaskFinished";
void ReadImageTask::addgetDocImageTask(const ReaderImageParam_t &readImageParam)
{
    m_docParam = readImageParam;
}

void ReadImageTask::setThreadPoolManager(QObject *object)
{
    m_threadpoolManager = object;
}

void ReadImageTask::run()
{
    QImage image;
    DocSheet *sheet = m_docParam.sheet;
    Q_ASSERT(sheet);
    if (sheet) {
        int totalPage = sheet->getPageSNum();
        m_docParam.pageNum = qBound(0, m_docParam.pageNum, totalPage - 1);
        bool bl = sheet->getImageMax(m_docParam.pageNum, image, m_docParam.maxPixel);
        if (bl) QMetaObject::invokeMethod(m_threadpoolManager, threadPoolSlotFun, Qt::QueuedConnection, Q_ARG(const ReaderImageParam_t &, m_docParam), Q_ARG(const QImage &, image));
        QThread::sleep(1);
    }
}

Q_GLOBAL_STATIC(ReaderImageThreadPoolManager, theInstance)
ReaderImageThreadPoolManager::ReaderImageThreadPoolManager()
{
    setMaxThreadCount(maxThreadCnt);
}

ReaderImageThreadPoolManager *ReaderImageThreadPoolManager::getInstance()
{
    return theInstance();
}

void ReaderImageThreadPoolManager::addgetDocImageTask(const ReaderImageParam_t &readImageParam)
{
    //To avoid repetitive tasks
    if (m_taskList.contains(readImageParam)) {
        return;
    }

    //only initOnce
    if (!m_docProxylst.contains(readImageParam.sheet)) {
        m_docProxylst << readImageParam.sheet;
        QVector<QPixmap> imagelst(readImageParam.sheet->getPageSNum());
        m_docSheetImgMap.insert(readImageParam.sheet, imagelst);
        connect(readImageParam.sheet, &QObject::destroyed, this, &ReaderImageThreadPoolManager::onDocProxyDestroyed);
    }

    //remove invalid task
    QMutexLocker mutext(&m_runMutex);
    if (m_taskList.size() >= maxTaskList) {
        for (int index = maxTaskList; index < m_taskList.size(); index++) {
            QRunnable *runable = m_taskList.at(index).task;
            if (this->tryTake(runable)) {
                delete runable;
                m_taskList.removeAt(index);
                index--;
            }
        }
    }

    ReaderImageParam_t tParam = readImageParam;
    ReadImageTask *task = new ReadImageTask;
    tParam.task = task;
    task->setThreadPoolManager(this);
    task->addgetDocImageTask(tParam);
    m_taskList << tParam;
    this->start(task);
}

void ReaderImageThreadPoolManager::onTaskFinished(const ReaderImageParam_t &task, const QImage &image)
{
    QMutexLocker mutext(&m_runMutex);
    if (m_docSheetImgMap.contains(task.sheet))
        m_docSheetImgMap[task.sheet][task.pageNum] = QPixmap::fromImage(image);
    if (task.receiver)
        QMetaObject::invokeMethod(task.receiver, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(int, task.pageNum));
    m_taskList.removeAll(task);
}

QPixmap ReaderImageThreadPoolManager::getImageForDocSheet(DocSheet *sheet, int pageIndex)
{
    if (m_docSheetImgMap.contains(sheet)) {
        return m_docSheetImgMap[sheet][pageIndex];
    }
    return QPixmap();
}

void ReaderImageThreadPoolManager::onDocProxyDestroyed(QObject *obj)
{
    m_docProxylst.removeAll(obj);
    m_docSheetImgMap.remove(obj);
}

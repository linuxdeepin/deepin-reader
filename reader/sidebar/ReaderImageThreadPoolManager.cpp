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
#include "ReaderImageThreadPoolManager.h"
#include "DocSheet.h"

#include <QUuid>

const int maxThreadCnt = 5;
const int maxTaskList = 5;
const static char *threadPoolSlotFun = "onTaskFinished";

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
    if (!DocSheet::getUuid(sheet).isNull() && sheet->isUnLocked()) {
        int totalPage = sheet->pageCount();
        m_docParam.pageIndex = qBound(0, m_docParam.pageIndex, totalPage - 1);
        bool bl = sheet->getImage(m_docParam.pageIndex, image, m_docParam.maxPixel, m_docParam.maxPixel);
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
        QVector<QPixmap> imagelst(readImageParam.sheet->pageCount());
        Q_ASSERT(imagelst.size() > 0 && "pagesNum == 0");
        m_docSheetImgMap.insert(readImageParam.sheet, imagelst);
        connect(readImageParam.sheet, &QObject::destroyed, this, &ReaderImageThreadPoolManager::onDocProxyDestroyed);
    }

    //remove invalid task
    QMutexLocker mutext(&m_runMutex);
    if (!readImageParam.bForceUpdate && m_taskList.size() >= maxTaskList) {
        for (int index = maxTaskList; index < m_taskList.size(); index++) {
            QRunnable *runable = m_taskList.at(index).task;
            if (this->tryTake(runable)) {
                delete runable;
                m_taskList.removeAt(index);
                index--;
            }
        }
    }

    connect(readImageParam.receiver, &QObject::destroyed, this, &ReaderImageThreadPoolManager::onReceiverDestroyed);
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

    setImageForDocSheet(task.sheet, task.pageIndex, QPixmap::fromImage(image));

    if (m_taskList.contains(task)) {
        QMetaObject::invokeMethod(task.receiver, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(int, task.pageIndex));
        m_taskList.removeAll(task);
    }
}

QPixmap ReaderImageThreadPoolManager::getImageForDocSheet(DocSheet *sheet, int pageIndex)
{
    if (m_docSheetImgMap.contains(sheet)) {
        return m_docSheetImgMap[sheet][pageIndex];
    }
    return QPixmap();
}

void ReaderImageThreadPoolManager::setImageForDocSheet(DocSheet *sheet, int pageIndex, const QPixmap &pixmap)
{
    if (pageIndex >= 0 && m_docSheetImgMap.contains(sheet) && m_docSheetImgMap[sheet].size() > pageIndex) {
        m_docSheetImgMap[sheet][pageIndex] = pixmap;
    }
}

void ReaderImageThreadPoolManager::onDocProxyDestroyed(QObject *obj)
{
    m_docProxylst.removeAll(obj);
    m_docSheetImgMap.remove(obj);
}

void ReaderImageThreadPoolManager::onReceiverDestroyed(QObject *obj)
{
    for (const ReaderImageParam_t &iter : m_taskList) {
        if (iter.receiver == obj) {
            m_taskList.removeAll(iter);
            return;
        }
    }
}

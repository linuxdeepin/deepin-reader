// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReaderImageThreadPoolManager.h"
#include "DocSheet.h"
#include "Application.h"
#include <QDebug>

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
    DocSheet *sheet = m_docParam.sheet;
    if (!DocSheet::getUuid(sheet).isNull() && sheet->opened()) {
        int totalPage = sheet->pageCount();

        m_docParam.pageIndex = qBound(0, m_docParam.pageIndex, totalPage - 1);

        QSizeF size = sheet->pageSizeByIndex(m_docParam.pageIndex);

        size.scale(m_docParam.boundedRect, Qt::KeepAspectRatio);

        QImage image = sheet->getImage(m_docParam.pageIndex, size.width() * dApp->devicePixelRatio(), size.height() * dApp->devicePixelRatio());

        image.setDevicePixelRatio(dApp->devicePixelRatio());

        if (!image.isNull())
            QMetaObject::invokeMethod(m_threadpoolManager, threadPoolSlotFun, Qt::QueuedConnection, Q_ARG(const ReaderImageParam_t &, m_docParam), Q_ARG(const QImage &, image));

        QThread::sleep(1);
    }
}

Q_GLOBAL_STATIC(ReaderImageThreadPoolManager, theInstance)
ReaderImageThreadPoolManager::ReaderImageThreadPoolManager()
{
    qDebug() << "Initializing ReaderImageThreadPoolManager with max threads:" << maxThreadCnt;
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
        qDebug() << "Skip duplicate task for page:" << readImageParam.pageIndex;
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
        qDebug() << "Task queue full (" << m_taskList.size() << "), removing excess tasks";
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
    qDebug() << "Starting new image load task for page:" << tParam.pageIndex
             << "total tasks:" << m_taskList.size();
    this->start(task);
}

void ReaderImageThreadPoolManager::onTaskFinished(const ReaderImageParam_t &task, const QImage &image)
{
    QMutexLocker mutext(&m_runMutex);

    QPixmap pixmap = QPixmap::fromImage(image);
    qDebug() << "Task finished for page:" << task.pageIndex
             << "image size:" << image.size()
             << "remaining tasks:" << m_taskList.size() - 1;

    setImageForDocSheet(task.sheet, task.pageIndex, pixmap);

    if (m_taskList.contains(task)) {
        QMetaObject::invokeMethod(task.receiver, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(int, task.pageIndex));
        m_taskList.removeAll(task);
    }
}

QPixmap ReaderImageThreadPoolManager::getImageForDocSheet(DocSheet *sheet, int pageIndex)
{
    if (m_docSheetImgMap.contains(sheet)) {
        qDebug() << "Image cache" << (m_docSheetImgMap[sheet][pageIndex].isNull() ? "miss" : "hit")
                 << "for page:" << pageIndex;
        return m_docSheetImgMap[sheet][pageIndex];
    }
    qDebug() << "No cache found for document";
    return QPixmap();
}

void ReaderImageThreadPoolManager::setImageForDocSheet(DocSheet *sheet, int pageIndex, const QPixmap &pixmap)
{
    if (pageIndex >= 0 && m_docSheetImgMap.contains(sheet) && m_docSheetImgMap[sheet].size() > pageIndex)
        m_docSheetImgMap[sheet][pageIndex] = pixmap;
}

void ReaderImageThreadPoolManager::onDocProxyDestroyed(QObject *obj)
{
    qDebug() << "Cleaning up resources for destroyed document proxy";
    m_docProxylst.removeAll(obj);
    m_docSheetImgMap.remove(obj);
}

void ReaderImageThreadPoolManager::onReceiverDestroyed(QObject *obj)
{
    qDebug() << "Cleaning up tasks for destroyed receiver";
    for (const ReaderImageParam_t &iter : m_taskList) {
        if (iter.receiver == obj) {
            qDebug() << "Removing task for page:" << iter.pageIndex;
            m_taskList.removeAll(iter);
            return;
        }
    }
}

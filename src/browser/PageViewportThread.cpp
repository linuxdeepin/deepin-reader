/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar Sheets)
*
* Sheet(SheetSidebar SheetBrowser document)
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
#include "PageViewportThread.h"
#include "BrowserPage.h"
#include "SheetBrowser.h"

#include <QTime>

QList<PageViewportThread *> PageViewportThread::instances;
bool PageViewportThread::quitForever = false;
PageViewportThread::PageViewportThread(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(BrowserPage *, QImage, double, QRect)), this, SLOT(onTaskFinished(BrowserPage *, QImage, double, QRect)), Qt::QueuedConnection);
}

PageViewportThread::~PageViewportThread()
{
    m_quit = true;
    wait();
}

void PageViewportThread::destroyForever()
{
    quitForever = true;

    QList<PageViewportThread *> instancesTemp = instances;
    instances.clear();

    foreach (PageViewportThread *instance, instancesTemp) {
        if (nullptr != instance) {
            delete instance;
        }
    }
}

void PageViewportThread::appendTask(RenderViewportTask task)
{
    if (quitForever)
        return;

    PageViewportThread *thread = nullptr;

    RenderViewportTaskPiece taskPiece0;
    taskPiece0.task = task;
    taskPiece0.task.renderRect = QRect(task.renderRect.x(), task.renderRect.y(), task.renderRect.width() / 2, task.renderRect.height() / 2);
    taskPiece0.pieceIndex = 0;
    thread = instance(0);
    if (nullptr == thread)
        return;
    thread->appendTaskPiece(taskPiece0);

    RenderViewportTaskPiece taskPiece1;
    taskPiece1.task = task;
    taskPiece1.task.renderRect = QRect(task.renderRect.x() + taskPiece0.task.renderRect.width(), task.renderRect.y(), task.renderRect.width() - taskPiece0.task.renderRect.width(), task.renderRect.height() / 2);
    taskPiece1.pieceIndex = 1;
    thread = instance(1);
    if (nullptr == thread)
        return;
    thread->appendTaskPiece(taskPiece1);

    RenderViewportTaskPiece taskPiece2;
    taskPiece2.task = task;
    taskPiece2.task.renderRect = QRect(task.renderRect.x(), task.renderRect.y() + taskPiece0.task.renderRect.height(), task.renderRect.width() / 2, task.renderRect.height() - taskPiece0.task.renderRect.height());
    taskPiece2.pieceIndex = 2;
    thread = instance(2);
    if (nullptr == thread)
        return;
    thread->appendTaskPiece(taskPiece2);

    RenderViewportTaskPiece taskPiece3;
    taskPiece3.task = task;
    taskPiece3.task.renderRect = QRect(task.renderRect.x() + taskPiece0.task.renderRect.width(), task.renderRect.y() + taskPiece0.task.renderRect.height(),
                                       task.renderRect.width() - taskPiece0.task.renderRect.width(), task.renderRect.height() - taskPiece0.task.renderRect.height());
    taskPiece3.pieceIndex = 3;
    thread = instance(3);
    if (nullptr == thread)
        return;

    thread->appendTaskPiece(taskPiece3);
}

void PageViewportThread::appendTaskPiece(RenderViewportTaskPiece taskPiece)
{
    if (m_curTaskPiece.task.page == taskPiece.task.page
            && m_curTaskPiece.task.renderRect == taskPiece.task.renderRect
            && m_curTaskPiece.task.rotation == taskPiece.task.rotation
            && qFuzzyCompare(m_curTaskPiece.task.scaleFactor, taskPiece.task.scaleFactor)) {
        return;
    }

    m_mutex.lock();

    for (int i = 0; i < m_taskPieces.count(); ++i) {
        if (m_taskPieces[i].task.page == taskPiece.task.page) {
            m_taskPieces.remove(i);
            i = 0;
        }
    }

    m_taskPieces.push(taskPiece);
    m_mutex.unlock();

    if (!isRunning())
        start();
}

void PageViewportThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_taskPieces.count() <= 0) {
            msleep(100);
            break;
        }

        m_mutex.lock();
        m_curTaskPiece = m_taskPieces.pop();
        m_mutex.unlock();

        if (BrowserPage::existInstance(m_curTaskPiece.task.page)) {
            QImage image = m_curTaskPiece.task.page->getImage(m_curTaskPiece.task.scaleFactor, m_curTaskPiece.task.rotation, m_curTaskPiece.task.renderRect);

            if (!image.isNull())
                emit sigTaskFinished(m_curTaskPiece.task.page, image, m_curTaskPiece.task.scaleFactor, m_curTaskPiece.task.renderRect);
        }

        m_mutex.lock();
        m_curTaskPiece = RenderViewportTaskPiece();
        m_mutex.unlock();
    }
}

void PageViewportThread::onTaskFinished(BrowserPage *page, QImage image, double scaleFactor, QRect rect)
{
    if (BrowserPage::existInstance(page) && !image.isNull() && qFuzzyCompare(scaleFactor, page->m_scaleFactor)) {
        page->handleViewportRenderFinished(scaleFactor, image, rect);
    }
}

PageViewportThread *PageViewportThread::instance(int threadIndex)
{
    if (quitForever)
        return nullptr;

    if (instances.count() <= 0) {
        for (int i = 0; i < 4; ++i) {
            PageViewportThread *instance = new PageViewportThread;
            instances.append(instance);
        }
    }

    return instances.value(threadIndex);
}


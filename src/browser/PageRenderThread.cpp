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
#include "PageRenderThread.h"
#include "BrowserPage.h"
#include "SheetBrowser.h"

#include <QTime>
#include <QDebug>

PageRenderThread *PageRenderThread::m_instance = nullptr;
PageRenderThread::PageRenderThread(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(BrowserPage *, QImage, double, int, QRect)), this, SLOT(onTaskFinished(BrowserPage *, QImage, double, int, QRect)), Qt::QueuedConnection);
}

PageRenderThread::~PageRenderThread()
{
    m_quit = true;
    wait();
}

void PageRenderThread::createInstance()
{
    if (nullptr == m_instance)
        m_instance = new PageRenderThread;
}

void PageRenderThread::destroyInstance()
{
    if (nullptr != m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

void PageRenderThread::appendTask(PageRenderTask task)
{
    if (nullptr == m_instance)
        return;

    if (m_instance->m_curTask.page == task.page
            && m_instance->m_curTask.renderRect == task.renderRect
            && m_instance->m_curTask.rotation == task.rotation
            && qFuzzyCompare(m_instance->m_curTask.scaleFactor, task.scaleFactor)) {
        return;
    }

    m_instance->m_mutex.lock();

    for (int i = 0; i < m_instance->m_tasks.count(); ++i) {
        if (m_instance->m_tasks[i].page == task.page)
            m_instance->m_tasks.remove(i);
    }

    m_instance->m_tasks.append(task);
    m_instance->m_mutex.unlock();

    if (!m_instance->isRunning())
        m_instance->start();
}

void PageRenderThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(100);
            break;
        }

        m_mutex.lock();
        m_curTask = m_tasks.pop();
        m_mutex.unlock();

        if (BrowserPage::existInstance(m_curTask.page)) {
            QImage image = m_curTask.page->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);
            if (!image.isNull())
                emit sigTaskFinished(m_curTask.page, image, m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);
        }

        m_mutex.lock();
        m_curTask = PageRenderTask();
        m_mutex.unlock();
    }
}

void PageRenderThread::onTaskFinished(BrowserPage *page, QImage image, double scaleFactor, int rotation, QRect rect)
{
    if (BrowserPage::existInstance(page) && !image.isNull() && qFuzzyCompare(scaleFactor, page->m_scaleFactor) && (rotation == page->m_rotation)) {
        page->setViewportFragment(rect, image);
    }
}


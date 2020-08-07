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
#include <QDebug>

PageViewportThread *PageViewportThread::m_instance = nullptr;
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
    if (nullptr != m_instance) {
        delete m_instance;
        quitForever = true;
        m_instance = nullptr;
    }
}

void PageViewportThread::appendTask(RenderViewportTask task)
{
    if (quitForever)
        return;

    if (nullptr == m_instance)
        m_instance = new PageViewportThread;

    if (m_instance->m_curTask.page == task.page
            && m_instance->m_curTask.renderRect == task.renderRect
            && m_instance->m_curTask.rotation == task.rotation
            && qFuzzyCompare(m_instance->m_curTask.scaleFactor, task.scaleFactor)) {
        return;
    }

    m_instance->m_mutex.lock();

    for (int i = 0; i < m_instance->m_tasks.count(); ++i) {
        if (m_instance->m_tasks[i].page == task.page) {
            m_instance->m_tasks.remove(i);
            i = 0;
        }
    }

    m_instance->m_tasks.push(task);
    m_instance->m_mutex.unlock();

    if (!m_instance->isRunning())
        m_instance->start();
}

int PageViewportThread::count(BrowserPage *page)
{
    if (nullptr == m_instance)
        return 0;

    int count = 0;

    foreach (RenderViewportTask task, m_instance->m_tasks) {
        if (task.page == page)
            count++;
    }

    return count;
}

void PageViewportThread::run()
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

            QTime time;
            time.start();
            QImage image = m_curTask.page->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);
            qDebug() << "Viewport:" << time.elapsed();

            if (!image.isNull())
                emit sigTaskFinished(m_curTask.page, image, m_curTask.scaleFactor, m_curTask.renderRect);
        }

        m_mutex.lock();
        m_curTask = RenderViewportTask();
        m_mutex.unlock();
    }
}

void PageViewportThread::onTaskFinished(BrowserPage *page, QImage image, double scaleFactor, QRect rect)
{
    if (BrowserPage::existInstance(page) && !image.isNull() && qFuzzyCompare(scaleFactor, page->m_scaleFactor)) {
        page->handleViewportRenderFinished(scaleFactor, image, rect);
    }
}


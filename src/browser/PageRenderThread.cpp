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
#include "PageViewportThread.h"

#include <QTime>
#include <QDebug>

PageRenderThread *PageRenderThread::instance = nullptr;
bool PageRenderThread::quitForever = false;
PageRenderThread::PageRenderThread(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(BrowserPage *, QImage, double, QRect)), this, SLOT(onTaskFinished(BrowserPage *, QImage, double, QRect)), Qt::QueuedConnection);
}

PageRenderThread::~PageRenderThread()
{
    m_quit = true;
    wait();
}

void PageRenderThread::clearTask(BrowserPage *item)
{
    if (quitForever)
        return;

    if (nullptr == instance)
        instance = new PageRenderThread;

    instance->m_mutex.lock();

    bool exist = true;
    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_tasks.count(); ++i) {
            if (instance->m_tasks[i].item == item) {
                instance->m_tasks.remove(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_mutex.unlock();
}

void PageRenderThread::clearTasks(SheetBrowser *view)
{
    if (quitForever)
        return;

    if (nullptr == instance)
        instance = new PageRenderThread;

    instance->m_mutex.lock();

    QStack<RenderPageTask> tasks;

    for (int i = 0; i < instance->m_tasks.count(); ++i) {
        if (instance->m_tasks[i].view != view) {
            tasks.append(instance->m_tasks[i]);
        }
    }

    instance->m_tasks = tasks;

    instance->m_mutex.unlock();
}

void PageRenderThread::appendTask(RenderPageTask task)
{
    if (quitForever)
        return;

    if (nullptr == instance)
        instance = new PageRenderThread;

    instance->m_mutex.lock();

    instance->m_tasks.push_back(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTasks(QList<RenderPageTask> list)
{
    if (quitForever)
        return;

    if (nullptr == instance)
        instance = new PageRenderThread;

    instance->m_mutex.lock();

    for (int i = list.count() - 1; i >= 0; --i)
        instance->m_tasks.push_back(list[i]);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(BrowserPage *item, double scaleFactor, Dr::Rotation rotation, QRect renderRect)
{
    if (quitForever)
        return;

    if (nullptr == instance)
        instance = new PageRenderThread;

    instance->m_mutex.lock();

    RenderPageTask task;
    task.item = item;
    task.scaleFactor = scaleFactor;
    task.rotation = rotation;
    task.renderRect = renderRect;
    instance->m_tasks.push(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(100);
            continue;
        }

        m_mutex.lock();

        m_curTask = m_tasks.pop();

        m_mutex.unlock();

        if (!BrowserPage::existInstance(m_curTask.item))
            continue;

        QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        if (!image.isNull())
            emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.renderRect);

        m_curTask = RenderPageTask();
    }
}

void PageRenderThread::destroyForever()
{
    if (nullptr != instance) {
        delete instance;
        quitForever = true;
        instance = nullptr;
    }
}

void PageRenderThread::onTaskFinished(BrowserPage *item, QImage image, double scaleFactor,  QRect rect)
{
    if (BrowserPage::existInstance(item)) {
        item->handleRenderFinished(scaleFactor, image, rect);
    }
}

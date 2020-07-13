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
#include "RenderPageThread.h"
#include "BrowserPage.h"
#include "SheetBrowser.h"

#include <QTime>
#include <QDebug>

RenderPageThread *RenderPageThread::instance = nullptr;
RenderPageThread::RenderPageThread(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(BrowserPage *, QImage, double, int, QRect, bool)), this, SLOT(onTaskFinished(BrowserPage *, QImage, double, int, QRect, bool)), Qt::QueuedConnection);
}

RenderPageThread::~RenderPageThread()
{
    instance = nullptr;
    m_quit = true;
    wait();
}

void RenderPageThread::clearTask(BrowserPage *item)
{
    if (nullptr == instance)
        instance = new RenderPageThread;

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

void RenderPageThread::clearTasks(SheetBrowser *view)
{
    if (nullptr == instance)
        instance = new RenderPageThread;

    instance->m_mutex.lock();

    QStack<RenderTask> tasks;

    for (int i = 0; i < instance->m_tasks.count(); ++i) {
        if (instance->m_tasks[i].view != view) {
            tasks.append(instance->m_tasks[i]);
        }
    }

    instance->m_tasks = tasks;

    instance->m_mutex.unlock();
}

void RenderPageThread::appendTask(RenderTask task)
{
    if (nullptr == instance)
        instance = new RenderPageThread;

    instance->m_mutex.lock();

    instance->m_tasks.append(task);

    instance->m_mutex.unlock();
}

void RenderPageThread::appendTasks(QList<RenderTask> list)
{
    if (nullptr == instance)
        instance = new RenderPageThread;

    instance->m_mutex.lock();

    for (int i = list.count() - 1; i >= 0; i--)
        instance->m_tasks.append(list[i]);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void RenderPageThread::appendTask(BrowserPage *item, double scaleFactor, Dr::Rotation rotation, QRect renderRect)
{
    if (nullptr == instance)
        instance = new RenderPageThread;

    instance->m_mutex.lock();

    RenderTask task;
    task.item = item;
    task.scaleFactor = scaleFactor;
    task.rotation = rotation;
    task.renderRect = renderRect;
    instance->m_tasks.append(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void RenderPageThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(10);
            break;
        }

        m_mutex.lock();
        m_curTask = m_tasks.pop();
        m_mutex.unlock();

        if (!BrowserPage::existInstance(m_curTask.item))
            continue;

        QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        if (!image.isNull())
            emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect, m_curTask.preRender);

        m_curTask = RenderTask();
    }
}

void RenderPageThread::destroy()
{
    if (nullptr != instance) {
        delete instance;
        instance = nullptr;
    }
}

void RenderPageThread::onTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect, bool preRender)
{
    if (BrowserPage::existInstance(item)) {
        if (preRender)
            item->handlePreRenderFinished(static_cast<Dr::Rotation>(rotation), image);
        else
            item->handleRenderFinished(scaleFactor, static_cast<Dr::Rotation>(rotation), image, rect);
    }
}

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
#include "BrowserRenderThread.h"
#include "BrowserPage.h"
#include "SheetBrowser.h"

#include <QTime>
#include <QDebug>

BrowserRenderThread *BrowserRenderThread::instance = nullptr;
BrowserRenderThread::BrowserRenderThread(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(BrowserPage *, QImage, double, int, QRect)), this, SLOT(onTaskFinished(BrowserPage *, QImage, double, int, QRect)), Qt::QueuedConnection);
}

BrowserRenderThread::~BrowserRenderThread()
{
    instance = nullptr;
    m_quit = true;
    wait();
}

void BrowserRenderThread::clearVipTask(BrowserPage *item)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    bool exist = true;
    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_vipTasks.count(); ++i) {
            if (instance->m_vipTasks[i].item == item) {
                instance->m_vipTasks.remove(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_mutex.unlock();
}

void BrowserRenderThread::clearTask(BrowserPage *item)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

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

void BrowserRenderThread::clearTasks(SheetBrowser *view)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    QStack<RenderTaskPDFL> tasks;

    for (int i = 0; i < instance->m_tasks.count(); ++i) {
        if (instance->m_tasks[i].view != view) {
            tasks.append(instance->m_tasks[i]);
        }
    }

    instance->m_tasks = tasks;

    instance->m_mutex.unlock();
}

void BrowserRenderThread::appendVipTask(RenderTaskPDFL task)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    instance->m_vipTasks.append(task);

    instance->m_mutex.unlock();
}

void BrowserRenderThread::appendTask(RenderTaskPDFL task)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    instance->m_tasks.append(task);

    instance->m_mutex.unlock();
}

void BrowserRenderThread::appendTasks(QList<RenderTaskPDFL> list)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    for (int i = list.count() - 1; i >= 0; i--)
        instance->m_tasks.append(list[i]);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void BrowserRenderThread::appendTask(BrowserPage *item, double scaleFactor, Dr::Rotation rotation, QRect renderRect)
{
    if (nullptr == instance)
        instance = new BrowserRenderThread;

    instance->m_mutex.lock();

    RenderTaskPDFL task;
    task.item = item;
    task.scaleFactor = scaleFactor;
    task.rotation = rotation;
    task.renderRect = renderRect;
    instance->m_tasks.append(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void BrowserRenderThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(10);
            break;
        }

        m_mutex.lock();

        if (m_vipTasks.isEmpty())
            m_curTask = m_tasks.pop();
        else {
            m_curTask = m_vipTasks.pop();
            m_vipTasks.clear();
        }

        m_mutex.unlock();

        if (!BrowserPage::existInstance(m_curTask.item))
            continue;

        QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        if (!image.isNull())
            emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        m_curTask = RenderTaskPDFL();
    }
}

void BrowserRenderThread::destroy()
{
    if (nullptr != instance) {
        delete instance;
        instance = nullptr;
    }
}

void BrowserRenderThread::onTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect)
{
    if (BrowserPage::existInstance(item))
        item->handleRenderFinished(scaleFactor, static_cast<Dr::Rotation>(rotation), image, rect);
}

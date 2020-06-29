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
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#include "RenderThreadPDFL.h"
#include "SheetBrowserPDFLItem.h"

#include <QTime>
#include <QDebug>

RenderThreadPDFL *RenderThreadPDFL::instance = nullptr;
RenderThreadPDFL::RenderThreadPDFL(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(SheetBrowserPDFLItem *, QImage, double, int, QRect)), this, SLOT(onTaskFinished(SheetBrowserPDFLItem *, QImage, double, int, QRect)), Qt::QueuedConnection);
}

RenderThreadPDFL::~RenderThreadPDFL()
{
    instance = nullptr;
    m_quit = true;
    wait();
}

void RenderThreadPDFL::clearVipTask(SheetBrowserPDFLItem *item)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

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

void RenderThreadPDFL::clearTask(SheetBrowserPDFLItem *item)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

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

void RenderThreadPDFL::appendVipTask(RenderTaskPDFL task)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

    instance->m_mutex.lock();

    instance->m_vipTasks.append(task);

    instance->m_mutex.unlock();
}

void RenderThreadPDFL::appendTask(RenderTaskPDFL task)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

    instance->m_mutex.lock();

    instance->m_tasks.append(task);

    instance->m_mutex.unlock();
}

void RenderThreadPDFL::appendTasks(QList<RenderTaskPDFL> list)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

    instance->m_mutex.lock();

    for (int i = list.count() - 1; i >= 0; i--)
        instance->m_tasks.append(list[i]);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void RenderThreadPDFL::appendTask(SheetBrowserPDFLItem *item, double scaleFactor, Dr::Rotation rotation, QRect renderRect)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

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

void RenderThreadPDFL::run()
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

        if (!SheetBrowserPDFLItem::existInstance(m_curTask.item))
            continue;

        QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        if (!image.isNull())
            emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation, m_curTask.renderRect);

        m_curTask = RenderTaskPDFL();
    }
}

void RenderThreadPDFL::destroy()
{
    if (nullptr != instance)
        delete instance;
}

void RenderThreadPDFL::onTaskFinished(SheetBrowserPDFLItem *item, QImage image, double scaleFactor, int rotation, QRect rect)
{
    if (SheetBrowserPDFLItem::existInstance(item))
        item->handleRenderFinished(scaleFactor, (Dr::Rotation)rotation, image, rect);
}

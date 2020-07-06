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
#include "RenderThreadDJVU.h"
#include "SheetBrowserDJVUItem.h"

#include <QTime>
#include <QDebug>

RenderThreadDJVU *RenderThreadDJVU::instance = nullptr;
RenderThreadDJVU::RenderThreadDJVU(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(SheetBrowserDJVUItem *, QImage, double, int)), this, SLOT(onTaskFinished(SheetBrowserDJVUItem *, QImage, double, int)), Qt::QueuedConnection);
}

RenderThreadDJVU::~RenderThreadDJVU()
{
    instance = nullptr;
    m_quit = true;
    wait();
}

void RenderThreadDJVU::appendTask(SheetBrowserDJVUItem *item, double scaleFactor, Dr::Rotation rotation)
{
    if (nullptr == instance)
        instance = new RenderThreadDJVU;

    if (instance->m_curTask.item == item && instance->m_curTask.scaleFactor == scaleFactor && instance->m_curTask.rotation == rotation)
        return;

    instance->m_mutex.lock();

    bool exist = false;
    for (int i = 0; i < instance->m_tasks.count(); ++i) {
        if (instance->m_tasks[i].item == item) {
            instance->m_tasks[i].scaleFactor = scaleFactor;
            instance->m_tasks[i].rotation = rotation;
            exist = true;
            break;
        }
    }

    if (!exist) {
        RenderTaskDJVU task;
        task.item = item;
        task.scaleFactor = scaleFactor;
        task.rotation = rotation;
        instance->m_tasks.append(task);
    }

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void RenderThreadDJVU::run()
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

        if (!SheetBrowserDJVUItem::existInstance(m_curTask.item))
            continue;

        QTime time;
        time.start();

        QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation);

        qDebug() << time.elapsed() / 1000.0 << "s";

        if (!image.isNull())
            emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation);
    }
}

void RenderThreadDJVU::destroy()
{
    if (nullptr != instance) {
        delete instance;
        instance = nullptr;
    }
}

void RenderThreadDJVU::onTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation)
{
    if (SheetBrowserDJVUItem::existInstance(item))
        item->handleRenderFinished(scaleFactor, (Dr::Rotation)rotation, image);
}

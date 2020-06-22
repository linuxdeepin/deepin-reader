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

void RenderThreadPDFL::appendTask(SheetBrowserPDFLItem *item, double scaleFactor, Dr::Rotation rotation)
{
    if (nullptr == instance)
        instance = new RenderThreadPDFL;

    if (instance->m_curTask.item == item && instance->m_curTask.scaleFactor == scaleFactor && instance->m_curTask.rotation == rotation)
        return;

    if (instance->m_curTask.item == item && (instance->m_curTask.scaleFactor != scaleFactor || instance->m_curTask.rotation != rotation)) {
        instance->m_quitRender = true;
    }

    instance->m_mutex.lock();

    RenderTaskPDFL task;
    task.item = item;
    task.scaleFactor = scaleFactor;
    task.rotation = rotation;
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

        m_curTask = m_tasks.pop();

        m_mutex.unlock();

        if (!SheetBrowserPDFLItem::existInstance(m_curTask.item))
            continue;

        QTime time;

        time.start();

        QRectF rect = m_curTask.item->boundingRect();
        if (rect.height() > 10000) {
            int y = 0;
            for (int i = 0 ; i * 400 < rect.height(); ++i) {
                if (m_quitRender) {
                    m_quitRender = false;
                    break;
                }

                int height = 400;
                if (rect.height() < i * 400)
                    height = rect.height() - 400 * i;

                QRect renderRect = QRect(0, y, m_curTask.item->boundingRect().width(), height);

                QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, renderRect);

                y += height;

                if (!image.isNull())
                    emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation, renderRect);
            }
        } else {
            QRect renderRect(m_curTask.item->boundingRect().x(), m_curTask.item->boundingRect().y(), (int)m_curTask.item->boundingRect().width(), (int)m_curTask.item->boundingRect().height());

            QImage image = m_curTask.item->getImage(m_curTask.scaleFactor, m_curTask.rotation, renderRect);

            qDebug() << time.elapsed() / 1000.0 << "s";

            if (!image.isNull())
                emit sigTaskFinished(m_curTask.item, image, m_curTask.scaleFactor, m_curTask.rotation, image.rect());
        }

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

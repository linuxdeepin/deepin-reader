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

    m_instance->m_mutex.lock();
    if (m_instance->m_curTask.page == task.page
            && m_instance->m_curTask.renderRect == task.renderRect
            && m_instance->m_curTask.rotation == task.rotation
            && qFuzzyCompare(m_instance->m_curTask.scaleFactor, task.scaleFactor))
        return;

    m_instance->m_tasks.clear();
    m_instance->m_tasks.append(task);
    m_instance->m_mutex.unlock();

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
            if (!image.isNull() && qFuzzyCompare(m_curTask.scaleFactor, m_curTask.page->m_scaleFactor) && m_curTask.rotation != m_curTask.page->m_rotation) {
                QPainter painter(&m_curTask.page->m_image);
                painter.drawImage(m_curTask.renderRect, image);
                m_curTask.page->update();
            }
        }

        m_mutex.lock();
        m_curTask = PageRenderTask();
        m_mutex.unlock();
    }
}


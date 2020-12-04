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

QList<PageRenderThread *> PageRenderThread::instances;
bool PageRenderThread::quitForever = false;
PageRenderThread::PageRenderThread(QObject *parent) : QThread(parent)
{
    m_delayTimer = new QTimer(this);
    m_delayTimer->setSingleShot(true);
    connect(m_delayTimer, &QTimer::timeout, this, &PageRenderThread::onDelayTaskTimeout);
    connect(this, SIGNAL(sigImageTaskFinished(BrowserPage *, QPixmap, int, QRect)), this, SLOT(onImageTaskFinished(BrowserPage *, QPixmap, int, QRect)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigWordTaskFinished(BrowserPage *, QList<deepin_reader::Word>)), this, SLOT(onWordTaskFinished(BrowserPage *, QList<deepin_reader::Word>)), Qt::QueuedConnection);
}

PageRenderThread::~PageRenderThread()
{
    m_quit = true;
    wait();
}

bool PageRenderThread::clearImageTask(BrowserPage *item, int pixmapId)
{
    if (nullptr == item)
        return true;

    PageRenderThread *instance  = PageRenderThread::instance(item->itemIndex());

    if (nullptr == instance) {
        return false;
    }

    instance->m_mutex.lock();

    bool exist = true;
    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_tasks.count(); ++i) {
            if (instance->m_tasks[i].type == RenderPageTask::word)
                continue;

            if (instance->m_tasks[i].page == item && (instance->m_tasks[i].pixmapId != pixmapId || -1 == pixmapId) && instance->m_tasks[i].rect.isEmpty()) {
                instance->m_tasks.remove(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_mutex.unlock();

    return true;
}

void PageRenderThread::appendTask(RenderPageTask task)
{
    if (nullptr == task.page)
        return;

    PageRenderThread *instance  = PageRenderThread::instance(task.page->itemIndex());

    if (nullptr == instance) {
        return;
    }

    instance->m_mutex.lock();

    if (task.type == RenderPageTask::word)
        qDebug() << "append:" << task.page->itemIndex();

    instance->m_tasks.push_back(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendDelayTask(RenderPageTask task)
{
    if (nullptr == task.page)
        return;

    PageRenderThread *instance  = PageRenderThread::instance(task.page->itemIndex());
    if (nullptr == instance) {
        return;
    }

    instance->m_delayTask = task;

    if (instance->m_delayTimer->isActive())
        instance->m_delayTimer->stop();

    instance->m_delayTimer->start(300);
}

void PageRenderThread::appendTask(BrowserPage *item, double scaleFactor, int pixmapId, QRect renderRect)
{
    if (nullptr == item)
        return;

    PageRenderThread *instance  = PageRenderThread::instance(item->itemIndex());
    if (nullptr == instance) {
        return;
    }

    instance->m_mutex.lock();

    RenderPageTask task;
    task.page = item;
    task.scaleFactor = scaleFactor;
    task.pixmapId = pixmapId;
    task.rect = renderRect;
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
        if (m_curTask.type == RenderPageTask::word)
            qDebug() << "pop:" << m_curTask.page->itemIndex();
        m_mutex.unlock();

        if (!BrowserPage::existInstance(m_curTask.page))
            continue;

        if (RenderPageTask::Image == m_curTask.type) {
            QImage image;

            if (m_curTask.rect.isEmpty())
                image = m_curTask.page->getImage(m_curTask.scaleFactor);
            else
                image = m_curTask.page->getImage(m_curTask.scaleFactor, m_curTask.rect);

            if (!image.isNull())
                emit sigImageTaskFinished(m_curTask.page, QPixmap::fromImage(image), m_curTask.pixmapId, m_curTask.rect);

        } else if (RenderPageTask::word == m_curTask.type) {
            QList<Word> words = m_curTask.page->getWords();
            if (words.count() > 0)
                emit sigWordTaskFinished(m_curTask.page, words);
        }

        m_curTask = RenderPageTask();
    }
}

void PageRenderThread::destroyForever()
{
    quitForever = true;

    QList<PageRenderThread *> instancesTemp = instances;
    instances.clear();

    foreach (PageRenderThread *instance, instancesTemp) {
        if (nullptr != instance) {
            delete instance;
        }
    }
}

void PageRenderThread::onImageTaskFinished(BrowserPage *item, QPixmap pixmap, int pixmapId,  QRect rect)
{
    if (BrowserPage::existInstance(item)) {
        item->handleRenderFinished(pixmapId, pixmap, rect);
    }
}

void PageRenderThread::onWordTaskFinished(BrowserPage *item, QList<deepin_reader::Word> words)
{
    qDebug() << "onWordTaskFinished:" << item->itemIndex();
    if (BrowserPage::existInstance(item)) {
        item->handleWordLoaded(words);
    }
}

void PageRenderThread::onDelayTaskTimeout()
{
    m_mutex.lock();

    m_tasks.push_back(m_delayTask);

    m_mutex.unlock();

    if (!isRunning())
        start();
}

PageRenderThread *PageRenderThread::instance(int itemIndex)
{
    if (quitForever)
        return nullptr;

    if (instances.count() <= 0) {
        qRegisterMetaType<QList<deepin_reader::Word>>("QList<deepin_reader::Word>");
        for (int i = 0; i < 4; ++i) {
            PageRenderThread *instance = new PageRenderThread;
            instances.append(instance);
        }
    }

    static int threadCounter = 0;

    int threadIndex = threadCounter++ % 4;

    if (itemIndex != -1)
        threadIndex = itemIndex % 4;

    if (threadCounter > 1000)
        threadCounter = 0;

    return instances.value(threadIndex);
}

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

PageRenderThread *PageRenderThread::s_instance = nullptr;   //由于pdfium不支持多线程，暂时单线程进行

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

    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return false;
    }

    instance->m_mutex.lock();

    bool exist = true;

    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_tasks.count(); ++i) {
            if (instance->m_tasks[i].type == RenderPageTask::Word || instance->m_tasks[i].type == RenderPageTask::ImageSlice)
                continue;

            if (instance->m_tasks[i].page == item && (instance->m_tasks[i].pixmapId != pixmapId || -1 == pixmapId) && instance->m_tasks[i].whole.isEmpty()) {
                instance->m_tasks.removeAt(i);
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

    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_mutex.lock();

    instance->m_tasks.append(task);

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendDelayTask(RenderPageTask task)
{
    if (nullptr == task.page)
        return;

    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_delayTask = task;

    if (instance->m_delayTimer->isActive())
        instance->m_delayTimer->stop();

    instance->m_delayTimer->start(300);
}

void PageRenderThread::appendTask(BrowserPage *page, int pixmapId, QRect whole, QRect slice)
{
    if (nullptr == page)
        return;

    PageRenderThread *instance  = PageRenderThread::instance();
    if (nullptr == instance) {
        return;
    }

    instance->m_mutex.lock();

    RenderPageTask task;
    task.page = page;
    task.whole = whole;
    task.pixmapId = pixmapId;
    task.slice = slice;
    instance->m_tasks.append(task);

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

        if (execNextImageTask())
            continue;

        if (execNextImageSliceTask())
            continue;

        if (execNextWordTask())
            continue;

        RenderPageTask task;

        if (!getNextTask(RenderPageTask::BigImage, task))
            continue;

        QList<QRect> renderRects;

        int wCount = task.whole.width() % 1000 == 0 ? (task.whole.width() / 1000) : (task.whole.width() / 1000 + 1);

        for (int i = 0; i < wCount; ++i) {//只能以宽度前进(即只能分割宽度)，如果x从0开始，每次都将消耗一定时间
            renderRects.append(QRect(i * 1000, 0, 1000, task.whole.height()));
        };

        QPixmap pixmap = QPixmap(task.whole.width(), task.whole.height());

        QPainter painter(&pixmap);

        for (QRect rect : renderRects) {
            if (m_quit)
                break;

            //外部删除了此处不判断会导致崩溃
            if (!BrowserPage::existInstance(task.page))
                break;

            //判断page存在之后 使用page之前，也就是此处，如果主线程先一步进入page被删流程，【理论上会导致崩溃】，目前概率非常低，未发现
            QImage image = task.page->getImage(task.whole.width(), task.whole.height(),
                                               QRect(static_cast<int>(rect.x()),
                                                     static_cast<int>(rect.y()),
                                                     static_cast<int>(rect.width()),
                                                     static_cast<int>(rect.height())));

            painter.drawImage(rect, image);

            //优先进行其他图片加载
            while (execNextImageTask())
            {}

            //优先进行其他图片部分加载
            while (execNextImageSliceTask())
            {}

            //优先进行其他文字加载
            while (execNextWordTask())
            {}
        }

        if (m_quit)
            break;

        emit sigImageTaskFinished(task.page, pixmap, task.pixmapId, QRect());
    }
}

bool PageRenderThread::execNextImageTask()
{
    if (m_quit)
        return false;

    RenderPageTask task;

    if (!getNextTask(RenderPageTask::Image, task))
        return false;

    //外部删除了此处不判断会导致崩溃
    if (!BrowserPage::existInstance(task.page))
        return false;;

    QImage image = task.page->getImage(task.whole.width(), task.whole.height());

    if (!image.isNull())
        emit sigImageTaskFinished(task.page, QPixmap::fromImage(image), task.pixmapId, QRect());

    return true;
}

bool PageRenderThread::execNextImageSliceTask()
{
    if (m_quit)
        return false;

    RenderPageTask task;

    if (!getNextTask(RenderPageTask::ImageSlice, task))
        return false;

    //外部删除了此处不判断会导致崩溃
    if (!BrowserPage::existInstance(task.page))
        return false;;

    QImage image = task.page->getImage(task.whole.width(), task.whole.height(), task.slice);

    if (!image.isNull())
        emit sigImageTaskFinished(task.page, QPixmap::fromImage(image), task.pixmapId, task.slice);

    return true;
}

bool PageRenderThread::execNextWordTask()
{
    if (m_quit)
        return false;

    RenderPageTask task;

    if (!getNextTask(RenderPageTask::Word, task))
        return false;

    //外部删除了此处不判断会导致崩溃
    if (!BrowserPage::existInstance(task.page))
        return false;;

    const QList<Word> &words = task.page->getWords();

    emit sigWordTaskFinished(task.page, words);

    return true;
}

bool PageRenderThread::getNextTask(RenderPageTask::RenderPageTaskType type, RenderPageTask &task)
{
    m_mutex.lock();

    for (int i = 0; i < m_tasks.count(); ++i) {
        if (type == m_tasks[i].type) {
            task = m_tasks[i];
            m_tasks.removeAt(i);
            m_mutex.unlock();
            return true;
        }
    }

    m_mutex.unlock();

    return false;
}

void PageRenderThread::destroyForever()
{
    quitForever = true;

    if (nullptr != s_instance)
        delete s_instance;
}

void PageRenderThread::onImageTaskFinished(BrowserPage *item, QPixmap pixmap, int pixmapId, QRect slice)
{
    if (BrowserPage::existInstance(item)) {
        item->handleRenderFinished(pixmapId, pixmap, slice);
    }
}

void PageRenderThread::onWordTaskFinished(BrowserPage *item, QList<deepin_reader::Word> words)
{
    if (BrowserPage::existInstance(item)) {
        item->handleWordLoaded(words);
    }
}

void PageRenderThread::onDelayTaskTimeout()
{
    m_mutex.lock();

    m_tasks.append(m_delayTask);

    m_mutex.unlock();

    if (!isRunning())
        start();
}

PageRenderThread *PageRenderThread::instance()
{
    if (quitForever)
        return nullptr;

    if (nullptr == s_instance) {
        qRegisterMetaType<QList<deepin_reader::Word>>("QList<deepin_reader::Word>");
        s_instance = new PageRenderThread;
    }

    return s_instance;
}

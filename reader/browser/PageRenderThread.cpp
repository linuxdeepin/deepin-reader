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
#include "DocSheet.h"
#include "SheetRenderer.h"
#include "SideBarImageViewModel.h"

#include <QTime>
#include <QDebug>
#include <QMetaType>
#include <QFileInfo>

PageRenderThread *PageRenderThread::s_instance = nullptr;   //由于pdfium不支持多线程，暂时单线程进行

bool PageRenderThread::s_quitForever = false;

PageRenderThread::PageRenderThread(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<deepin_reader::Document *>("deepin_reader::Document *");
    qRegisterMetaType<QList<deepin_reader::Word>>("QList<deepin_reader::Word>");
    qRegisterMetaType<QList<deepin_reader::Annotation *>>("QList<deepin_reader::Annotation *>");
    qRegisterMetaType<QList<deepin_reader::Page *>>("QList<deepin_reader::Page *>");
    qRegisterMetaType<deepin_reader::Document::Error>("deepin_reader::Document::Error");

    qRegisterMetaType<DocPageNormalImageTask>("DocPageNormalImageTask");
    qRegisterMetaType<DocPageSliceImageTask>("DocPageSliceImageTask");
    qRegisterMetaType<DocPageBigImageTask>("DocPageBigImageTask");
    qRegisterMetaType<DocPageWordTask>("DocPageWordTask");
    qRegisterMetaType<DocPageAnnotationTask>("DocPageAnnotationTask");
    qRegisterMetaType<DocPageThumbnailTask>("DocPageThumbnailTask");
    qRegisterMetaType<DocOpenTask>("DocOpenTask");

    connect(this, &PageRenderThread::sigDocPageNormalImageTaskFinished, this, &PageRenderThread::onDocPageNormalImageTaskFinished, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocPageSliceImageTaskFinished, this, &PageRenderThread::onDocPageSliceImageTaskFinished, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocPageBigImageTaskFinished, this, &PageRenderThread::onDocPageBigImageTaskFinished, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocPageWordTaskFinished, this, &PageRenderThread::onDocPageWordTaskFinished, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocPageAnnotationTaskFinished, this, &PageRenderThread::onDocPageAnnotationTaskFinished, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocPageThumbnailTaskFinished, this, &PageRenderThread::onDocPageThumbnailTask, Qt::QueuedConnection);
    connect(this, &PageRenderThread::sigDocOpenTask, this, &PageRenderThread::onDocOpenTask, Qt::QueuedConnection);
}

PageRenderThread::~PageRenderThread()
{
    m_quit = true;
    wait();
}

bool PageRenderThread::clearImageTasks(DocSheet *sheet, BrowserPage *page, int pixmapId)
{
    if (nullptr == page)
        return true;

    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return false;
    }

    instance->m_pageNormalImageMutex.lock();

    bool exist = true;

    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_pageNormalImageTasks.count(); ++i) {
            if (instance->m_pageNormalImageTasks[i].page == page &&
                    instance->m_pageNormalImageTasks[i].sheet == sheet &&
                    (instance->m_pageNormalImageTasks[i].pixmapId != pixmapId || -1 == pixmapId)) {
                instance->m_pageNormalImageTasks.removeAt(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_pageNormalImageMutex.unlock();

    instance->m_pageSliceImageMutex.lock();

    exist = true;

    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_pageSliceImageTasks.count(); ++i) {
            if (instance->m_pageSliceImageTasks[i].page == page &&
                    instance->m_pageSliceImageTasks[i].sheet == sheet &&
                    (instance->m_pageSliceImageTasks[i].pixmapId != pixmapId || -1 == pixmapId)) {
                instance->m_pageSliceImageTasks.removeAt(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_pageSliceImageMutex.unlock();

    instance->m_pageBigImageMutex.lock();

    exist = true;

    while (exist) {
        exist = false;
        for (int i = 0; i < instance->m_pageBigImageTasks.count(); ++i) {
            if (instance->m_pageBigImageTasks[i].page == page &&
                    instance->m_pageBigImageTasks[i].sheet == sheet &&
                    (instance->m_pageBigImageTasks[i].pixmapId != pixmapId || -1 == pixmapId)) {
                instance->m_pageBigImageTasks.removeAt(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_pageBigImageMutex.unlock();

    return true;
}

void PageRenderThread::appendTask(DocPageNormalImageTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageNormalImageMutex.lock();

    instance->m_pageNormalImageTasks.append(task);

    instance->m_pageNormalImageMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocPageSliceImageTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageSliceImageMutex.lock();

    instance->m_pageSliceImageTasks.append(task);

    instance->m_pageSliceImageMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocPageBigImageTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageBigImageMutex.lock();

    instance->m_pageBigImageTasks.append(task);

    instance->m_pageBigImageMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocPageWordTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageWordMutex.lock();

    instance->m_pageWordTasks.append(task);

    instance->m_pageWordMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocPageAnnotationTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageAnnotationMutex.lock();

    instance->m_pageAnnotationTasks.append(task);

    instance->m_pageAnnotationMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocPageThumbnailTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_pageThumbnailMutex.lock();

    instance->m_pageThumbnailTasks.append(task);

    instance->m_pageThumbnailMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocOpenTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_openMutex.lock();

    instance->m_openTasks.append(task);

    instance->m_openMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::appendTask(DocCloseTask task)
{
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        return;
    }

    instance->m_closeMutex.lock();

    instance->m_closeTasks.append(task);

    instance->m_closeMutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void PageRenderThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (!hasNextTask()) {
            msleep(100);
            continue;
        }

        //先完成所有的关闭任务再进行打开
        while (execNextDocCloseTask())
        {}

        while (execNextDocOpenTask())
        {}

        while (execNextDocPageSliceImageTask())
        {}

        while (execNextDocPageNormalImageTask())
        {}

        while (execNextDocPageWordTask())
        {}

        while (execNextDocPageAnnotationTask())
        {}

        while (execNextDocPageThumbnailTask())
        {}

        DocPageBigImageTask task;

        if (!popNextDocPageBigImageTask(task))
            continue;

        QList<QRect> renderRects;

        int wCount = task.rect.width() % 1000 == 0 ? (task.rect.width() / 1000) : (task.rect.width() / 1000 + 1);

        for (int i = 0; i < wCount; ++i) {//只能以宽度前进(即只能分割宽度)，如果x从0开始，每次都将消耗一定时间
            renderRects.append(QRect(i * 1000, 0, 1000, task.rect.height()));
        };

        QPixmap pixmap = QPixmap(task.rect.width(), task.rect.height());

        QPainter painter(&pixmap);

        for (QRect rect : renderRects) {
            if (m_quit)
                break;

            //外部删除了此处不判断会导致崩溃
            if (!DocSheet::existSheet(task.sheet))
                break;

            //判断page存在之后 使用page之前，也就是此处，如果主线程先一步进入page被删流程，【理论上会导致崩溃】，目前概率非常低，未发现
            QImage image = task.sheet->getImage(task.page->itemIndex(), task.rect.width(), task.rect.height(),
                                                QRect(static_cast<int>(rect.x()),
                                                      static_cast<int>(rect.y()),
                                                      static_cast<int>(rect.width()),
                                                      static_cast<int>(rect.height())));

            painter.drawImage(rect, image);

            //先完成所有的关闭任务再进行打开
            while (execNextDocCloseTask())
            {}

            while (execNextDocOpenTask())
            {}

            while (execNextDocPageSliceImageTask())
            {}

            while (execNextDocPageNormalImageTask())
            {}

            while (execNextDocPageWordTask())
            {}

            while (execNextDocPageAnnotationTask())
            {}

            while (execNextDocPageThumbnailTask())
            {}
        }

        if (m_quit)
            break;

        emit sigDocPageBigImageTaskFinished(task, pixmap);
    }
}

bool PageRenderThread::hasNextTask()
{
    QMutexLocker pageNormalImageLocker(&m_pageNormalImageMutex);
    QMutexLocker pageSliceImageLocker(&m_pageSliceImageMutex);
    QMutexLocker pageBigImageLocker(&m_pageBigImageMutex);
    QMutexLocker pageWordLocker(&m_pageWordMutex);
    QMutexLocker pageThumbnailLocker(&m_pageThumbnailMutex);
    QMutexLocker pageOpenLocker(&m_openMutex);

    return !m_pageNormalImageTasks.isEmpty() || !m_pageSliceImageTasks.isEmpty() ||
           !m_pageBigImageTasks.isEmpty() || !m_pageWordTasks.isEmpty() || !m_pageThumbnailTasks.isEmpty() || !m_openTasks.isEmpty();
}

bool PageRenderThread::popNextDocPageNormalImageTask(DocPageNormalImageTask &task)
{
    QMutexLocker locker(&m_pageNormalImageMutex);

    if (m_pageNormalImageTasks.count() <= 0)
        return false;

    task = m_pageNormalImageTasks.value(0);

    m_pageNormalImageTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocPageSliceImageTask(DocPageSliceImageTask &task)
{
    QMutexLocker locker(&m_pageSliceImageMutex);

    if (m_pageSliceImageTasks.count() <= 0)
        return false;

    task = m_pageSliceImageTasks.value(0);

    m_pageSliceImageTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocPageBigImageTask(DocPageBigImageTask &task)
{
    QMutexLocker locker(&m_pageBigImageMutex);

    if (m_pageBigImageTasks.count() <= 0)
        return false;

    task = m_pageBigImageTasks.value(0);

    m_pageBigImageTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocPageWordTask(DocPageWordTask &task)
{
    QMutexLocker locker(&m_pageWordMutex);

    if (m_pageWordTasks.count() <= 0)
        return false;

    task = m_pageWordTasks.value(0);

    m_pageWordTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocPageAnnotationTask(DocPageAnnotationTask &task)
{
    QMutexLocker locker(&m_pageAnnotationMutex);

    if (m_pageAnnotationTasks.count() <= 0)
        return false;

    task = m_pageAnnotationTasks.value(0);

    m_pageAnnotationTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocPageThumbnailTask(DocPageThumbnailTask &task)
{
    QMutexLocker locker(&m_pageThumbnailMutex);

    if (m_pageThumbnailTasks.count() <= 0)
        return false;

    task = m_pageThumbnailTasks.value(0);

    m_pageThumbnailTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocOpenTask(DocOpenTask &task)
{
    QMutexLocker locker(&m_openMutex);

    if (m_openTasks.count() <= 0)
        return false;

    task = m_openTasks.value(0);

    m_openTasks.removeAt(0);

    return true;
}

bool PageRenderThread::popNextDocCloseTask(DocCloseTask &task)
{
    QMutexLocker locker(&m_closeMutex);

    if (m_closeTasks.count() <= 0)
        return false;

    task = m_closeTasks.value(0);

    m_closeTasks.removeAt(0);

    return true;
}

bool PageRenderThread::execNextDocPageNormalImageTask()
{
    if (m_quit)
        return false;

    DocPageNormalImageTask task;

    if (!popNextDocPageNormalImageTask(task))
        return false;

    if (!DocSheet::existSheet(task.sheet))
        return true;

    QImage image = task.sheet->getImage(task.page->itemIndex(), task.rect.width(), task.rect.height());

    if (!image.isNull())
        emit sigDocPageNormalImageTaskFinished(task, QPixmap::fromImage(image));

    return true;
}

bool PageRenderThread::execNextDocPageSliceImageTask()
{
    if (m_quit)
        return false;

    DocPageSliceImageTask task;

    if (!popNextDocPageSliceImageTask(task))
        return false;

    if (!DocSheet::existSheet(task.sheet))
        return true;

    QImage image = task.sheet->getImage(task.page->itemIndex(), task.whole.width(), task.whole.height(), task.slice);

    if (!image.isNull())
        emit sigDocPageSliceImageTaskFinished(task, QPixmap::fromImage(image));

    return true;
}

bool PageRenderThread::execNextDocPageWordTask()
{
    if (m_quit)
        return false;

    DocPageWordTask task;

    if (!popNextDocPageWordTask(task))
        return false;

    if (!DocSheet::existSheet(task.sheet))
        return true;

    const QList<Word> &words = task.sheet->renderer()->getWords(task.page->itemIndex());

    emit sigDocPageWordTaskFinished(task, words);

    return true;
}

bool PageRenderThread::execNextDocPageAnnotationTask()
{
    if (m_quit)
        return false;

    DocPageAnnotationTask task;

    if (!popNextDocPageAnnotationTask(task))
        return false;

    if (!DocSheet::existSheet(task.sheet))
        return true;

    const QList<deepin_reader::Annotation *> annots = task.sheet->renderer()->getAnnotations(task.page->itemIndex());

    emit sigDocPageAnnotationTaskFinished(task, annots);

    return true;
}

bool PageRenderThread::execNextDocPageThumbnailTask()
{
    if (m_quit)
        return false;

    DocPageThumbnailTask task;

    if (!popNextDocPageThumbnailTask(task))
        return false;

    if (!DocSheet::existSheet(task.sheet))
        return true;

    QImage image = task.sheet->getImage(task.index, 174, 174);

    if (!image.isNull())
        emit sigDocPageThumbnailTaskFinished(task, QPixmap::fromImage(image));

    return true;
}

bool PageRenderThread::execNextDocOpenTask()
{
    if (m_quit)
        return false;//false 为不用再继续循环调用

    DocOpenTask task;

    if (!popNextDocOpenTask(task))
        return false;//false 为不用再继续循环调用

    if (!DocSheet::existSheet(task.sheet))
        return true;

    QString filePath = task.sheet->filePath();

    PERF_PRINT_BEGIN("POINT-03", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));

    deepin_reader::Document::Error error = deepin_reader::Document::NoError;

    deepin_reader::Document *document = deepin_reader::DocumentFactory::getDocument(task.sheet->fileType(), filePath, task.sheet->convertedFileDir(), task.password, error);

    if (nullptr == document) {
        emit sigDocOpenTask(task, error, nullptr, QList<deepin_reader::Page *>());

    } else {
        int pagesNumber = document->pageCount();

        QList<deepin_reader::Page *> pages;

        for (int i = 0; i < pagesNumber; ++i) {
            deepin_reader::Page *page = document->page(i);

            if (nullptr == page)
                break;

            pages.append(page);
        }

        if (pages.count() == pagesNumber) {
            emit sigDocOpenTask(task, deepin_reader::Document::NoError, document, pages);

        } else {
            qDeleteAll(pages);

            pages.clear();

            delete document;

            emit sigDocOpenTask(task, deepin_reader::Document::FileDamaged, nullptr, QList<deepin_reader::Page *>());
        }
    }

    PERF_PRINT_END("POINT-03", "");
    PERF_PRINT_END("POINT-05", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));

    return true;
}

bool PageRenderThread::execNextDocCloseTask()
{
    if (m_quit)
        return false;   //false 为不用再继续循环调用

    DocCloseTask task;

    if (!popNextDocCloseTask(task))
        return false;   //false 为不用再继续循环调用

    qDeleteAll(task.pages);

    delete task.document;

    return true;
}

void PageRenderThread::onDocPageNormalImageTaskFinished(DocPageNormalImageTask task, QPixmap pixmap)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap);
    }
}

void PageRenderThread::onDocPageSliceImageTaskFinished(DocPageSliceImageTask task, QPixmap pixmap)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap, task.slice);
    }
}

void PageRenderThread::onDocPageBigImageTaskFinished(DocPageBigImageTask task, QPixmap pixmap)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap);
    }
}

void PageRenderThread::onDocPageWordTaskFinished(DocPageWordTask task, QList<deepin_reader::Word> words)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleWordLoaded(words);
    }
}

void PageRenderThread::onDocPageAnnotationTaskFinished(DocPageAnnotationTask task, QList<deepin_reader::Annotation *> annots)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleAnnotationLoaded(annots);
    }
}

void PageRenderThread::onDocPageThumbnailTask(DocPageThumbnailTask task, QPixmap pixmap)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.model->handleRenderThumbnail(task.index, pixmap);
    }
}

void PageRenderThread::onDocOpenTask(DocOpenTask task, deepin_reader::Document::Error error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    if (DocSheet::existSheet(task.sheet)) {
        task.renderer->handleOpened(error, document, pages);
    }
}

void PageRenderThread::destroyForever()
{
    s_quitForever = true;

    if (nullptr != s_instance)
        delete s_instance;
}

PageRenderThread *PageRenderThread::instance()
{
    if (s_quitForever)
        return nullptr;

    if (nullptr == s_instance) {
        s_instance = new PageRenderThread;
    }

    return s_instance;
}

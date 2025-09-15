// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    qDebug() << "PageRenderThread created";
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
    qDebug() << "PageRenderThread::PageRenderThread() - Constructor completed";
}

PageRenderThread::~PageRenderThread()
{
    // qDebug() << "PageRenderThread destroyed";
    m_quit = true;
    wait();
    // qDebug() << "PageRenderThread::~PageRenderThread() - Destructor completed";
}

bool PageRenderThread::clearImageTasks(DocSheet *sheet, BrowserPage *page, int pixmapId)
{
    // qDebug() << "PageRenderThread::clearImageTasks() - Starting clear image tasks";
    if (nullptr == page) {
        // qDebug() << "PageRenderThread::clearImageTasks() - Page is null, returning true";
        return true;
    }

    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::clearImageTasks() - Instance is null, returning false";
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
                // qDebug() << "PageRenderThread::clearImageTasks() - Removing normal image task at index:" << i;
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
                // qDebug() << "PageRenderThread::clearImageTasks() - Removing slice image task at index:" << i;
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
                // qDebug() << "PageRenderThread::clearImageTasks() - Removing big image task at index:" << i;
                instance->m_pageBigImageTasks.removeAt(i);
                exist = true;
                break;
            }
        }
    }

    instance->m_pageBigImageMutex.unlock();
    // qDebug() << "PageRenderThread::clearImageTasks() - Clear image tasks completed";
    return true;
}

void PageRenderThread::appendTask(DocPageNormalImageTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append normal image task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        qWarning() << "PageRenderThread instance is null";
        return;
    }

    instance->m_pageNormalImageMutex.lock();

    instance->m_pageNormalImageTasks.append(task);
    // qDebug() << "Append normal image task, page:" << task.page->itemIndex();

    instance->m_pageNormalImageMutex.unlock();

    if (!instance->isRunning()) {
        // qDebug() << "Starting render thread";
        instance->start();
    }
    // qDebug() << "PageRenderThread::appendTask() - Append normal image task completed";
}

void PageRenderThread::appendTask(DocPageSliceImageTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append slice image task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
        return;
    }

    instance->m_pageSliceImageMutex.lock();

    instance->m_pageSliceImageTasks.append(task);

    instance->m_pageSliceImageMutex.unlock();

    if (!instance->isRunning()) {
        // qDebug() << "Starting render thread";
        instance->start();
    }
    // qDebug() << "PageRenderThread::appendTask() - Append slice image task completed";
}

void PageRenderThread::appendTask(DocPageBigImageTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append big image task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
        return;
    }

    instance->m_pageBigImageMutex.lock();

    instance->m_pageBigImageTasks.append(task);

    instance->m_pageBigImageMutex.unlock();

    if (!instance->isRunning()) {
        // qDebug() << "Starting render thread";
        instance->start();
    }
    // qDebug() << "PageRenderThread::appendTask() - Append big image task completed";
}

void PageRenderThread::appendTask(DocPageWordTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append word task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
        return;
    }

    instance->m_pageWordMutex.lock();

    instance->m_pageWordTasks.append(task);

    instance->m_pageWordMutex.unlock();

    if (!instance->isRunning()) {
        // qDebug() << "Starting render thread";
        instance->start();
    }
    // qDebug() << "PageRenderThread::appendTask() - Append word task completed";
}

void PageRenderThread::appendTask(DocPageAnnotationTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append annotation task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
        return;
    }

    instance->m_pageAnnotationMutex.lock();

    instance->m_pageAnnotationTasks.append(task);

    instance->m_pageAnnotationMutex.unlock();

    if (!instance->isRunning()) {
        // qDebug() << "Starting render thread";
        instance->start();
    }
    // qDebug() << "PageRenderThread::appendTask() - Append annotation task completed";
}

void PageRenderThread::appendTask(DocPageThumbnailTask task)
{
    // qDebug() << "PageRenderThread::appendTask() - Starting append thumbnail task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
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
    // qDebug() << "PageRenderThread::appendTask() - Starting append open task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
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
    // qDebug() << "PageRenderThread::appendTask() - Starting append close task";
    PageRenderThread *instance  = PageRenderThread::instance();

    if (nullptr == instance) {
        // qDebug() << "PageRenderThread::appendTask() - Instance is null, returning";
        return;
    }

    instance->m_closeMutex.lock();

    instance->m_closeTasks.append(task);

    instance->m_closeMutex.unlock();

    // qDebug() << "当前任务线程运行状态: " << instance->isRunning();
    if (!instance->isRunning())
        instance->start();
    // qDebug() << "PageRenderThread::appendTask() - Append close task completed";
}

void PageRenderThread::run()
{
    m_quit = false;
    qDebug() << "====开始执行任务====";

    while (!m_quit) {
        if (!hasNextTask()) {
            //qDebug() << "任务池中不存在任务！等待添加任务";
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

        if (m_quit)
            break;

        DocPageBigImageTask task;
        if (!popNextDocPageBigImageTask(task)) {
            continue;
        }

        QList<QRect> renderRects;

        if (task.sheet->renderer()->hasWidgetAnnots(task.page->itemIndex())) {
            //if has signature,render whole rect
            renderRects.append(task.rect);
        } else {
            int wCount = task.rect.width() % 1000 == 0 ? (task.rect.width() / 1000) : (task.rect.width() / 1000 + 1);
            for (int i = 0; i < wCount; ++i) {//只能以宽度前进(即只能分割宽度)，如果x从0开始，每次都将消耗一定时间
                renderRects.append(QRect(i * 1000, 0, 1000, task.rect.height()));
            };
        }

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

            int counter = 0;
            while (execNextDocPageThumbnailTask()) {
                if (counter++ >= 2) { // 在获取大图期间，每次最多只加载3个缩略图
                    break;
                }
            }
        }

        if (m_quit)
            break;

        emit sigDocPageBigImageTaskFinished(task, pixmap);
    }

    //处理关闭所有文档
    while (execNextDocCloseTask())
    {}
    qDebug() << "====结束执行任务====";
}

bool PageRenderThread::hasNextTask()
{
    // qDebug() << "PageRenderThread::hasNextTask() - Starting has next task";
    QMutexLocker pageNormalImageLocker(&m_pageNormalImageMutex);
    QMutexLocker pageSliceImageLocker(&m_pageSliceImageMutex);
    QMutexLocker pageBigImageLocker(&m_pageBigImageMutex);
    QMutexLocker pageWordLocker(&m_pageWordMutex);
    QMutexLocker pageThumbnailLocker(&m_pageThumbnailMutex);
    QMutexLocker pageOpenLocker(&m_openMutex);
    QMutexLocker pageCloseLocker(&m_closeMutex);

    return !m_pageNormalImageTasks.isEmpty() || !m_pageSliceImageTasks.isEmpty() ||
           !m_pageBigImageTasks.isEmpty() || !m_pageWordTasks.isEmpty() || !m_pageThumbnailTasks.isEmpty() || !m_openTasks.isEmpty() || !m_closeTasks.isEmpty();
}

bool PageRenderThread::popNextDocPageNormalImageTask(DocPageNormalImageTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageNormalImageTask() - Starting pop next normal image task";
    QMutexLocker locker(&m_pageNormalImageMutex);

    if (m_pageNormalImageTasks.count() <= 0)
        return false;

    task = m_pageNormalImageTasks.last();

    m_pageNormalImageTasks.removeLast();

    // qDebug() << "PageRenderThread::popNextDocPageNormalImageTask() - Pop next normal image task completed";
    return true;
}

bool PageRenderThread::popNextDocPageSliceImageTask(DocPageSliceImageTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageSliceImageTask() - Starting pop next slice image task";
    QMutexLocker locker(&m_pageSliceImageMutex);

    if (m_pageSliceImageTasks.count() <= 0)
        return false;

    task = m_pageSliceImageTasks.last();

    m_pageSliceImageTasks.removeLast();

    // qDebug() << "PageRenderThread::popNextDocPageSliceImageTask() - Pop next slice image task completed";
    return true;
}

bool PageRenderThread::popNextDocPageBigImageTask(DocPageBigImageTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageBigImageTask() - Starting pop next big image task";
    QMutexLocker locker(&m_pageBigImageMutex);

    if (m_pageBigImageTasks.count() <= 0)
        return false;

    task = m_pageBigImageTasks.last();

    m_pageBigImageTasks.removeLast();

    // qDebug() << "PageRenderThread::popNextDocPageBigImageTask() - Pop next big image task completed";
    return true;
}

bool PageRenderThread::popNextDocPageWordTask(DocPageWordTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageWordTask() - Starting pop next word task";
    QMutexLocker locker(&m_pageWordMutex);

    if (m_pageWordTasks.count() <= 0)
        return false;

    task = m_pageWordTasks.value(0);

    m_pageWordTasks.removeAt(0);

    // qDebug() << "PageRenderThread::popNextDocPageWordTask() - Pop next word task completed";
    return true;
}

bool PageRenderThread::popNextDocPageAnnotationTask(DocPageAnnotationTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageAnnotationTask() - Starting pop next annotation task";
    QMutexLocker locker(&m_pageAnnotationMutex);

    if (m_pageAnnotationTasks.count() <= 0)
        return false;

    task = m_pageAnnotationTasks.value(0);

    m_pageAnnotationTasks.removeAt(0);

    // qDebug() << "PageRenderThread::popNextDocPageAnnotationTask() - Pop next annotation task completed";
    return true;
}

bool PageRenderThread::popNextDocPageThumbnailTask(DocPageThumbnailTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocPageThumbnailTask() - Starting pop next thumbnail task";
    QMutexLocker locker(&m_pageThumbnailMutex);

    // 优先显示除缩略图外的图片
    if (m_pageThumbnailTasks.count() <= 0
            || m_pageNormalImageTasks.count() > 0
            || m_pageBigImageTasks.count() > 0
            || m_pageSliceImageTasks.count() > 0) {
        return false;
    }

    task = m_pageThumbnailTasks.last();

    m_pageThumbnailTasks.removeLast();

    // qDebug() << "PageRenderThread::popNextDocPageThumbnailTask() - Pop next thumbnail task completed";
    return true;
}

bool PageRenderThread::popNextDocOpenTask(DocOpenTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocOpenTask() - Starting pop next open task";
    QMutexLocker locker(&m_openMutex);

    if (m_openTasks.count() <= 0)
        return false;

    task = m_openTasks.value(0);

    m_openTasks.removeAt(0);

    // qDebug() << "PageRenderThread::popNextDocOpenTask() - Pop next open task completed";
    return true;
}

bool PageRenderThread::popNextDocCloseTask(DocCloseTask &task)
{
    // qDebug() << "PageRenderThread::popNextDocCloseTask() - Starting pop next close task";
    QMutexLocker locker(&m_closeMutex);

    if (m_closeTasks.count() <= 0)
        return false;

    task = m_closeTasks.value(0);

    m_closeTasks.removeAt(0);

    // qDebug() << "PageRenderThread::popNextDocCloseTask() - Pop next close task completed";
    return true;
}

bool PageRenderThread::execNextDocPageNormalImageTask()
{
    qDebug() << "Executing normal image task...";

    if (m_quit) {
        qDebug() << "Render thread quitting, abort task";
        return false;
    }

    DocPageNormalImageTask task;

    if (!popNextDocPageNormalImageTask(task)) {
        qDebug() << "No normal image tasks in queue";
        return false;
    }

    if (!DocSheet::existSheet(task.sheet)) {
        qWarning() << "Sheet no longer exists, skip task";
        return true;
    }

    QImage image = task.sheet->getImage(task.page->itemIndex(), task.rect.width(), task.rect.height());

    if (image.isNull()) {
        qWarning() << "Failed to get image for page:" << task.page->itemIndex();
    } else {
        qDebug() << "Image rendered successfully for page:" << task.page->itemIndex();
        emit sigDocPageNormalImageTaskFinished(task, QPixmap::fromImage(image));
    }

    return true;
}

bool PageRenderThread::execNextDocPageSliceImageTask()
{
    qDebug() << "正在执行取切片任务...";
    if (m_quit) {
        qDebug() << "取切片任务已结束";
        return false;
    }

    DocPageSliceImageTask task;

    if (!popNextDocPageSliceImageTask(task)) {
        qDebug() << "任务池不存在取切片任务，已结束";
        return false;
    }


    if (!DocSheet::existSheet(task.sheet)) {
        qDebug() << "文档不存在，取切片任务已结束";
        return true;
    }

    QImage image = task.sheet->getImage(task.page->itemIndex(), task.whole.width(), task.whole.height(), task.slice);

    if (!image.isNull())
        emit sigDocPageSliceImageTaskFinished(task, QPixmap::fromImage(image));

    qDebug() << "执行取切片文字任务已完成";
    return true;
}

bool PageRenderThread::execNextDocPageWordTask()
{
    qDebug() << "正在执行取页码文字任务...";
    if (m_quit) {
        qDebug() << "取页码文字任务已结束";
        return false;
    }

    DocPageWordTask task;

    if (!popNextDocPageWordTask(task)) {
        qDebug() << "任务池不存在取页码文字任务，已结束";
        return false;
    }

    if (!DocSheet::existSheet(task.sheet)) {
        qDebug() << "文档不存在，取页码文字任务已结束";
        return true;
    }


    const QList<Word> &words = task.sheet->renderer()->getWords(task.page->itemIndex());

    emit sigDocPageWordTaskFinished(task, words);

    qDebug() << "执行取页码文字任务已完成";
    return true;
}

bool PageRenderThread::execNextDocPageAnnotationTask()
{
    qDebug() << "正在执行取页码注释任务...";
    if (m_quit) {
        qDebug() << "取页码注释任务已结束";
        return false;
    }

    DocPageAnnotationTask task;

    if (!popNextDocPageAnnotationTask(task)) {
        qDebug() << "任务池不存在取页码注释任务，已结束";
        return false;
    }

    if (!DocSheet::existSheet(task.sheet)) {
        qDebug() << "文档不存在，取页码注释任务已结束";
        return true;
    }

    const QList<deepin_reader::Annotation *> annots = task.sheet->renderer()->getAnnotations(task.page->itemIndex());

    emit sigDocPageAnnotationTaskFinished(task, annots);

    qDebug() << "执行取页码注释任务已完成";
    return true;
}

bool PageRenderThread::execNextDocPageThumbnailTask()
{
    qDebug() << "正在执行缩略图任务...";
    if (m_quit) {
        qDebug() << "缩略图任务已结束";
        return false;
    }

    DocPageThumbnailTask task;

    if (!popNextDocPageThumbnailTask(task)) {
        qDebug() << "任务池不存在缩略图任务，已结束";
        return false;
    }

    if (!DocSheet::existSheet(task.sheet)) {
        qDebug() << "文档不存在，缩略图任务已结束";
        return true;
    }

    QImage image = task.sheet->getImage(task.index, 174, 174);

    if (!image.isNull())
        emit sigDocPageThumbnailTaskFinished(task, QPixmap::fromImage(image));
    qDebug() << "执行缩略图任务已完成";
    return true;
}

bool PageRenderThread::execNextDocOpenTask()
{
    qDebug() << "正在执行文档打开任务...";
    if (m_quit) {
        qDebug() << "文档打开任务已结束";
        return false;//false 为不用再继续循环调用
    }

    DocOpenTask task;

    if (!popNextDocOpenTask(task)) {
        qDebug() << "任务池不存在文档打开任务，已结束";
        return false;//false 为不用再继续循环调用
    }

    if (!DocSheet::existSheet(task.sheet)) {
        qDebug() << "文档不存在，文档打开任务已结束";
        return true;
    }

    QString filePath = task.sheet->filePath();

    PERF_PRINT_BEGIN("POINT-03", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));

    deepin_reader::Document::Error error = deepin_reader::Document::NoError;

    qDebug() << "PageRenderThread::execNextDocOpenTask" <<  task.sheet->convertedFileDir();
    deepin_reader::Document *document = deepin_reader::DocumentFactory::getDocument(task.sheet->fileType(), filePath, task.sheet->convertedFileDir(), task.password, &(task.sheet->m_process), error);

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
    qDebug() << "文档打开任务已完成";

    return true;
}

bool PageRenderThread::execNextDocCloseTask()
{
    qDebug() << "正在执行文档关闭任务...";
    DocCloseTask task;

    if (!popNextDocCloseTask(task)) {
        qDebug() << "不存在关闭任务，已结束";
        return false;   //false 为不用再继续循环调用
    }

    qDeleteAll(task.pages);

    delete task.document;

    qDebug() << "文档关闭任务已完成";
    return true;
}

void PageRenderThread::onDocPageNormalImageTaskFinished(DocPageNormalImageTask task, QPixmap pixmap)
{
    // qDebug() << "PageRenderThread::onDocPageNormalImageTaskFinished() - Starting on doc page normal image task finished";
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap);
    }
    // qDebug() << "PageRenderThread::onDocPageNormalImageTaskFinished() - On doc page normal image task finished completed";
}

void PageRenderThread::onDocPageSliceImageTaskFinished(DocPageSliceImageTask task, QPixmap pixmap)
{
    // qDebug() << "PageRenderThread::onDocPageSliceImageTaskFinished() - Starting on doc page slice image task finished";
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap, task.slice);
    }
    // qDebug() << "PageRenderThread::onDocPageSliceImageTaskFinished() - On doc page slice image task finished completed";
}

void PageRenderThread::onDocPageBigImageTaskFinished(DocPageBigImageTask task, QPixmap pixmap)
{
    // qDebug() << "PageRenderThread::onDocPageBigImageTaskFinished() - Starting on doc page big image task finished";
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleRenderFinished(task.pixmapId, pixmap);
    }
    // qDebug() << "PageRenderThread::onDocPageBigImageTaskFinished() - On doc page big image task finished completed";
}

void PageRenderThread::onDocPageWordTaskFinished(DocPageWordTask task, QList<deepin_reader::Word> words)
{
    // qDebug() << "PageRenderThread::onDocPageWordTaskFinished() - Starting on doc page word task finished";
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleWordLoaded(words);
    }
    // qDebug() << "PageRenderThread::onDocPageWordTaskFinished() - On doc page word task finished completed";
}

void PageRenderThread::onDocPageAnnotationTaskFinished(DocPageAnnotationTask task, QList<deepin_reader::Annotation *> annots)
{
    // qDebug() << "PageRenderThread::onDocPageAnnotationTaskFinished() - Starting on doc page annotation task finished";
    if (DocSheet::existSheet(task.sheet)) {
        task.page->handleAnnotationLoaded(annots);
    }
    // qDebug() << "PageRenderThread::onDocPageAnnotationTaskFinished() - On doc page annotation task finished completed";
}

void PageRenderThread::onDocPageThumbnailTask(DocPageThumbnailTask task, QPixmap pixmap)
{
    // qDebug() << "PageRenderThread::onDocPageThumbnailTask() - Starting on doc page thumbnail task";
    if (DocSheet::existSheet(task.sheet)) {
        task.model->handleRenderThumbnail(task.index, pixmap);
    }
    // qDebug() << "PageRenderThread::onDocPageThumbnailTask() - On doc page thumbnail task completed";
}

void PageRenderThread::onDocOpenTask(DocOpenTask task, deepin_reader::Document::Error error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    // qDebug() << "PageRenderThread::onDocOpenTask() - Starting on doc open task";
    if (DocSheet::existSheet(task.sheet)) {
        task.renderer->handleOpened(error, document, pages);
    }
    // qDebug() << "PageRenderThread::onDocOpenTask() - On doc open task completed";
}

void PageRenderThread::destroyForever()
{
    // qDebug() << "PageRenderThread::destroyForever() - Starting destroy forever";
    s_quitForever = true;

    if (nullptr != s_instance) {
        // qDebug() << "PageRenderThread::destroyForever() - Instance is not null, quitting instance";
        s_instance->m_quit = true;
        s_instance->wait();
        delete s_instance;
    }
    // qDebug() << "PageRenderThread::destroyForever() - Destroy forever completed";
}

PageRenderThread *PageRenderThread::instance()
{
    // qDebug() << "PageRenderThread::instance() - Starting instance";
    if (s_quitForever) {
        // qDebug() << "Render thread permanently quit";
        return nullptr;
    }

    if (nullptr == s_instance) {
        // qDebug() << "Creating new render thread instance";
        s_instance = new PageRenderThread;
    }

    // qDebug() << "PageRenderThread::instance() - Instance created";
    return s_instance;
}

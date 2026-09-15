// Copyright (C) 2019 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PAGERENDERTHREAD_H
#define PAGERENDERTHREAD_H

#include "Global.h"
#include "Model.h"

#include <QThread>
#include <QMutex>
#include <atomic>
#include <QStack>
#include <QImage>
#include <QSharedPointer>

class DocSheet;
class BrowserPage;
class SheetRenderer;
class SideBarImageViewModel;
class QProcess;

// worker 线程约定:任务结构体中仅允许解引用 renderer(共享引用,生命周期安全)与
// 入队时快照的值类型数据;sheet/page 等裸指针仅供主线程回调使用,worker 禁止访问。
struct DocPageNormalImageTask {//正常取图
    // ---- worker 线程只读数据 ----
    QSharedPointer<SheetRenderer> renderer; //渲染器共享引用
    QString uuid;               //入队时sheet唯一标识,worker据此跳过已销毁文档的任务(仅优化)
    int pageIndex = -1;         //入队时快照,worker不再解引用BrowserPage
    qreal scaleFactor = 0.0;    //入队时快照,兜底尺寸计算用
    QSizeF originSize;          //入队时快照,兜底尺寸计算用
    // ---- 仅供主线程回调使用 ----
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect rect = QRect();       //整个大小
    QVector<QRectF> imageRects; //图片对象 bbox(物理像素,夜间蒙版用;渲染线程预取)
};

struct DocPageSliceImageTask {//取切片
    QSharedPointer<SheetRenderer> renderer;
    QString uuid;
    int pageIndex = -1;
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect whole = QRect();      //整个大小
    QRect slice = QRect();      //切片大小
};

struct DocPageBigImageTask {//取大图
    QSharedPointer<SheetRenderer> renderer;
    QString uuid;
    int pageIndex = -1;
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect rect = QRect();       //整个大小
    QVector<QRectF> imageRects; //图片对象 bbox(物理像素,夜间蒙版用;渲染线程预取)
};

struct DocPageWordTask {//取页码文字
    QSharedPointer<SheetRenderer> renderer;
    QString uuid;
    int pageIndex = -1;
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
};

struct DocPageAnnotationTask {//取页码注释
    QSharedPointer<SheetRenderer> renderer;
    QString uuid;
    int pageIndex = -1;
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
};

struct DocPageThumbnailTask {//缩略图
    QSharedPointer<SheetRenderer> renderer;
    QString uuid;
    DocSheet *sheet = nullptr;
    SideBarImageViewModel *model = nullptr;
    int index = -1;
};

struct DocOpenTask {//打开文档
    DocSheet *sheet = nullptr;   //仅供主线程回调使用
    QString password;
    QString uuid;                //排队时的sheet唯一标识,防止地址复用误判存活
    QString filePath;            //入队时快照,worker不再访问sheet
    QString convertedFileDir;
    int fileType = 0;            //Dr::FileType
    QProcess *process = nullptr; //getDocument出参,由主线程回调写回sheet->m_process
};

struct DocCloseTask {//关闭文档
    deepin_reader::Document *document = nullptr;
    QList<deepin_reader::Page *> pages;
};

/**
 * @brief The PageRenderThread class
 * 执行加载图片和文字等耗时操作的线程,由于pdfium非常线程不安全，所有操作都在本线程中进行
 */
class PageRenderThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief clearImageTasks
     * 清除需要读取图片的任务
     * @param sheet
     * @param item 项指针
     * @param pixmapId 删除不同的pixmapId,-1为删除所有
     * @return 是否成功
     */
    static bool clearImageTasks(DocSheet *sheet, BrowserPage *page, int pixmapId = -1);
    /**
     * @brief clearAllTasksForSheet
     * 清除指定 sheet 的所有待处理任务（图片/文字/注释/缩略图）。
     * 用于 DocSheet 析构前排空引用该 sheet 的后台任务，避免悬空访问。
     * @param sheet 目标 sheet 指针
     */
    static void clearAllTasksForSheet(DocSheet *sheet);

    /**
     * @brief clearAllTasksForPage
     * 排空队列中所有引用指定 page 的文字/注释任务(图像任务由 clearImageTasks 负责);
     * 仅主线程调用(BrowserPage 析构时),与 worker 通过各队列互斥锁互斥
     * @param page 页面对象
     */
    static void clearAllTasksForPage(const BrowserPage *page);


    /**
     * @brief appendTask
     * 添加任务到队列
     * @param task
     */
    static void appendTask(DocPageNormalImageTask task);

    static void appendTask(DocPageSliceImageTask task);

    static void appendTask(DocPageBigImageTask task);

    static void appendTask(DocPageWordTask task);

    static void appendTask(DocPageAnnotationTask task);

    static void appendTask(DocPageThumbnailTask task);

    static void appendTask(DocOpenTask task);

    static void appendTask(DocCloseTask task);

    /**
     * @brief destroyForever
     * 销毁线程且不会再被创建
     */
    static void destroyForever();

private:
    explicit PageRenderThread(QObject *parent = nullptr);

    ~PageRenderThread();

    void run();

private:
    /**
     * @brief hasNextTask 任务池中是否还存在任务
     * @return true:存在 false:不存在
     */
    bool hasNextTask();

    /**
     * @brief popNextDocPageNormalImageTask 任务池中是否还存在正常取图任务
     * @param task
     * @return
     */
    bool popNextDocPageNormalImageTask(DocPageNormalImageTask &task);

    /**
     * @brief popNextDocPageSliceImageTask 任务池中是否还存在取切片任务
     * @param task
     * @return
     */
    bool popNextDocPageSliceImageTask(DocPageSliceImageTask &task);

    /**
     * @brief popNextDocPageBigImageTask 任务池中是否还存在取大图任务
     * @param task
     * @return
     */
    bool popNextDocPageBigImageTask(DocPageBigImageTask &task);

    /**
     * @brief popNextDocPageWordTask 任务池中是否还存在取页码文字任务
     * @param task
     * @return
     */
    bool popNextDocPageWordTask(DocPageWordTask &task);

    /**
     * @brief popNextDocPageAnnotationTask 任务池中是否还存在取页码注释任务
     * @param task
     * @return
     */
    bool popNextDocPageAnnotationTask(DocPageAnnotationTask &task);

    /**
     * @brief popNextDocPageThumbnailTask 任务池中是否还存在缩略图任务
     * @param task
     * @return
     */
    bool popNextDocPageThumbnailTask(DocPageThumbnailTask &task);

    /**
     * @brief popNextDocOpenTask 任务池中是否还存在打开文档任务
     * @param task
     * @return
     */
    bool popNextDocOpenTask(DocOpenTask &task);

    /**
     * @brief popNextDocCloseTask 任务池中是否还存在文档关闭任务
     * @param task
     * @return true: 是 false:否
     */
    bool popNextDocCloseTask(DocCloseTask &task);

private:

    /**
     * @brief execNextDocPageNormalImageTask 执行正常取图任务
     * @return
     */
    bool execNextDocPageNormalImageTask();

    /**
     * @brief execNextDocPageSliceImageTask 执行取切片任务
     * @return
     */
    bool execNextDocPageSliceImageTask();

    /**
     * @brief execNextDocPageWordTask 执行取页码文字
     * @return
     */
    bool execNextDocPageWordTask();

    /**
     * @brief execNextDocPageAnnotationTask 执行取页码注释
     * @return
     */
    bool execNextDocPageAnnotationTask();

    /**
     * @brief execNextDocPageThumbnailTask 执行缩略图
     * @return
     */
    bool execNextDocPageThumbnailTask();

    /**
     * @brief execNextDocOpenTask 执行文档打开任务
     * @return
     */
    bool execNextDocOpenTask();

    /**
     * @brief execNextDocCloseTask 执行文档关闭任务
     * @return
     */
    bool execNextDocCloseTask();

signals:
    void sigDocPageNormalImageTaskFinished(DocPageNormalImageTask, QPixmap);

    void sigDocPageSliceImageTaskFinished(DocPageSliceImageTask, QPixmap);

    void sigDocPageBigImageTaskFinished(DocPageBigImageTask, QPixmap);

    void sigDocPageWordTaskFinished(DocPageWordTask, QList<deepin_reader::Word>);

    void sigDocPageAnnotationTaskFinished(DocPageAnnotationTask, QList<deepin_reader::Annotation *>);

    void sigDocPageThumbnailTaskFinished(DocPageThumbnailTask, QPixmap);

    void sigDocOpenTask(DocOpenTask, deepin_reader::Document::Error, deepin_reader::Document *, QList<deepin_reader::Page *>);

private slots:
    void onDocPageNormalImageTaskFinished(DocPageNormalImageTask task, QPixmap pixmap);

    void onDocPageSliceImageTaskFinished(DocPageSliceImageTask task, QPixmap pixmap);

    void onDocPageBigImageTaskFinished(DocPageBigImageTask task, QPixmap pixmap);

    void onDocPageWordTaskFinished(DocPageWordTask task, QList<deepin_reader::Word> words);

    void onDocPageAnnotationTaskFinished(DocPageAnnotationTask task, QList<deepin_reader::Annotation *> annots);

    void onDocPageThumbnailTask(DocPageThumbnailTask task, QPixmap pixmap);

    void onDocOpenTask(DocOpenTask task, deepin_reader::Document::Error error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages);

private:
    QMutex m_pageNormalImageMutex;
    QList<DocPageNormalImageTask> m_pageNormalImageTasks;

    QMutex m_pageSliceImageMutex;
    QList<DocPageSliceImageTask> m_pageSliceImageTasks;

    QMutex m_pageBigImageMutex;
    QList<DocPageBigImageTask> m_pageBigImageTasks;

    QMutex m_pageWordMutex;
    QList<DocPageWordTask> m_pageWordTasks;

    QMutex m_pageAnnotationMutex;
    QList<DocPageAnnotationTask> m_pageAnnotationTasks;

    QMutex m_pageThumbnailMutex;
    QList<DocPageThumbnailTask> m_pageThumbnailTasks;

    QMutex m_openMutex;
    QList<DocOpenTask> m_openTasks;

    QMutex m_closeMutex;
    QList<DocCloseTask> m_closeTasks;

    // 原子量: 主线程 destroyForever 写、渲染线程读, 避免数据竞争
    std::atomic<bool> m_quit {false};

    static bool s_quitForever;

    static PageRenderThread *s_instance;

    static PageRenderThread *instance();
};

#endif // PAGERENDERTHREAD_H

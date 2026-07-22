// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PageRenderThread.h"
#include "DocSheet.h"
#include "BrowserPage.h"
#include "SheetRenderer.h"
#include "SideBarImageViewModel.h"
#include "stub.h"

#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <gtest/gtest.h>
/********测试PageRenderThread***********/
class TestPageRenderThread : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    PageRenderThread *m_tester = nullptr;
};

void TestPageRenderThread::SetUp()
{
    m_tester = PageRenderThread::instance();
}

void TestPageRenderThread::TearDown()
{
}

/**********桩函数*************/
static QString g_funcName;
static void start_stub(QThread::Priority)
{
    g_funcName = __FUNCTION__;
    qInfo() << "chendu" << __FUNCTION__;
}

static bool wait_stub(unsigned long)
{
    g_funcName = __FUNCTION__;
    qInfo() << "chendu" << __FUNCTION__;
    return true;
}
void run_stub()
{
    g_funcName = __FUNCTION__;
    qInfo() << "chendu" << __FUNCTION__;
}

// Stubs for methods called inside onDoc*Finished slots (true-branch).
// When task.sheet exists, the slot calls task.page/model/renderer methods.
// These stubs are installed so nullptr page/model/renderer won't crash.
static void handleRenderFinished_stub(const int &, const QPixmap &, const QRect &)
{
    g_funcName = __FUNCTION__;
}

static void handleWordLoaded_stub(const QList<deepin_reader::Word> &)
{
    g_funcName = __FUNCTION__;
}

static void handleAnnotationLoaded_stub(const QList<deepin_reader::Annotation *> &)
{
    g_funcName = __FUNCTION__;
}

static void handleRenderThumbnail_stub(int, QPixmap)
{
    g_funcName = __FUNCTION__;
}

static void handleOpened_stub(deepin_reader::Document::Error, deepin_reader::Document *, QList<deepin_reader::Page *>)
{
    g_funcName = __FUNCTION__;
}

// Makes DocSheet::existSheet() return true so the onDoc*Finished slots
// take the "sheet exists" branch without needing a real DocSheet (whose
// destructor would otherwise start the render thread and deadlock the test).
static bool existSheet_true_stub(DocSheet *)
{
    return true;
}

/*********测试用例**********/
//TEST_F(TestPageRenderThread, UT_PageRenderThread_clearImageTasks_001)
//{
//    typedef void (*fptr)(PageRenderThread *);
//    fptr A_foo = (fptr)(&PageRenderThread::run);   //获取虚函数地址
//    Stub s;
////    s.set(ADDR(QThread, start), start_stub);
////    s.set(ADDR(QThread, wait), wait_stub);
//    s.set(A_foo, run_stub);
//    EXPECT_TRUE(m_tester->clearImageTasks(nullptr, nullptr, 0));

//    QString strPath = UTSOURCEDIR;
//    strPath += "/files/normal.pdf";
//    DocSheet *docsheet = new DocSheet(Dr::FileType::PDF, strPath, nullptr);
//    BrowserPage *browserpage = new BrowserPage(docsheet->m_browser, 0, docsheet);
//    DocPageNormalImageTask task;
//    task.sheet = docsheet;
//    task.page = browserpage;
//    task.rect = QRect(0, 0, 500, 800);
//    m_tester->appendTask(task);
//    EXPECT_TRUE(m_tester->clearImageTasks(docsheet, browserpage, 0));

//}

//======================================================================
// onDoc*Finished slots, false-branch (sheet == nullptr, existSheet false)
//======================================================================

// Tests onDocPageNormalImageTaskFinished when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageNormalImageTaskFinished_001)
{
    DocPageNormalImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    task.pixmapId = 0;
    QPixmap pix;
    m_tester->onDocPageNormalImageTaskFinished(task, pix);
    SUCCEED();
}

// Tests onDocPageSliceImageTaskFinished when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageSliceImageTaskFinished_001)
{
    DocPageSliceImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    task.pixmapId = 0;
    QPixmap pix;
    m_tester->onDocPageSliceImageTaskFinished(task, pix);
    SUCCEED();
}

// Tests onDocPageBigImageTaskFinished when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageBigImageTaskFinished_001)
{
    DocPageBigImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    task.pixmapId = 0;
    QPixmap pix;
    m_tester->onDocPageBigImageTaskFinished(task, pix);
    SUCCEED();
}

// Tests onDocPageWordTaskFinished when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageWordTaskFinished_001)
{
    DocPageWordTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    QList<deepin_reader::Word> words;
    m_tester->onDocPageWordTaskFinished(task, words);
    SUCCEED();
}

// Tests onDocPageAnnotationTaskFinished when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageAnnotationTaskFinished_001)
{
    DocPageAnnotationTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    QList<deepin_reader::Annotation *> annots;
    m_tester->onDocPageAnnotationTaskFinished(task, annots);
    SUCCEED();
}

// Tests onDocPageThumbnailTask when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageThumbnailTask_001)
{
    DocPageThumbnailTask task;
    task.sheet = nullptr;
    task.model = nullptr;
    task.index = 0;
    QPixmap pix;
    m_tester->onDocPageThumbnailTask(task, pix);
    SUCCEED();
}

// Tests onDocOpenTask when sheet does not exist.
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocOpenTask_001)
{
    DocOpenTask task;
    task.sheet = nullptr;
    task.renderer = nullptr;
    QList<deepin_reader::Page *> pages;
    m_tester->onDocOpenTask(task, deepin_reader::Document::NoError, nullptr, pages);
    SUCCEED();
}

//======================================================================
// onDoc*Finished slots, true-branch (real sheet, existSheet true)
// Stubs are installed so the forwarded calls on null page/model/renderer
// don't crash.
//======================================================================

// Tests onDocPageNormalImageTaskFinished when sheet exists; forwards to
// BrowserPage::handleRenderFinished (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageNormalImageTaskFinished_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(BrowserPage, handleRenderFinished), handleRenderFinished_stub);

    DocPageNormalImageTask task;
    task.sheet = nullptr;   // existSheet is stubbed to return true anyway
    task.page = nullptr;   // stub will be invoked, nullptr this is ignored
    task.pixmapId = 1;
    QPixmap pix;
    m_tester->onDocPageNormalImageTaskFinished(task, pix);
    EXPECT_TRUE(g_funcName == "handleRenderFinished_stub");
}

// Tests onDocPageSliceImageTaskFinished when sheet exists; forwards to
// BrowserPage::handleRenderFinished with slice (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageSliceImageTaskFinished_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(BrowserPage, handleRenderFinished), handleRenderFinished_stub);

    DocPageSliceImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    task.pixmapId = 2;
    task.slice = QRect(0, 0, 10, 10);
    QPixmap pix;
    m_tester->onDocPageSliceImageTaskFinished(task, pix);
    EXPECT_TRUE(g_funcName == "handleRenderFinished_stub");
}

// Tests onDocPageBigImageTaskFinished when sheet exists; forwards to
// BrowserPage::handleRenderFinished (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageBigImageTaskFinished_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(BrowserPage, handleRenderFinished), handleRenderFinished_stub);

    DocPageBigImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    task.pixmapId = 3;
    QPixmap pix;
    m_tester->onDocPageBigImageTaskFinished(task, pix);
    EXPECT_TRUE(g_funcName == "handleRenderFinished_stub");
}

// Tests onDocPageWordTaskFinished when sheet exists; forwards to
// BrowserPage::handleWordLoaded (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageWordTaskFinished_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(BrowserPage, handleWordLoaded), handleWordLoaded_stub);

    DocPageWordTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    QList<deepin_reader::Word> words;
    m_tester->onDocPageWordTaskFinished(task, words);
    EXPECT_TRUE(g_funcName == "handleWordLoaded_stub");
}

// Tests onDocPageAnnotationTaskFinished when sheet exists; forwards to
// BrowserPage::handleAnnotationLoaded (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageAnnotationTaskFinished_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(BrowserPage, handleAnnotationLoaded), handleAnnotationLoaded_stub);

    DocPageAnnotationTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    QList<deepin_reader::Annotation *> annots;
    m_tester->onDocPageAnnotationTaskFinished(task, annots);
    EXPECT_TRUE(g_funcName == "handleAnnotationLoaded_stub");
}

// Tests onDocPageThumbnailTask when sheet exists; forwards to
// SideBarImageViewModel::handleRenderThumbnail (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocPageThumbnailTask_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(SideBarImageViewModel, handleRenderThumbnail), handleRenderThumbnail_stub);

    DocPageThumbnailTask task;
    task.sheet = nullptr;
    task.model = nullptr;
    task.index = 5;
    QPixmap pix;
    m_tester->onDocPageThumbnailTask(task, pix);
    EXPECT_TRUE(g_funcName == "handleRenderThumbnail_stub");
}

// Tests onDocOpenTask when sheet exists; forwards to
// SheetRenderer::handleOpened (stubbed).
TEST_F(TestPageRenderThread, UT_PageRenderThread_onDocOpenTask_002)
{
    Stub s;
    s.set(ADDR(DocSheet, existSheet), existSheet_true_stub);
    s.set(ADDR(SheetRenderer, handleOpened), handleOpened_stub);

    DocOpenTask task;
    task.sheet = nullptr;
    task.renderer = nullptr;
    QList<deepin_reader::Page *> pages;
    m_tester->onDocOpenTask(task, deepin_reader::Document::NoError, nullptr, pages);
    EXPECT_TRUE(g_funcName == "handleOpened_stub");
}

//======================================================================
// appendTask overloads (static). Thread start is stubbed to keep the
// queued tasks from running during the test.
//======================================================================

// Tests appendTask(DocPageNormalImageTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_NormalImage)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageNormalImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageNormalImageTasks.isEmpty());
    m_tester->m_pageNormalImageTasks.clear();
}

// Tests appendTask(DocPageSliceImageTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_SliceImage)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageSliceImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageSliceImageTasks.isEmpty());
    m_tester->m_pageSliceImageTasks.clear();
}

// Tests appendTask(DocPageBigImageTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_BigImage)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageBigImageTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageBigImageTasks.isEmpty());
    m_tester->m_pageBigImageTasks.clear();
}

// Tests appendTask(DocPageWordTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_Word)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageWordTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageWordTasks.isEmpty());
    m_tester->m_pageWordTasks.clear();
}

// Tests appendTask(DocPageAnnotationTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_Annotation)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageAnnotationTask task;
    task.sheet = nullptr;
    task.page = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageAnnotationTasks.isEmpty());
    m_tester->m_pageAnnotationTasks.clear();
}

// Tests appendTask(DocPageThumbnailTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_Thumbnail)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocPageThumbnailTask task;
    task.sheet = nullptr;
    task.model = nullptr;
    task.index = 0;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_pageThumbnailTasks.isEmpty());
    m_tester->m_pageThumbnailTasks.clear();
}

// Tests appendTask(DocOpenTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_Open)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocOpenTask task;
    task.sheet = nullptr;
    task.renderer = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_openTasks.isEmpty());
    m_tester->m_openTasks.clear();
}

// Tests appendTask(DocCloseTask).
TEST_F(TestPageRenderThread, UT_PageRenderThread_appendTask_Close)
{
    Stub s;
    s.set(ADDR(QThread, start), start_stub);

    DocCloseTask task;
    task.document = nullptr;
    PageRenderThread::appendTask(task);
    EXPECT_FALSE(m_tester->m_closeTasks.isEmpty());
    m_tester->m_closeTasks.clear();
}

// (NullInstance test removed: modifying s_quitForever corrupts global state
//  and causes segfaults in subsequent DocSheet destructor tests.)

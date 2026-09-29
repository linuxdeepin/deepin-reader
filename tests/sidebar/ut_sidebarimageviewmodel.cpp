// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "SideBarImageViewModel.h"
#include "PageRenderThread.h"
#include "PDFModel.h"
#include "dpdfannot.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>
#include <QListView>

class TestImagePageInfo_t : public ::testing::Test
{
public:
    TestImagePageInfo_t(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ImagePageInfo_t();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ImagePageInfo_t *m_tester;
};

TEST_F(TestImagePageInfo_t, initTest)
{

}

TEST_F(TestImagePageInfo_t, test_operators)
{
    ImagePageInfo_t a, b;
    a.pageIndex = 1;
    b.pageIndex = 1;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);

    b.pageIndex = 2;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(a > b);

    a.pageIndex = 3;
    EXPECT_TRUE(a > b);
}



class TestSideBarImageViewModel : public ::testing::Test
{
public:
    TestSideBarImageViewModel() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new SideBarImageViewModel(m_sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    SideBarImageViewModel *m_tester = nullptr;
};

TEST_F(TestSideBarImageViewModel, inittest)
{

}

TEST_F(TestSideBarImageViewModel, testresetData)
{
    m_tester->resetData();
    EXPECT_TRUE(m_tester->m_pagelst.count() == 0);
}

TEST_F(TestSideBarImageViewModel, testinitModelLst)
{
    m_tester->initModelLst(QList<ImagePageInfo_t>() << ImagePageInfo_t(), true);
    EXPECT_TRUE(m_tester->m_pagelst.count() == 1);
}

TEST_F(TestSideBarImageViewModel, testchangeModelData)
{
    m_tester->changeModelData(QList<ImagePageInfo_t>() << ImagePageInfo_t());
    EXPECT_TRUE(m_tester->m_pagelst.count() == 1);
}

TEST_F(TestSideBarImageViewModel, testsetBookMarkVisible)
{
    m_tester->setBookMarkVisible(0, true, true);
    EXPECT_TRUE(m_tester->m_cacheBookMarkMap.count() == 1);
    EXPECT_TRUE(m_tester->m_cacheBookMarkMap[0] == true);
}

TEST_F(TestSideBarImageViewModel, testrowCount)
{
    m_tester->m_pagelst << ImagePageInfo_t();
    EXPECT_TRUE(m_tester->rowCount(QModelIndex()) == 1);
}

TEST_F(TestSideBarImageViewModel, testcolumnCount)
{
    EXPECT_TRUE(m_tester->columnCount(QModelIndex()) == 1);
}

TEST_F(TestSideBarImageViewModel, testdata)
{
    EXPECT_TRUE(m_tester->data(QModelIndex(), Qt::DisplayRole) == QVariant());
}

TEST_F(TestSideBarImageViewModel, testsetData)
{
    EXPECT_TRUE(m_tester->setData(QModelIndex(), "", Qt::DisplayRole) == false);
}

TEST_F(TestSideBarImageViewModel, testgetModelIndexForPageIndex)
{
    m_tester->m_pagelst << ImagePageInfo_t(0);
    EXPECT_TRUE(m_tester->getModelIndexForPageIndex(0).count() == 1);
}

TEST_F(TestSideBarImageViewModel, testgetPageIndexForModelIndex)
{
    m_tester->m_pagelst << ImagePageInfo_t(0);
    EXPECT_TRUE(m_tester->getPageIndexForModelIndex(0) == 0);
}

TEST_F(TestSideBarImageViewModel, testonUpdateImage)
{
    m_tester->onUpdateImage(0);
}

TEST_F(TestSideBarImageViewModel, testinsertPageIndex)
{
    m_tester->insertPageIndex(0);
    EXPECT_TRUE(m_tester->m_pagelst.count() == 1);
}

TEST_F(TestSideBarImageViewModel, testinsertPageIndex1)
{
    m_tester->insertPageIndex(ImagePageInfo_t());
    EXPECT_TRUE(m_tester->m_pagelst.count() == 1);
}

TEST_F(TestSideBarImageViewModel, testremovePageIndex)
{
    m_tester->m_pagelst << ImagePageInfo_t(0);
    m_tester->removePageIndex(0);
    EXPECT_TRUE(m_tester->m_pagelst.count() == 0);
}

TEST_F(TestSideBarImageViewModel, testremoveItemForAnno)
{
    m_tester->removeItemForAnno(nullptr);
    EXPECT_TRUE(m_tester->m_pagelst.count() == 0);
}

TEST_F(TestSideBarImageViewModel, testgetModelIndexImageInfo)
{
    m_tester->m_pagelst << ImagePageInfo_t(0);
    ImagePageInfo_t temp;
    m_tester->getModelIndexImageInfo(0, temp);
    EXPECT_TRUE(temp == ImagePageInfo_t(0));
}

TEST_F(TestSideBarImageViewModel, testfindItemForAnno)
{
    EXPECT_TRUE(m_tester->findItemForAnno(nullptr) == -1);
}

TEST_F(TestSideBarImageViewModel, testhandleRenderThumbnail)
{
    m_tester->handleRenderThumbnail(0, QPixmap());
}

// handleRenderThumbnail 第三参(图片对象 bbox)需存入 DocSheet，供 data(IMAGE_NIGHT_MASK) 读取
TEST_F(TestSideBarImageViewModel, testhandleRenderThumbnailStoresImageRects)
{
    const QVector<QRectF> rects { QRectF(1, 2, 3, 4), QRectF(5, 6, 7, 8) };
    QPixmap thumb(174, 174);
    thumb.fill(Qt::white);

    m_tester->handleRenderThumbnail(0, thumb, rects);

    EXPECT_TRUE(m_sheet->thumbnailImageRects(0) == rects);

    // data(IMAGE_NIGHT_MASK) 返回同一份 bbox，供代理反色时跳过图片区域
    m_tester->insertPageIndex(0);
    const QModelIndex index = m_tester->index(0, 0);
    ASSERT_TRUE(index.isValid());
    const QVector<QRectF> got =
            index.data(ImageinfoType_e::IMAGE_NIGHT_MASK).value<QVector<QRectF>>();
    EXPECT_TRUE(got == rects);
}

// 未设置蒙版时 IMAGE_NIGHT_MASK 返回空列表（整页反色，不跳过图片区域）
TEST_F(TestSideBarImageViewModel, testImageNightMaskDefaultsEmpty)
{
    m_tester->insertPageIndex(0);
    const QModelIndex index = m_tester->index(0, 0);
    ASSERT_TRUE(index.isValid());
    const QVector<QRectF> got =
            index.data(ImageinfoType_e::IMAGE_NIGHT_MASK).value<QVector<QRectF>>();
    EXPECT_TRUE(got.isEmpty());
    EXPECT_TRUE(m_sheet->thumbnailImageRects(0).isEmpty());
}

TEST_F(TestSideBarImageViewModel, testonBatchUpdateTimer)
{
    // Trigger onBatchUpdateTimer directly
    m_tester->onBatchUpdateTimer();
    SUCCEED();
}

/* ========== PMS 回归用例（sev1/2 bug 补强，批次1） ========== */

// PMS: https://pms.uniontech.com/bug-view-343541.html  commit: a3cb6cff
TEST_F(TestSideBarImageViewModel, BUG343541_ctor_batchUpdateTimerConfigured)
{
    // XPS 渲染慢修复引入批量更新去抖：构造后批量刷新 timer 必须为 100ms 单次触发且未激活
    EXPECT_FALSE(m_tester->m_batchUpdateTimer->isActive());
    EXPECT_TRUE(m_tester->m_batchUpdateTimer->isSingleShot());
    EXPECT_EQ(m_tester->m_batchUpdateTimer->interval(), 100);
}

// PMS: https://pms.uniontech.com/bug-view-343541.html  commit: a3cb6cff
TEST_F(TestSideBarImageViewModel, BUG343541_ctor_nullSheetNoCrash)
{
    // 构造入参 sheet 允许为空（内部日志有指针守卫），不得崩溃
    SideBarImageViewModel model(nullptr);
    EXPECT_TRUE(model.m_pagelst.isEmpty());
    EXPECT_EQ(model.rowCount(), 0);
}

// PMS: https://pms.uniontech.com/bug-view-343541.html  commit: a3cb6cff
TEST_F(TestSideBarImageViewModel, BUG343541_handleRenderThumbnail_batchDebounce)
{
    // 缩略图渲染去抖：连续渲染仅启动一次批量刷新 timer，
    // timer 触发后发出 dataChanged 且待更新集合清空
    m_tester->initModelLst(QList<ImagePageInfo_t>() << ImagePageInfo_t(0) << ImagePageInfo_t(1), true);
    m_tester->handleRenderThumbnail(0, QPixmap(10, 10), QVector<QRectF>() << QRectF(0, 0, 10, 10));
    m_tester->handleRenderThumbnail(1, QPixmap(10, 10), QVector<QRectF>() << QRectF(0, 0, 10, 10));
    EXPECT_TRUE(m_tester->m_pendingUpdatePages.contains(0));
    EXPECT_TRUE(m_tester->m_pendingUpdatePages.contains(1));
    EXPECT_TRUE(m_tester->m_batchUpdateTimer->isActive());

    QSignalSpy spy(m_tester, &SideBarImageViewModel::dataChanged);
    QTest::qWait(200);  // timer 100ms 单次触发 onBatchUpdateTimer
    EXPECT_GE(spy.count(), 1);
    EXPECT_TRUE(m_tester->m_pendingUpdatePages.isEmpty());
    EXPECT_FALSE(m_tester->m_batchUpdateTimer->isActive());
}

// PMS: https://pms.uniontech.com/bug-view-343541.html  commit: a3cb6cff
TEST_F(TestSideBarImageViewModel, BUG343541_onBatchUpdateTimer_emptyPendingNoCrash)
{
    // 空待更新集合直接批量刷新：提前返回，不崩溃且不发出 dataChanged
    QSignalSpy spy(m_tester, &SideBarImageViewModel::dataChanged);
    m_tester->onBatchUpdateTimer();
    EXPECT_EQ(spy.count(), 0);
    EXPECT_TRUE(m_tester->m_pendingUpdatePages.isEmpty());
}

// PMS: https://pms.uniontech.com/bug-view-343541.html  commit: a3cb6cff
TEST_F(TestSideBarImageViewModel, BUG343541_onBatchUpdateTimer_emitsDataChangedAndClears)
{
    // 批量刷新：待更新页映射为模型索引后按行序发出 dataChanged，集合清空
    m_tester->initModelLst(QList<ImagePageInfo_t>() << ImagePageInfo_t(2) << ImagePageInfo_t(0), true);
    m_tester->m_pendingUpdatePages.insert(0);
    m_tester->m_pendingUpdatePages.insert(2);
    QSignalSpy spy(m_tester, &SideBarImageViewModel::dataChanged);
    m_tester->onBatchUpdateTimer();
    EXPECT_GE(spy.count(), 2);
    EXPECT_TRUE(m_tester->m_pendingUpdatePages.isEmpty());
}

// PMS: https://pms.uniontech.com/bug-view-335473.html  commit: b8625f70
TEST_F(TestSideBarImageViewModel, BUG335473_findItemForAnno_boundaryLookup)
{
    // 注释拖动崩溃修复（移除不安全的 QObject* 日志）：注释查找必须返回边界值不崩溃
    // 列表内空注释命中索引 0；不存在的注释返回 -1；空列表返回 -1
    ImagePageInfo_t info;
    info.pageIndex = 0;
    info.annotation = nullptr;
    m_tester->initModelLst(QList<ImagePageInfo_t>() << info, true);
    EXPECT_EQ(m_tester->findItemForAnno(nullptr), 0);

    DPdfTextAnnot dAnnot;
    deepin_reader::PDFAnnotation annot(&dAnnot);
    EXPECT_EQ(m_tester->findItemForAnno(&annot), -1);

    m_tester->resetData();
    EXPECT_EQ(m_tester->findItemForAnno(nullptr), -1);
}

// PMS: https://pms.uniontech.com/bug-view-335473.html  commit: b8625f70
TEST_F(TestSideBarImageViewModel, BUG335473_removeItemForAnno_idempotentNoCrash)
{
    // 注释拖动崩溃修复：移除存在的注释生效，重复移除（索引 -1 守卫）不崩溃
    DPdfTextAnnot dAnnot;
    deepin_reader::PDFAnnotation annot(&dAnnot);
    ImagePageInfo_t info;
    info.pageIndex = 0;
    info.annotation = &annot;
    m_tester->initModelLst(QList<ImagePageInfo_t>() << info, true);
    EXPECT_EQ(m_tester->m_pagelst.count(), 1);

    m_tester->removeItemForAnno(&annot);
    EXPECT_TRUE(m_tester->m_pagelst.isEmpty());

    m_tester->removeItemForAnno(&annot);
    EXPECT_TRUE(m_tester->m_pagelst.isEmpty());

    m_tester->removeItemForAnno(nullptr);
    EXPECT_TRUE(m_tester->m_pagelst.isEmpty());
}

// PMS: https://pms.uniontech.com/bug-view-335473.html  commit: b8625f70
TEST_F(TestSideBarImageViewModel, BUG335473_removePageIndex_idempotentNoCrash)
{
    // 页码移除：存在的页移除生效，不存在的页仅告警，重复移除不崩溃
    m_tester->initModelLst(QList<ImagePageInfo_t>() << ImagePageInfo_t(0) << ImagePageInfo_t(1), true);
    EXPECT_EQ(m_tester->m_pagelst.count(), 2);

    m_tester->removePageIndex(0);
    EXPECT_EQ(m_tester->m_pagelst.count(), 1);

    m_tester->removePageIndex(99);
    EXPECT_EQ(m_tester->m_pagelst.count(), 1);

    m_tester->removePageIndex(1);
    EXPECT_TRUE(m_tester->m_pagelst.isEmpty());
}

// PMS: https://pms.uniontech.com/bug-view-335473.html  commit: b8625f70
TEST_F(TestSideBarImageViewModel, BUG335473_getModelIndexImageInfo_boundaryGuard)
{
    // 模型索引信息获取：有效索引填充数据，越界索引守卫不修改不崩溃
    m_tester->initModelLst(QList<ImagePageInfo_t>() << ImagePageInfo_t(3), true);

    ImagePageInfo_t info;
    info.pageIndex = -1;
    m_tester->getModelIndexImageInfo(0, info);
    EXPECT_EQ(info.pageIndex, 3);

    ImagePageInfo_t untouched;
    untouched.pageIndex = 42;
    m_tester->getModelIndexImageInfo(-1, untouched);
    EXPECT_EQ(untouched.pageIndex, 42);

    m_tester->getModelIndexImageInfo(m_tester->m_pagelst.size(), untouched);
    EXPECT_EQ(untouched.pageIndex, 42);
}

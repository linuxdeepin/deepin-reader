// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "SideBarImageViewModel.h"
#include "PageRenderThread.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
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

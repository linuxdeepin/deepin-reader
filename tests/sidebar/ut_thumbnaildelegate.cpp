// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThumbnailDelegate.h"
#include "DocSheet.h"
#include "EyeProtectionManager.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>
#include <QPainter>

namespace {

// 未打开文档时渲染器没有页面尺寸，桩掉以得到稳定的缩略图卡片区域
QSizeF pageSizeByIndex_stub(DocSheet *, int)
{
    return QSizeF(210, 297);
}

} // namespace

class UT_ThumbnailDelegate : public ::testing::Test
{
public:
    UT_ThumbnailDelegate() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pView = new SideBarImageListView(m_sheet);
        m_tester = new ThumbnailDelegate(m_pView);
        m_pView->setItemDelegate(m_tester);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_pView;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    ThumbnailDelegate *m_tester = nullptr;
    SideBarImageListView *m_pView = nullptr;
};

TEST_F(UT_ThumbnailDelegate, initTest)
{

}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QPainter *painter = new QPainter();
    QStyleOptionViewItem option;
    m_tester->paint(painter, option, m_pView->getImageModel()->index(0, 0));
    EXPECT_TRUE(m_tester->m_parent == m_pView);
    delete painter;
}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_drawBookMark)
{
    QPainter *painter = new QPainter();
    m_tester->drawBookMark(painter, m_pView->geometry(), true);
    EXPECT_TRUE(m_pView != nullptr);
    delete painter;
}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_drawBookMarkNotVisible)
{
    QPainter *painter = new QPainter();
    m_tester->drawBookMark(painter, m_pView->geometry(), false);
    SUCCEED();
    delete painter;
}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_sizeHint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QStyleOptionViewItem option;
    QModelIndex index = m_pView->getImageModel()->index(0, 0);
    QSize size = m_tester->sizeHint(option, index);
    EXPECT_FALSE(size.isEmpty());
}

// 缩略图外观跟随护眼模式（而非系统深浅主题），与主视图保持一致：
// 无护眼时保持文档原始白底，夜间护眼时反转为深色底
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintFollowsEyeProtectionMode)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    m_pView->getImageModel()->insertPageIndex(0);
    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    m_sheet->setThumbnail(0, whiteThumb);

    const QModelIndex index = m_pView->getImageModel()->index(0, 0);
    ASSERT_TRUE(index.isValid());

    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 240, 300);

    QImage canvas(240, 300, QImage::Format_ARGB32_Premultiplied);

    const auto previousMode = EyeProtectionManager::instance()->mode();
    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Off);
    canvas.fill(Qt::red);
    QPainter offPainter(&canvas);
    m_tester->paint(&offPainter, option, index);
    offPainter.end();
    EXPECT_GT(canvas.pixelColor(120, 150).lightness(), 239);

    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Night);
    canvas.fill(Qt::red);
    QPainter nightPainter(&canvas);
    m_tester->paint(&nightPainter, option, index);
    nightPainter.end();
    EXPECT_LT(canvas.pixelColor(120, 150).lightness(), 32);

    EyeProtectionManager::instance()->setMode(previousMode);
}

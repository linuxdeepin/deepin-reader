// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NotesDelegate.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "DocSheet.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>
#include <QPainter>
#include <QImage>

#include <DGuiApplicationHelper>
#include <dtkgui_global.h>

DGUI_USE_NAMESPACE

namespace {

// 未打开文档时渲染器没有页面尺寸，桩掉以得到稳定的缩略图卡片区域
QSizeF pageSizeByIndex_stub(DocSheet *, int)
{
    return QSizeF(210, 297);
}

// 测试期间屏蔽 dtk 主题持久化，避免 setPaletteType 污染用户配置
class ThemeGuard
{
public:
    explicit ThemeGuard(DGuiApplicationHelper::ColorType type)
    {
        DGuiApplicationHelper::setAttribute(DGuiApplicationHelper::DontSaveApplicationTheme, true);
        m_previous = DGuiApplicationHelper::instance()->themeType();
        DGuiApplicationHelper::instance()->setPaletteType(type);
    }
    ~ThemeGuard()
    {
        if (m_previous != DGuiApplicationHelper::UnknownType)
            DGuiApplicationHelper::instance()->setPaletteType(m_previous);
        else
            DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
        DGuiApplicationHelper::setAttribute(DGuiApplicationHelper::DontSaveApplicationTheme, false);
    }

private:
    DGuiApplicationHelper::ColorType m_previous = DGuiApplicationHelper::UnknownType;
};

} // namespace

class UT_NotesDelegate : public ::testing::Test
{
public:
    UT_NotesDelegate() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pView = new SideBarImageListView(m_sheet);
        m_tester = new NotesDelegate(m_pView);
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
    NotesDelegate *m_tester = nullptr;
    SideBarImageListView *m_pView = nullptr;
};

TEST_F(UT_NotesDelegate, initTest)
{

}

TEST_F(UT_NotesDelegate, UT_NotesDelegate_paint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QPainter *painter = new QPainter;
    QStyleOptionViewItem option;
    m_tester->paint(painter, option, m_pView->getImageModel()->index(0, 0));
    EXPECT_TRUE(m_tester->m_parent == m_pView);
    delete painter;
}

namespace {

// 绘制到离屏画布：页面 (210,297) 按 62×62 等比缩放后绘制在
// (option.rect.x()+10, 垂直居中) 处，返回缩略图卡片内相对坐标 (rx, ry) 处像素
QColor paintPixelAt(UT_NotesDelegate *fixture, const QPixmap &thumb, int rx, int ry)
{
    fixture->m_pView->getImageModel()->insertPageIndex(0);
    fixture->m_sheet->setThumbnail(0, thumb);

    const QModelIndex index = fixture->m_pView->getImageModel()->index(0, 0);
    if (!index.isValid())
        return QColor();

    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 200, 300);

    QImage canvas(200, 300, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::red);   // 红底便于观察是否被绘制覆盖
    QPainter painter(&canvas);
    fixture->m_tester->paint(&painter, option, index);
    painter.end();

    // 缩略图卡片：宽 44(=62*210/297)、高 62，起点 (10, 150-31)
    return canvas.pixelColor(10 + rx, 150 - 31 + ry);
}

} // namespace

// 浅色主题：注释列表保持文档原始白底
TEST_F(UT_NotesDelegate, UT_NotesDelegate_paintLightThemeKeepsWhitePage)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard light(DGuiApplicationHelper::LightType);

    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    const QColor center = paintPixelAt(this, whiteThumb, 22, 31);

    EXPECT_GT(center.lightness(), 239);
}

// 深色主题：白底反转为深色（走 NightFilter 主干滤镜）
TEST_F(UT_NotesDelegate, UT_NotesDelegate_paintDarkThemeInvertsWhitePage)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard dark(DGuiApplicationHelper::DarkType);

    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    const QColor center = paintPixelAt(this, whiteThumb, 22, 31);

    EXPECT_LT(center.lightness(), 32);
}

// 深色主题 + 图片对象蒙版：照片区域保持原始像素，白底反转为深色
TEST_F(UT_NotesDelegate, UT_NotesDelegate_paintDarkThemeWithNightMaskKeepsPhotoPixels)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard dark(DGuiApplicationHelper::DarkType);

    // 左半为纯色照片块(70,70,70)，右半白底；蒙版罩住照片（源缩略图 174 像素坐标）
    QPixmap mixed(174, 246);
    mixed.fill(Qt::white);
    QPainter p(&mixed);
    p.fillRect(0, 0, 87, 246, QColor(70, 70, 70));
    p.end();

    m_pView->getImageModel()->insertPageIndex(0);
    m_sheet->setThumbnail(0, mixed, QVector<QRectF>() << QRectF(0, 0, 87, 246));

    const QModelIndex index = m_pView->getImageModel()->index(0, 0);
    ASSERT_TRUE(index.isValid());

    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 200, 300);

    QImage canvas(200, 300, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::red);
    QPainter painter(&canvas);
    m_tester->paint(&painter, option, index);
    painter.end();

    // 蒙版区域（照片中心，避开缩放采样边界）像素保持不变
    EXPECT_EQ(canvas.pixelColor(10 + 15, 150), QColor(70, 70, 70));
    // 蒙版外白底已反转为深色
    EXPECT_LT(canvas.pixelColor(10 + 40, 150).lightness(), 32);
}

TEST_F(UT_NotesDelegate, UT_NotesDelegate_sizeHint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QStyleOptionViewItem option;
    QModelIndex index = m_pView->getImageModel()->index(0, 0);
    QSize size = m_tester->sizeHint(option, index);
    EXPECT_FALSE(size.isEmpty());
}

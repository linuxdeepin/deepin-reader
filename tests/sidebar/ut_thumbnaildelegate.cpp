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

namespace {

// 绘制到离屏画布，返回画面中心的像素（缩略图卡片正中）
QColor paintCenterPixel(UT_ThumbnailDelegate *fixture, const QPixmap &thumb)
{
    fixture->m_pView->getImageModel()->insertPageIndex(0);
    fixture->m_sheet->setThumbnail(0, thumb);

    const QModelIndex index = fixture->m_pView->getImageModel()->index(0, 0);
    if (!index.isValid())
        return QColor();

    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 240, 300);

    QImage canvas(240, 300, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::red);   // 红底便于观察是否被绘制覆盖
    QPainter painter(&canvas);
    fixture->m_tester->paint(&painter, option, index);
    painter.end();
    return canvas.pixelColor(120, 150);
}

} // namespace

// 缩略图外观只跟随系统深浅主题（不再跟随护眼模式）：
// 浅色主题保持文档原始白底，深色主题反转为深色底
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintLightThemeKeepsWhitePage)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard light(DGuiApplicationHelper::LightType);

    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    const QColor center = paintCenterPixel(this, whiteThumb);

    EXPECT_GT(center.lightness(), 239);
}

// 深色主题下白底反转为深色（与 BookMark/Notes 列表观感一致）
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintDarkThemeInvertsWhitePage)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard dark(DGuiApplicationHelper::DarkType);

    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    const QColor center = paintCenterPixel(this, whiteThumb);

    EXPECT_LT(center.lightness(), 32);
}

// 护眼模式不再影响缩略图：夜间护眼开启时浅色主题下仍绘制原始白底
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintIgnoresEyeProtectionMode)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard light(DGuiApplicationHelper::LightType);

    const EyeProtectionManager::Mode previousMode = EyeProtectionManager::instance()->mode();
    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Night);

    QPixmap whiteThumb(174, 246);
    whiteThumb.fill(Qt::white);
    const QColor center = paintCenterPixel(this, whiteThumb);

    EyeProtectionManager::instance()->setMode(previousMode);

    EXPECT_GT(center.lightness(), 239);
}

// 深色主题 + 图片对象蒙版：蒙版区域(照片)保持原始像素，非蒙版区域(白底)反转为深色
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintDarkThemeWithNightMaskKeepsPhotoPixels)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard dark(DGuiApplicationHelper::DarkType);

    // 左半为纯色照片块(70,70,70)，右半白底；蒙版仅罩住照片，覆盖率 < 70% 不触发扫描页整页反色
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
    option.rect = QRect(0, 0, 240, 300);

    QImage canvas(240, 300, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::red);
    QPainter painter(&canvas);
    m_tester->paint(&painter, option, index);
    painter.end();

    // 蒙版区域像素保持不变（imageDimFactor=1 时 dimPixelImage 不修改像素值）
    EXPECT_EQ(canvas.pixelColor(60, 150), QColor(70, 70, 70));
    // 蒙版外白底已反转为深色
    EXPECT_LT(canvas.pixelColor(180, 150).lightness(), 32);
}

// 深色主题 + 扫描页（图片 bbox 覆盖率 > 70%）：整页反色，蒙版不生效
TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paintDarkThemeScannedPageFullInvert)
{
    Stub s;
    typedef QSizeF(*fptr)(DocSheet *, int);
    fptr pageSizeFunc = (fptr)(&DocSheet::pageSizeByIndex);
    s.set(pageSizeFunc, pageSizeByIndex_stub);

    ThemeGuard dark(DGuiApplicationHelper::DarkType);

    // 整页照片(米黄纸面色)：bbox 覆盖率 100%，按扫描页整页反色，深色像素反转后提亮为白
    QPixmap scanned(174, 246);
    scanned.fill(QColor(60, 60, 60));

    m_pView->getImageModel()->insertPageIndex(0);
    m_sheet->setThumbnail(0, scanned, QVector<QRectF>() << QRectF(0, 0, 174, 246));

    const QModelIndex index = m_pView->getImageModel()->index(0, 0);
    ASSERT_TRUE(index.isValid());

    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 240, 300);

    QImage canvas(240, 300, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::red);
    QPainter painter(&canvas);
    m_tester->paint(&painter, option, index);
    painter.end();

    // 中心点：若蒙版生效则保持 (60,60,60)；整页反色后亮度提升，应明显偏亮
    EXPECT_GT(canvas.pixelColor(120, 150).lightness(), 128);
}

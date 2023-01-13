// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "SlideWidget.h"
#include "SlidePlayWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "MainWindow.h"
#include "ReaderImageThreadPoolManager.h"
#include "stub.h"
#include "addr_pri.h"
#include "Application.h"

#include <gtest/gtest.h>
#include <QTimer>
#include <QTest>
#include <QPropertyAnimation>

namespace {
void ReaderImageThreadPoolManager_addgetDocImageTask_stub(const ReaderImageParam_t &);
void show_stub();
class TestSlideWidget : public ::testing::Test
{
public:
    TestSlideWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        Stub stub;
        stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);
        stub.set(ADDR(QWidget, show), show_stub);

        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }
        m_sheet = new DocSheet(Dr::FileType::PDF, filePath, nullptr);
        m_tester = new SlideWidget(m_sheet);
        m_tester->disconnect();

    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    SlideWidget *m_tester = nullptr;
};

static QString g_funcname;
void showControl_stub()
{
    g_funcname = __FUNCTION__;
}
int pageCount_stub()
{
    return 2;
}
void playImage_stub()
{
    g_funcname = __FUNCTION__;
}
void closeSlide_stub()
{
    g_funcname = __FUNCTION__;
}
void setPlayStatus_stub(bool)
{
    g_funcname = __FUNCTION__;
}
void onPreBtnClicked_stub()
{
    g_funcname = __FUNCTION__;
}
void onNextBtnClicked_stub()
{
    g_funcname = __FUNCTION__;
}
void ReaderImageThreadPoolManager_addgetDocImageTask_stub(const ReaderImageParam_t &)
{
    g_funcname = __FUNCTION__;
}
QPixmap getImageForDocSheet_stub(void *, DocSheet *, int pageIndex)
{
    g_funcname = QString::number(pageIndex);
    return QPixmap();
}

void show_stub()
{

}
}



TEST_F(TestSlideWidget, initTest)
{

}

ACCESS_PRIVATE_FIELD(SlideWidget, int, m_offset);
ACCESS_PRIVATE_FUN(SlideWidget, void(const QVariant &), onImagevalueChanged);
TEST_F(TestSlideWidget, testonImagevalueChanged)
{
    access_private_field::SlideWidgetm_offset(*m_tester) = 0;
    call_private_fun::SlideWidgetonImagevalueChanged(*m_tester, QVariant(123));
    EXPECT_EQ(access_private_field::SlideWidgetm_offset(*m_tester), 123);
}

ACCESS_PRIVATE_FIELD(SlideWidget, bool, m_parentIsDestroyed);
ACCESS_PRIVATE_FUN(SlideWidget, void(), onParentDestroyed);
TEST_F(TestSlideWidget, testonParentDestroyed)
{
    access_private_field::SlideWidgetm_parentIsDestroyed(*m_tester) = false;
    call_private_fun::SlideWidgetonParentDestroyed(*m_tester);
    EXPECT_TRUE(access_private_field::SlideWidgetm_parentIsDestroyed(*m_tester));
}

ACCESS_PRIVATE_FUN(SlideWidget, void(QMouseEvent *), mouseMoveEvent);
TEST_F(TestSlideWidget, testmouseMoveEvent)
{
    Stub s;
    s.set(ADDR(SlidePlayWidget, showControl), showControl_stub);
    QMouseEvent *event = new QMouseEvent(QEvent::MouseMove, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::SlideWidgetmouseMoveEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(g_funcname == "showControl_stub");
    s.reset(ADDR(SlidePlayWidget, showControl));
}

TEST_F(TestSlideWidget, testsetWidgetState)
{
    m_tester->setWidgetState(true);
    m_tester->setWidgetState(false);
}

ACCESS_PRIVATE_FIELD(SlideWidget, int, m_preIndex);
ACCESS_PRIVATE_FIELD(SlideWidget, int, m_curPageIndex);
ACCESS_PRIVATE_FUN(SlideWidget, void(), playImage);
ACCESS_PRIVATE_FUN(SlideWidget, void(), onPreBtnClicked);
TEST_F(TestSlideWidget, testonPreBtnClicked)
{
    auto SlideWidget_playImage = get_private_fun::SlideWidgetplayImage();
    Stub stub;
    stub.set(ADDR(DocSheet, pageCount), pageCount_stub);
    stub.set(SlideWidget_playImage, playImage_stub);

    access_private_field::SlideWidgetm_curPageIndex(*m_tester) = 2;
    call_private_fun::SlideWidgetonPreBtnClicked(*m_tester);
    EXPECT_EQ(access_private_field::SlideWidgetm_preIndex(*m_tester), 2);
    EXPECT_EQ(access_private_field::SlideWidgetm_curPageIndex(*m_tester), 1);
    EXPECT_TRUE(g_funcname == "playImage_stub");

    stub.reset(ADDR(DocSheet, pageCount));
    stub.reset(SlideWidget_playImage);

}

ACCESS_PRIVATE_FIELD(SlideWidget, bool, m_canRestart);
ACCESS_PRIVATE_FIELD(SlideWidget, SlidePlayWidget *, m_slidePlayWidget);
ACCESS_PRIVATE_FIELD(SlidePlayWidget, bool, m_autoPlay);
ACCESS_PRIVATE_FIELD(SlideWidget, QTimer *, m_imageTimer);
ACCESS_PRIVATE_FUN(SlideWidget, void(), onPlayBtnClicked);
TEST_F(TestSlideWidget, testonPlayBtnClicked)
{
    access_private_field::SlideWidgetm_canRestart(*m_tester) = false;
    access_private_field::SlidePlayWidgetm_autoPlay(*access_private_field::SlideWidgetm_slidePlayWidget(*m_tester)) = true;
    call_private_fun::SlideWidgetonPlayBtnClicked(*m_tester);
    access_private_field::SlideWidgetm_imageTimer(*m_tester)->stop();
    EXPECT_TRUE(access_private_field::SlideWidgetm_canRestart(*m_tester));
}

ACCESS_PRIVATE_FUN(SlideWidget, void(), onNextBtnClicked);
TEST_F(TestSlideWidget, testonNextBtnClicked)
{
    auto SlideWidget_playImage = get_private_fun::SlideWidgetplayImage();
    Stub stub;
    stub.set(ADDR(DocSheet, pageCount), pageCount_stub);
    stub.set(SlideWidget_playImage, playImage_stub);

    access_private_field::SlideWidgetm_curPageIndex(*m_tester) = 2;


    call_private_fun::SlideWidgetonNextBtnClicked(*m_tester);
    EXPECT_EQ(access_private_field::SlideWidgetm_preIndex(*m_tester), 2);
    EXPECT_EQ(access_private_field::SlideWidgetm_curPageIndex(*m_tester), 0);
    EXPECT_TRUE(g_funcname == "playImage_stub");

    stub.reset(ADDR(DocSheet, pageCount));
    stub.reset(SlideWidget_playImage);
}

ACCESS_PRIVATE_FUN(SlideWidget, void(), onExitBtnClicked);
TEST_F(TestSlideWidget, testononExitBtnClicked)
{
    Stub stub;
    stub.set(ADDR(DocSheet, closeSlide), closeSlide_stub);
    call_private_fun::SlideWidgetonExitBtnClicked(*m_tester);
    EXPECT_TRUE(g_funcname == "closeSlide_stub");
    stub.reset(ADDR(DocSheet, closeSlide));
}

ACCESS_PRIVATE_FIELD(SlideWidget, bool, m_blefttoright);
ACCESS_PRIVATE_FIELD(SlideWidget, QPropertyAnimation *, m_imageAnimation);
TEST_F(TestSlideWidget, testplayImage)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

    access_private_field::SlideWidgetm_blefttoright(*m_tester) = false;
    access_private_field::SlideWidgetm_imageAnimation(*m_tester)->setStartValue(10);
    call_private_fun::SlideWidgetplayImage(*m_tester);
    EXPECT_FALSE(access_private_field::SlideWidgetm_blefttoright(*m_tester));
    EXPECT_EQ(access_private_field::SlideWidgetm_imageAnimation(*m_tester)->startValue(), 0);
}
ACCESS_PRIVATE_FUN(SlideWidget, void(), onImageShowTimeOut);
TEST_F(TestSlideWidget, testonImageShowTimeOut)
{
    auto SlideWidget_playImage = get_private_fun::SlideWidgetplayImage();
    Stub stub;
    stub.set(ADDR(SlidePlayWidget, setPlayStatus), setPlayStatus_stub);
    stub.set(ADDR(DocSheet, pageCount), pageCount_stub);
    stub.set(SlideWidget_playImage, playImage_stub);

    access_private_field::SlideWidgetm_curPageIndex(*m_tester) = 2;
    access_private_field::SlideWidgetm_canRestart(*m_tester) = false;
    call_private_fun::SlideWidgetonImageShowTimeOut(*m_tester);
    EXPECT_EQ(access_private_field::SlideWidgetm_preIndex(*m_tester), 2);
    EXPECT_EQ(access_private_field::SlideWidgetm_curPageIndex(*m_tester), 1);
    EXPECT_TRUE(g_funcname == "setPlayStatus_stub");

    stub.reset(ADDR(SlidePlayWidget, setPlayStatus));
    stub.reset(ADDR(DocSheet, pageCount));
    stub.reset(SlideWidget_playImage);
}

ACCESS_PRIVATE_FUN(SlideWidget, QPixmap(const QPixmap &), drawImage);
TEST_F(TestSlideWidget, testdrawImage)
{
    QPixmap srcImage("1.png");
    QPixmap pixmap(static_cast<int>(m_tester->width() * dApp->devicePixelRatio()), static_cast<int>(m_tester->height() * dApp->devicePixelRatio()));
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    qreal iwidth = srcImage.width();
    qreal iheight = srcImage.height();
    painter.drawPixmap(static_cast<int>((pixmap.width() - iwidth) * 0.5 / dApp->devicePixelRatio()),
                       static_cast<int>((pixmap.height() - iheight) * 0.5 / dApp->devicePixelRatio()), srcImage);

    EXPECT_EQ(call_private_fun::SlideWidgetdrawImage(*m_tester, srcImage).width(), pixmap.width());
}

ACCESS_PRIVATE_FUN(SlideWidget, void(QMouseEvent *), mousePressEvent);
TEST_F(TestSlideWidget, testmousePressEvent)
{
    Stub s;
    s.set(ADDR(SlidePlayWidget, showControl), showControl_stub);

    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::SlideWidgetmousePressEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(g_funcname == "showControl_stub");
    s.reset(ADDR(SlidePlayWidget, showControl));
}

TEST_F(TestSlideWidget, testhandleKeyPressEvent)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);
    stub.set(ADDR(SlidePlayWidget, setPlayStatus), setPlayStatus_stub);
    auto SlideWidget_onPreBtnClicked = get_private_fun::SlideWidgetonPreBtnClicked();
    auto SlideWidget_onNextBtnClicked = get_private_fun::SlideWidgetonNextBtnClicked();
    stub.set(SlideWidget_onPreBtnClicked, onPreBtnClicked_stub);
    stub.set(SlideWidget_onNextBtnClicked, onNextBtnClicked_stub);

    m_tester->handleKeyPressEvent(Dr::key_space);
    EXPECT_TRUE(g_funcname == "setPlayStatus_stub");
    m_tester->handleKeyPressEvent(Dr::key_left);
    EXPECT_TRUE(g_funcname == "onPreBtnClicked_stub");
    m_tester->handleKeyPressEvent(Dr::key_right);
    EXPECT_TRUE(g_funcname == "onNextBtnClicked_stub");

    stub.reset(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask));
    stub.reset(ADDR(SlidePlayWidget, setPlayStatus));
    stub.reset(SlideWidget_onPreBtnClicked);
    stub.reset(SlideWidget_onNextBtnClicked);
}

ACCESS_PRIVATE_FUN(SlideWidget, void(int), onFetchImage);
TEST_F(TestSlideWidget, testonFetchImage)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);
    call_private_fun::SlideWidgetonFetchImage(*m_tester, 0);
    EXPECT_TRUE(g_funcname == "ReaderImageThreadPoolManager_addgetDocImageTask_stub");
    stub.reset(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask));
}

ACCESS_PRIVATE_FUN(SlideWidget, void(int), onUpdatePageImage);
TEST_F(TestSlideWidget, testonUpdatePageImage)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, getImageForDocSheet), getImageForDocSheet_stub);
    access_private_field::SlideWidgetm_preIndex(*m_tester) = 1;
    access_private_field::SlideWidgetm_curPageIndex(*m_tester) = 0;
    call_private_fun::SlideWidgetonUpdatePageImage(*m_tester, 0);
    EXPECT_TRUE(g_funcname == "0");

}


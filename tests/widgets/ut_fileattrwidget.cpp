// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FileAttrWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <DWidget>
#include <DGuiApplicationHelper>

// ImageWidget is defined in FileAttrWidget.cpp as a top-level class
// deriving from DWidget. We redeclare it here so we can call setPixmap
// and paintEvent directly. Layout must match the real definition.
class ImageWidget : public DWidget
{
public:
    explicit ImageWidget(DWidget *parent) : DWidget(parent) {}
    void setPixmap(const QPixmap &pixmap)
    {
        if (!pixmap.isNull()) {
            m_pixmap = pixmap;
            update();
        }
    }
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QPixmap m_pixmap;
};
namespace  {
class TestFileAttrWidget : public ::testing::Test
{
public:
    TestFileAttrWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new FileAttrWidget();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    FileAttrWidget *m_tester;
};

static QString g_funcname;
static void show_stub()
{
    g_funcname = __FUNCTION__;
}
}

TEST_F(TestFileAttrWidget, initTest)
{

}

TEST_F(TestFileAttrWidget, testsetFileAttr)
{
    int childrencount = m_tester->m_pVBoxLayout->children().count();
    m_tester->setFileAttr(nullptr);
    EXPECT_EQ(m_tester->m_pVBoxLayout->children().count(), childrencount + 0) ;

    QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
    if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
        QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
    }
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, filePath, m_tester);
    m_tester->setFileAttr(sheet);
    EXPECT_EQ(m_tester->m_pVBoxLayout->children().count(), childrencount + 1) ;
}

TEST_F(TestFileAttrWidget, testaddTitleFrame)
{
    int childrencount = m_tester->children().count();
    m_tester->addTitleFrame("111111");
    EXPECT_EQ(m_tester->children().count(), childrencount + 1);
}

TEST_F(TestFileAttrWidget, testshowScreenCenter)
{
    Stub s;
    s.set(ADDR(QWidget, show), show_stub);
    m_tester->showScreenCenter();
    EXPECT_TRUE(g_funcname == "show_stub");
}

TEST_F(TestFileAttrWidget, testsizeModeChanged)
{
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    EXPECT_TRUE(m_tester->m_pVBoxLayout != nullptr);
}

TEST_F(TestFileAttrWidget, testImageWidgetPaintEvent)
{
    // frameImage is the private ImageWidget* member created in initImageLabel().
    ASSERT_NE(m_tester->frameImage, nullptr);

    // Provide a non-null pixmap so paintEvent performs drawing.
    QPixmap pix(20, 20);
    pix.fill(Qt::red);
    static_cast<ImageWidget *>(m_tester->frameImage)->setPixmap(pix);

    // Force a paint event via the public repaint() slot.
    m_tester->frameImage->repaint();

    // Also call paintEvent directly through -fno-access-control to ensure
    // the uncovered function gets executed.
    QRect rect(0, 0, m_tester->frameImage->width(), m_tester->frameImage->height());
    QPaintEvent event(rect);
    static_cast<ImageWidget *>(m_tester->frameImage)->paintEvent(&event);
    SUCCEED();
}

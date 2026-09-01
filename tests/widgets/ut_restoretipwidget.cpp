// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RestoreTipWidget.h"

#include <QAbstractButton>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSignalSpy>

#include <gtest/gtest.h>

/********测试RestoreTipWidget***********/
class UT_RestoreTipWidget : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    QWidget *m_parent = nullptr;
    RestoreTipWidget *m_tester = nullptr;
};

void UT_RestoreTipWidget::SetUp()
{
    m_parent = new QWidget;
    m_parent->resize(800, 600);
    m_parent->show();
    m_tester = new RestoreTipWidget(m_parent);
}

void UT_RestoreTipWidget::TearDown()
{
    delete m_parent;
}

TEST_F(UT_RestoreTipWidget, initTest)
{
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_showTip_001)
{
    m_tester->showTip();
    EXPECT_FALSE(m_tester->isHidden());
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_paintEvent_001)
{
    QPaintEvent event(QRect(m_tester->rect()));
    m_tester->paintEvent(&event);
    EXPECT_FALSE(m_tester->grab().isNull());
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_resizeEvent_001)
{
    QResizeEvent event(QSize(400, 40), m_tester->size());
    m_tester->resizeEvent(&event);      // 隐藏分支
    m_tester->showTip();
    m_tester->resizeEvent(&event);      // 可见分支 -> reposition
    SUCCEED();
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_onFontChanged_001)
{
    m_tester->onFontChanged();
    SUCCEED();
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_jumpBtnClicked_001)
{
    m_tester->showTip();
    QSignalSpy spy(m_tester, SIGNAL(sigJumpToFirstPage()));
    QAbstractButton *btn = m_tester->findChild<QAbstractButton *>("JumpBtn");
    ASSERT_NE(btn, nullptr);
    btn->click();
    EXPECT_EQ(spy.count(), 1);
    EXPECT_TRUE(m_tester->isHidden());
}

TEST_F(UT_RestoreTipWidget, UT_RestoreTipWidget_closeBtnClicked_001)
{
    m_tester->showTip();
    QAbstractButton *btn = m_tester->findChild<QAbstractButton *>("CloseBtn");
    ASSERT_NE(btn, nullptr);
    btn->click();
    EXPECT_TRUE(m_tester->isHidden());
}

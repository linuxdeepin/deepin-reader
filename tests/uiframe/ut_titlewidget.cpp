// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TitleWidget.h"
#include "DocSheet.h"
#include "ScaleWidget.h"
#include "MsgHeader.h"

#include "stub.h"

#include <DIconButton>
#include <DLineEdit>
#include <DGuiApplicationHelper>

#include <gtest/gtest.h>
#include <QTest>
#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

namespace {
void setSidebarVisible_stub(bool, bool)
{
}
}

class TestTitleWidget : public ::testing::Test
{
public:
    TestTitleWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_parent = new DWidget();
        m_tester = new TitleWidget(m_parent);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_parent;
    }

protected:
    DWidget *m_parent = nullptr;
    TitleWidget *m_tester = nullptr;
};

TEST_F(TestTitleWidget, initTest)
{
}

TEST_F(TestTitleWidget, testSetControlEnabled)
{
    m_tester->setControlEnabled(true);
    SUCCEED();
}

TEST_F(TestTitleWidget, testSetControlEnabled_false)
{
    m_tester->setControlEnabled(false);
    SUCCEED();
}

TEST_F(TestTitleWidget, testOnThumbnailBtnClicked_noSheet)
{
    m_tester->onThumbnailBtnClicked(true);
    SUCCEED();
}

TEST_F(TestTitleWidget, testOnThumbnailBtnClicked_withSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, strPath, nullptr);
    m_tester->m_curSheet = sheet;

    Stub s;
    s.set(ADDR(DocSheet, setSidebarVisible), setSidebarVisible_stub);

    m_tester->onThumbnailBtnClicked(true);
    SUCCEED();

    delete sheet;
}

TEST_F(TestTitleWidget, testKeyPressEvent)
{
    QKeyEvent upEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    m_tester->keyPressEvent(&upEvent);

    QKeyEvent downEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    m_tester->keyPressEvent(&downEvent);

    QKeyEvent otherEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
    m_tester->keyPressEvent(&otherEvent);
    SUCCEED();
}

TEST_F(TestTitleWidget, testSetBtnDisable)
{
    m_tester->setBtnDisable(true);
    EXPECT_FALSE(m_tester->m_pThumbnailBtn->isEnabled());

    m_tester->setBtnDisable(false);
    EXPECT_TRUE(m_tester->m_pThumbnailBtn->isEnabled());
}

TEST_F(TestTitleWidget, testSizeModeChangedLambda)
{
    // Trigger the lambda registered on sizeModeChanged
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    SUCCEED();
}

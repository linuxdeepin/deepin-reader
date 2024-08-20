// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TransparentTextEdit.h"
#include "ut_common.h"
#include "stub.h"
#include "addr_pri.h"
#include <QSignalSpy>
#include <QDebug>
#include <QMimeData>
#include <QTest>

#include <gtest/gtest.h>

class UT_TransparentTextEdit : public ::testing::Test
{
public:
    UT_TransparentTextEdit(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new TransparentTextEdit();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    TransparentTextEdit *m_tester;
};

TEST_F(UT_TransparentTextEdit, initTest)
{

}

ACCESS_PRIVATE_FIELD(TransparentTextEdit, int, m_nMaxContantLen);
ACCESS_PRIVATE_FUN(TransparentTextEdit, void(), slotTextEditMaxContantNum);
TEST_F(UT_TransparentTextEdit, UT_TransparentTextEdit_slotTextEditMaxContantNum)
{
    access_private_field::TransparentTextEditm_nMaxContantLen(*m_tester) = 3;
    m_tester->setPlainText("123456");
    call_private_fun::TransparentTextEditslotTextEditMaxContantNum(*m_tester);
    EXPECT_TRUE(m_tester->toPlainText() == "123");
}

ACCESS_PRIVATE_FUN(TransparentTextEdit, void(QPaintEvent *event), paintEvent);
TEST_F(UT_TransparentTextEdit, UT_TransparentTextEdit_paintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    call_private_fun::TransparentTextEditpaintEvent(*m_tester, &paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

ACCESS_PRIVATE_FUN(TransparentTextEdit, void(const QMimeData *source), insertFromMimeData);
TEST_F(UT_TransparentTextEdit, UT_TransparentTextEdit_insertFromMimeData)
{
    QMimeData *source = new QMimeData();
    source->setText("123");
    call_private_fun::TransparentTextEditinsertFromMimeData(*m_tester, source);
    EXPECT_TRUE(m_tester->toPlainText() == "123");
    delete source;
}

TEST_F(UT_TransparentTextEdit, UT_TransparentTextEdit_keyPressEvent)
{
    Stub stub;
    UTCommon::stub_DMenu_exec(stub);
    QTest::keyPress(m_tester, Qt::Key_M, Qt::AltModifier);
}

ACCESS_PRIVATE_FUN(TransparentTextEdit, void(QFocusEvent *event), focusOutEvent);
TEST_F(UT_TransparentTextEdit, UT_TransparentTextEdit_focusOutEvent)
{
    QSignalSpy spy(m_tester, SIGNAL(sigCloseNoteWidget(bool)));
    QFocusEvent *event = new QFocusEvent(QEvent::FocusOut);
    call_private_fun::TransparentTextEditfocusOutEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(spy.count() == 1);
}

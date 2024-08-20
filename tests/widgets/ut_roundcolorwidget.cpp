// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoundColorWidget.h"

#include <QSignalSpy>

#include <gtest/gtest.h>
#include "stub.h"
#include "addr_pri.h"
class UT_RoundColorWidget : public ::testing::Test
{
public:
    UT_RoundColorWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new RoundColorWidget(Qt::red);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    RoundColorWidget *m_tester;
};

ACCESS_PRIVATE_FIELD(RoundColorWidget, bool, m_isSelected);

TEST_F(UT_RoundColorWidget, initTest)
{

}

TEST_F(UT_RoundColorWidget, UT_RoundColorWidget_setSelected_001)
{
    m_tester->setSelected(true);
    bool &RoundColorWidgetm_isSelected = access_private_field::RoundColorWidgetm_isSelected(*m_tester);
    EXPECT_TRUE(RoundColorWidgetm_isSelected == true);
}

TEST_F(UT_RoundColorWidget, UT_RoundColorWidget_setSelected_002)
{
    m_tester->setSelected(false);
    bool &RoundColorWidgetm_isSelected = access_private_field::RoundColorWidgetm_isSelected(*m_tester);
    EXPECT_TRUE(RoundColorWidgetm_isSelected == false);
}

ACCESS_PRIVATE_FUN(RoundColorWidget, void(QMouseEvent *event), mousePressEvent);
TEST_F(UT_RoundColorWidget, UT_RoundColorWidget_mousePressEvent)
{
    QSignalSpy spy(m_tester, SIGNAL(clicked()));
    access_private_field::RoundColorWidgetm_isSelected(*m_tester) = false;
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::RoundColorWidgetmousePressEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(spy.count() == 1);
}

ACCESS_PRIVATE_FUN(RoundColorWidget, void(QPaintEvent *event), paintEvent);
TEST_F(UT_RoundColorWidget, UT_RoundColorWidget_paintEvent)
{
    access_private_field::RoundColorWidgetm_isSelected(*m_tester) = true;
    QPaintEvent paint(QRect(m_tester->rect()));
    call_private_fun::RoundColorWidgetpaintEvent(*m_tester, &paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

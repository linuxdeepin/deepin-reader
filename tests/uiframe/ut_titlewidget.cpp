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

// ==================== PMS 批次 2 补强 ====================
// BUG44136 (sev2): 标题栏控件 Tab 焦点顺序错乱/回车无效。
// 回归意图: TitleWidget 在构造后焦点策略与 Tab 遍历链可用,
//          且按键事件处理不崩溃。

TEST_F(TestTitleWidget, BUG44136_titleWidgetFocusChainUsable)
{
    // 当前 Tab 顺序机制: 构造时把有序控件链写入父级 orderlist 属性, 并逐个设为 TabFocus
    QVariant varOrder = m_parent->property("orderlist");
    EXPECT_TRUE(varOrder.isValid());
    QList<QWidget *> orderList = varOrder.value<QList<QWidget *>>();
    // 有序链槽位数: 缩略图/减/缩放框/增/opt/最小/最大/关闭 = 8
    // (全屏按钮仅在全屏模式下由 DTitlebar 提供, 裸父级 fixture 中为空不追加)
    ASSERT_EQ(orderList.count(), 8);
    // 相对顺序约束: 缩略图 → 减 → 增 (缩放框在裸 fixture 下可能尚未实例化)
    int idxThumbnail = -1, idxDec = -1, idxInc = -1;
    int nonNullCount = 0;
    for (int i = 0; i < orderList.count(); ++i) {
        QWidget *w = orderList.at(i);
        if (w == nullptr)
            continue;
        ++nonNullCount;
        EXPECT_TRUE(w->focusPolicy() == Qt::TabFocus) << "noTabFocus at idx=" << i;
        if (w->objectName() == "SP_DecreaseElement")
            idxDec = i;
        else if (w->objectName() == "SP_IncreaseElement")
            idxInc = i;
        else if (qobject_cast<QAbstractButton *>(w) && w == orderList.first())
            idxThumbnail = i;
    }
    EXPECT_GE(nonNullCount, 3);
    EXPECT_GE(idxThumbnail, 0);
    EXPECT_GT(idxDec, idxThumbnail);
    EXPECT_GT(idxInc, idxDec);

    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    m_tester->keyPressEvent(&tabEvent);
    SUCCEED();
}

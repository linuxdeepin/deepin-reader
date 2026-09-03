// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// 临时复现测试：多标签页打开时 ScaleWidget/ScaleMenu 是否指向当前标签页文档

#include "Central.h"
#include "CentralDocPage.h"
#include "TitleWidget.h"
#include "ScaleWidget.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "ut_defines.h"

#include <QStackedLayout>
#include <QTimer>
#include <QEventLoop>

#include <gtest/gtest.h>

static void processEvents(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

// 模拟用户点击标签栏第 idx 个标签（与鼠标点击一致：DTabBar 内部即 setCurrentIndex）
static void clickTab(CentralDocPage *docPage, int idx)
{
    DocTabBar *bar = docPage->m_tabBar;
    ASSERT_NE(bar, nullptr);
    ASSERT_TRUE(idx >= 0 && idx < bar->count());
    bar->setCurrentIndex(idx);   // 与用户点击等价，触发 currentChanged
}

static DocSheet *scaleMenuTarget(Central *central)
{
    return central->titleWidget()->m_pSw->m_sheet.data();
}

// 场景1：一次打开多个文档，等全部加载完，切到第一个标签
TEST(TestScaleMenuRepro, batchOpenThenSwitch)
{
    QWidget w;
    Central central(&w);

    central.addFileAsync(QString(UTSOURCEDIR) + "/files/1.pdf");
    central.addFileAsync(QString(UTSOURCEDIR) + "/files/2.pdf");
    central.addFileAsync(QString(UTSOURCEDIR) + "/files/3.pdf");
    processEvents(1500);

    CentralDocPage *docPage = central.docPage();
    QList<DocSheet *> sheets = docPage->getSheets();
    ASSERT_EQ(sheets.size(), 3);

    clickTab(docPage, 0);
    processEvents(300);
    EXPECT_EQ(docPage->getCurSheet(), sheets[0]);
    qDebug() << "[case1] after click tab0, scale target =" << (scaleMenuTarget(&central) ? scaleMenuTarget(&central)->filePath() : "null");
    EXPECT_EQ(scaleMenuTarget(&central), sheets[0]);
}

// 场景2：打开多个文档，加载未完成时立即切换标签（更贴近真实操作）
TEST(TestScaleMenuRepro, switchWhileLoading)
{
    QWidget w;
    Central central(&w);

    central.addFileAsync(QString(UTSOURCEDIR) + "/files/1.pdf");
    central.addFileAsync(QString(UTSOURCEDIR) + "/files/2.pdf");
    // 不等待，立即点击第一个标签（模拟加载过程中切换）
    processEvents(5);          // 让 1ms 延时的 setCurrentIndex 触发
    clickTab(central.docPage(), 0);

    // 等待所有文档异步加载完成
    processEvents(2000);

    CentralDocPage *docPage = central.docPage();
    QList<DocSheet *> sheets = docPage->getSheets();
    ASSERT_EQ(sheets.size(), 2);
    EXPECT_EQ(docPage->getCurSheet(), sheets[0]);
    qDebug() << "[case2] after load complete, cur =" << docPage->getCurSheet()->filePath()
             << ", scale target =" << (scaleMenuTarget(&central) ? scaleMenuTarget(&central)->filePath() : "null");
    EXPECT_EQ(scaleMenuTarget(&central), sheets[0]);
}

// 场景3：逐个打开文档（每次打开后等加载完成再打开下一个），再切换标签
TEST(TestScaleMenuRepro, sequentialOpenThenSwitch)
{
    QWidget w;
    Central central(&w);

    central.addFileAsync(QString(UTSOURCEDIR) + "/files/1.pdf");
    processEvents(1000);
    central.addFileAsync(QString(UTSOURCEDIR) + "/files/2.pdf");
    processEvents(1000);

    CentralDocPage *docPage = central.docPage();
    QList<DocSheet *> sheets = docPage->getSheets();
    ASSERT_EQ(sheets.size(), 2);

    clickTab(docPage, 0);
    processEvents(300);
    EXPECT_EQ(docPage->getCurSheet(), sheets[0]);
    qDebug() << "[case3] after click tab0, cur=" << docPage->getCurSheet()->filePath()
             << ", scale target =" << (scaleMenuTarget(&central) ? scaleMenuTarget(&central)->filePath() : "null");
    EXPECT_EQ(scaleMenuTarget(&central), sheets[0]);
}

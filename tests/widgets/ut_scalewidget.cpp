// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ScaleWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"
#include "addr_pri.h"

#include <gtest/gtest.h>
#include <QTest>

namespace {
class TestScaleWidget : public ::testing::Test
{
public:
    TestScaleWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester = new ScaleWidget();
        m_tester->setSheet(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ScaleWidget *m_tester;
};

static QString g_funcname;
void zoomout_stub()
{
    g_funcname = __FUNCTION__;
}

void zoomin_stub()
{
    g_funcname = __FUNCTION__;
}
}

QAction *menu_exec_stub(const QPoint &, QAction *)
{
    g_funcname = __FUNCTION__;
    return nullptr;
}

TEST_F(TestScaleWidget, initTest)
{

}

TEST_F(TestScaleWidget, testonPrevScale)
{
    Stub s;
    s.set(ADDR(DocSheet, zoomout), zoomout_stub);
    m_tester->onPrevScale();
    EXPECT_TRUE(g_funcname == "zoomout_stub");
}

TEST_F(TestScaleWidget, testonNextScale)
{
    Stub s;
    s.set(ADDR(DocSheet, zoomin), zoomin_stub);
    m_tester->onNextScale();
    EXPECT_TRUE(g_funcname == "zoomin_stub");
}

ACCESS_PRIVATE_FIELD(ScaleWidget, Dtk::Widget::DLineEdit *, m_lineEdit);
ACCESS_PRIVATE_FIELD(ScaleWidget, QPointer<DocSheet>, m_sheet);
ACCESS_PRIVATE_FIELD(DocSheet, SheetOperation, m_operation);
ACCESS_PRIVATE_FUN(ScaleWidget, void(), onReturnPressed);
TEST_F(TestScaleWidget, testonReturnPressed)
{
    access_private_field::ScaleWidgetm_lineEdit(*m_tester)->setText("30%");
    call_private_fun::ScaleWidgetonReturnPressed(*m_tester);
    EXPECT_EQ(access_private_field::DocSheetm_operation(*access_private_field::ScaleWidgetm_sheet(*m_tester)).scaleFactor, 0.3);
}

ACCESS_PRIVATE_FUN(ScaleWidget, void(), onArrowBtnlicked);
TEST_F(TestScaleWidget, testonArrowBtnlicked)
{
    Stub stub;
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), menu_exec_stub);
    call_private_fun::ScaleWidgetonArrowBtnlicked(*m_tester);
    stub.reset((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec));
    EXPECT_TRUE(g_funcname == "menu_exec_stub");
}

ACCESS_PRIVATE_FUN(ScaleWidget, void(), onEditFinished);
TEST_F(TestScaleWidget, testonEditFinished)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleWidgetm_sheet(*m_tester)).scaleFactor = 0.2;
    call_private_fun::ScaleWidgetonEditFinished(*m_tester);
    EXPECT_TRUE(access_private_field::ScaleWidgetm_lineEdit(*m_tester)->text() == "20%");
}

TEST_F(TestScaleWidget, testclear)
{
    access_private_field::ScaleWidgetm_lineEdit(*m_tester)->setText("50%");
    m_tester->clear();
    EXPECT_TRUE(access_private_field::ScaleWidgetm_lineEdit(*m_tester)->text().isEmpty());
}

/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScaleWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"

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

TEST_F(TestScaleWidget, testonReturnPressed)
{
    m_tester->m_lineEdit->setText("30%");
    m_tester->onReturnPressed();
    EXPECT_EQ(m_tester->m_sheet->m_operation.scaleFactor, 0.3);
}

TEST_F(TestScaleWidget, testonArrowBtnlicked)
{
    Stub stub;
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), menu_exec_stub);
    m_tester->onArrowBtnlicked();
    EXPECT_TRUE(g_funcname == "menu_exec_stub");
}

TEST_F(TestScaleWidget, testonEditFinished)
{
    m_tester->m_sheet->m_operation.scaleFactor = 0.2;
    m_tester->onEditFinished();
    EXPECT_TRUE(m_tester->m_lineEdit->text() == "20%");
}

TEST_F(TestScaleWidget, testclear)
{
    m_tester->m_lineEdit->setText("50%");
    m_tester->clear();
    EXPECT_TRUE(m_tester->m_lineEdit->text().isEmpty());
}

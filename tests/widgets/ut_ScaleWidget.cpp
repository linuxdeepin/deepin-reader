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


QAction *menu_exec_stub(const QPoint &, QAction *)
{
    return nullptr;
}

TEST_F(TestScaleWidget, initTest)
{

}

TEST_F(TestScaleWidget, testonPrevScale)
{
    m_tester->onPrevScale();
}

TEST_F(TestScaleWidget, testonNextScale)
{
    m_tester->onNextScale();
}

TEST_F(TestScaleWidget, testonReturnPressed)
{
    m_tester->onReturnPressed();
}

TEST_F(TestScaleWidget, testonArrowBtnlicked)
{
    Stub stub;
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), menu_exec_stub);
    m_tester->onArrowBtnlicked();
}

TEST_F(TestScaleWidget, testonEditFinished)
{
    m_tester->onEditFinished();
}

TEST_F(TestScaleWidget, testclear)
{
    m_tester->clear();
}

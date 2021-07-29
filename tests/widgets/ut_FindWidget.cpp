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

#include "FindWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

class TestFindWidget : public ::testing::Test
{
public:
    TestFindWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new FindWidget();
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester->setDocSheet(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    FindWidget *m_tester;
};

void DocSheet_startSearch_stub(const QString &)
{
    return ;
}

TEST_F(TestFindWidget, initTest)
{

}

TEST_F(TestFindWidget, testsetSearchEditFocus)
{
    m_tester->setSearchEditFocus();
}

TEST_F(TestFindWidget, testonSearchStop)
{
    m_tester->onSearchStop();
}

TEST_F(TestFindWidget, testonSearchStart)
{
    Stub stub;
    stub.set(ADDR(DocSheet, startSearch), DocSheet_startSearch_stub);
    m_tester->m_pSearchEdit->setText("123");
    m_tester->onSearchStart();
}

TEST_F(TestFindWidget, testslotFindNextBtnClicked)
{
    m_tester->slotFindNextBtnClicked();
}

TEST_F(TestFindWidget, testslotFindPrevBtnClicked)
{
    m_tester->slotFindPrevBtnClicked();
}

TEST_F(TestFindWidget, testonTextChanged)
{
    m_tester->onTextChanged();
}

TEST_F(TestFindWidget, testsetEditAlert)
{
    m_tester->setEditAlert(1);
}

TEST_F(TestFindWidget, testonCloseBtnClicked)
{
    m_tester->onCloseBtnClicked();
}

TEST_F(TestFindWidget, testkeyPressEvent)
{
    QTest::keyPress(m_tester, Qt::Key_Up);
    QTest::keyPress(m_tester, Qt::Key_Delete);
}

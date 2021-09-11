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

#include "HandleMenu.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>

class TestHandleMenu : public ::testing::Test
{
public:
    TestHandleMenu(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new HandleMenu();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    HandleMenu *m_tester;
};

TEST_F(TestHandleMenu, initTest)
{

}

TEST_F(TestHandleMenu, testonHandTool)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
    if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
        QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
    }
    m_tester->m_docSheet = new DocSheet(Dr::FileType::PDF, filePath, m_tester);
    QSignalSpy spy(m_tester->m_docSheet, SIGNAL(sigOperationChanged(DocSheet *)));
    m_tester->onHandTool();
    EXPECT_TRUE(m_tester->m_docSheet->m_operation.mouseShape == Dr::MouseShapeHand);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(TestHandleMenu, testonSelectText)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
    if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
        QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
    }
    m_tester->m_docSheet = new DocSheet(Dr::FileType::PDF, filePath, m_tester);
    QSignalSpy spy(m_tester->m_docSheet, SIGNAL(sigOperationChanged(DocSheet *)));
    m_tester->onSelectText();
    EXPECT_TRUE(m_tester->m_docSheet->m_operation.mouseShape == Dr::MouseShapeNormal);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(TestHandleMenu, testreadCurDocParam)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
    if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
        QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
    }
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, filePath, m_tester);
    m_tester->readCurDocParam(sheet);
    EXPECT_TRUE(m_tester->m_docSheet == sheet);
    EXPECT_TRUE(m_tester->m_textAction->isChecked());
    delete sheet;
}

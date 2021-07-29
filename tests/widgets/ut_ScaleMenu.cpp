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

#include "ScaleMenu.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include <gtest/gtest.h>
#include <QTest>

class TestScaleMenu : public ::testing::Test
{
public:
    TestScaleMenu(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester = new ScaleMenu();
        m_tester->readCurDocParam(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ScaleMenu *m_tester;
};

TEST_F(TestScaleMenu, initTest)
{

}

TEST_F(TestScaleMenu, testonTwoPage)
{
    m_tester->onTwoPage();
}

TEST_F(TestScaleMenu, testonFiteH)
{
    m_tester->onFiteH();
}

TEST_F(TestScaleMenu, testonFiteW)
{
    m_tester->onFiteW();
}

TEST_F(TestScaleMenu, testonDefaultPage)
{
    m_tester->onDefaultPage();
}

TEST_F(TestScaleMenu, testonFitPage)
{
    m_tester->onFitPage();
}

TEST_F(TestScaleMenu, testonScaleFactor)
{
    m_tester->onScaleFactor();
}

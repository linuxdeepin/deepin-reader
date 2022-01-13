/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
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

#include "Global.h"
#include "stub.h"

//#include <QWidget>
#include <QMimeType>

#include <gtest/gtest.h>
using namespace Dr;
/********测试DBusObject***********/
class TestGlobal : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();
};

void TestGlobal::SetUp()
{

}

void TestGlobal::TearDown()
{

}

/***********桩函数*************/
QString name_stub()
{
    return "application/postscript";
}

QString name_stub2()
{
    return "application/x-ole-storage";
}

/***********测试用例***********/
TEST_F(TestGlobal, UT_Global_fileType_001)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    EXPECT_TRUE(fileType(strPath) == PDF);
}

TEST_F(TestGlobal, UT_Global_fileType_002)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.djvu";
    EXPECT_TRUE(fileType(strPath) == DJVU);
}

TEST_F(TestGlobal, UT_Global_fileType_003)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pptx";
    EXPECT_TRUE(fileType(strPath) == PPTX);
}

TEST_F(TestGlobal, UT_Global_fileType_004)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.docx";
    EXPECT_TRUE(fileType(strPath) == DOCX);
}

TEST_F(TestGlobal, UT_Global_fileType_005)
{
    Stub s;
    s.set(ADDR(QMimeType, name), name_stub);

    EXPECT_TRUE(fileType("1.ps") == PS);
}

TEST_F(TestGlobal, UT_Global_fileType_006)
{
    Stub s;
    s.set(ADDR(QMimeType, name), name_stub2);

    EXPECT_TRUE(fileType("1.docx") == DOCX);
}

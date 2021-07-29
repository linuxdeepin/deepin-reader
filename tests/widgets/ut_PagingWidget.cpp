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

#include "PagingWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

class TestPagingWidget : public ::testing::Test
{
public:
    TestPagingWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester = new PagingWidget(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    PagingWidget *m_tester;
};

TEST_F(TestPagingWidget, initTest)
{

}

TEST_F(TestPagingWidget, testslotUpdateTheme)
{
    m_tester->slotUpdateTheme();
}

TEST_F(TestPagingWidget, testresizeEvent)
{
    QResizeEvent *event = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    m_tester->resizeEvent(event);
    delete event;
}

TEST_F(TestPagingWidget, testsetIndex)
{
    m_tester->setIndex(0);
}

TEST_F(TestPagingWidget, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
}

TEST_F(TestPagingWidget, testSlotJumpPageLineEditReturnPressed)
{
    m_tester->SlotJumpPageLineEditReturnPressed();
}

TEST_F(TestPagingWidget, testonEditFinished)
{
    m_tester->onEditFinished();
}

TEST_F(TestPagingWidget, testnormalChangePage)
{
    m_tester->normalChangePage();
}

TEST_F(TestPagingWidget, testpageNumberJump)
{
    m_tester->pageNumberJump();
}

TEST_F(TestPagingWidget, testslotPrePageBtnClicked)
{
    m_tester->slotPrePageBtnClicked();
}

TEST_F(TestPagingWidget, testslotNextPageBtnClicked)
{
    m_tester->slotNextPageBtnClicked();
}

TEST_F(TestPagingWidget, testsetTabOrderWidget)
{
    QList<QWidget *> tabWidgetlst;
    m_tester->setTabOrderWidget(tabWidgetlst);
}

TEST_F(TestPagingWidget, testonFuncThreadFinished)
{
    m_tester->onFuncThreadFinished();
}

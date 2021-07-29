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

#include "SlideWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "MainWindow.h"
#include "ReaderImageThreadPoolManager.h"
#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

void ReaderImageThreadPoolManager_addgetDocImageTask_stub(const ReaderImageParam_t &)
{
    return;
}

class TestSlideWidget : public ::testing::Test
{
public:
    TestSlideWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        Stub stub;
        stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester = new SlideWidget(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();

    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SlideWidget *m_tester;
};


TEST_F(TestSlideWidget, initTest)
{

}

TEST_F(TestSlideWidget, testonImagevalueChanged)
{
    m_tester->onImagevalueChanged(QVariant());
}

TEST_F(TestSlideWidget, testonParentDestroyed)
{
    m_tester->onParentDestroyed();
}

TEST_F(TestSlideWidget, testmouseMoveEvent)
{
    QMouseEvent *event = new QMouseEvent(QEvent::MouseMove, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_tester->mouseMoveEvent(event);
    delete event;
}

TEST_F(TestSlideWidget, testsetWidgetState)
{
    m_tester->setWidgetState(true);
    m_tester->setWidgetState(false);
}

TEST_F(TestSlideWidget, testonPreBtnClicked)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

    m_tester->onPreBtnClicked();
}

TEST_F(TestSlideWidget, testonPlayBtnClicked)
{
    m_tester->onPlayBtnClicked();
}

TEST_F(TestSlideWidget, testonNextBtnClicked)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

    m_tester->onNextBtnClicked();
}

TEST_F(TestSlideWidget, testononExitBtnClicked)
{
    m_tester->onExitBtnClicked();
}

TEST_F(TestSlideWidget, testplayImage)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

    m_tester->playImage();
}

TEST_F(TestSlideWidget, testonImageShowTimeOut)
{
    m_tester->onImageShowTimeOut();
}

TEST_F(TestSlideWidget, testdrawImage)
{
    m_tester->drawImage(QPixmap("1.png"));
}

TEST_F(TestSlideWidget, testmousePressEvent)
{
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_tester->mousePressEvent(event);
    delete event;
}

TEST_F(TestSlideWidget, testhandleKeyPressEvent)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);

    m_tester->handleKeyPressEvent(Dr::key_space);
    m_tester->handleKeyPressEvent(Dr::key_left);
    m_tester->handleKeyPressEvent(Dr::key_right);
}

TEST_F(TestSlideWidget, testonFetchImage)
{
    Stub stub;
    stub.set(ADDR(ReaderImageThreadPoolManager, addgetDocImageTask), ReaderImageThreadPoolManager_addgetDocImageTask_stub);
    m_tester->onFetchImage(0);
}

TEST_F(TestSlideWidget, testonUpdatePageImage)
{
    m_tester->onUpdatePageImage(0);
}


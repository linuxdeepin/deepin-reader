/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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

#include "MainWindow.h"
#include "DocSheet.h"
#include "Application.h"
#include "SheetRenderer.h"

#include <QTimer>
#include <QCloseEvent>
#include <QHoverEvent>
#include <QResizeEvent>

#include "stub.h"
#include <gtest/gtest.h>

static void openFileAsync_stub(const QString &)
{
    return;
}

class TestMainWindow : public ::testing::Test
{
public:
    TestMainWindow(): m_tester(nullptr), m_tester1(nullptr) {}

public:
    virtual void SetUp()
    {
        Stub stub;
        stub.set(ADDR(SheetRenderer, openFileAsync), openFileAsync_stub);

        m_tester = new MainWindow(QStringList());

        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_tester1 = new MainWindow(QStringList() << strPath);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_tester1;
    }

protected:
    MainWindow *m_tester;
    MainWindow *m_tester1;
};

TEST_F(TestMainWindow, initTest)
{

}

TEST_F(TestMainWindow, testaddSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
    m_tester->addSheet(sheet);
}

TEST_F(TestMainWindow, testhasSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->hasSheet(sheet);
}

TEST_F(TestMainWindow, testactivateSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->activateSheet(sheet);
}

TEST_F(TestMainWindow, testhandleClose)
{
    m_tester->handleClose(false);
    m_tester->handleClose(true);
}

TEST_F(TestMainWindow, testaddFile)
{
    m_tester->addFile("");
}

TEST_F(TestMainWindow, testcloseEvent)
{
    QCloseEvent *event = new QCloseEvent;
    m_tester->closeEvent(event);
    delete event;
}

TEST_F(TestMainWindow, testeventFilter)
{
    QHoverEvent *mouseEvent = new QHoverEvent(QEvent::HoverMove, QPointF(100, 100), QPointF(0, 0));
    m_tester->eventFilter(m_tester, mouseEvent);
    delete mouseEvent;

    QEvent *pEvent = new QEvent(QEvent::WindowStateChange);
    m_tester->eventFilter(m_tester, pEvent);
    delete pEvent;

    QResizeEvent *pResizeEvent = new QResizeEvent(QSize(200, 200), QSize(500, 500));
    m_tester->eventFilter(m_tester, pResizeEvent);
    delete pResizeEvent;
}

TEST_F(TestMainWindow, testsetDocTabBarWidget)
{
    m_tester->setDocTabBarWidget(nullptr);
}

TEST_F(TestMainWindow, testonTitleAniFinished)
{
    m_tester->setDocTabBarWidget(nullptr);
    m_tester->onTitleAniFinished();
}

TEST_F(TestMainWindow, testonMainWindowFull)
{
    m_tester->onMainWindowFull();

    m_tester->setDocTabBarWidget(new QWidget(m_tester));
    m_tester->onMainWindowFull();
}

TEST_F(TestMainWindow, testonMainWindowExitFull)
{
    m_tester1->setDocTabBarWidget(nullptr);
    m_tester1->m_lastWindowState = Qt::WindowFullScreen;
    m_tester1->onMainWindowExitFull();
}

TEST_F(TestMainWindow, testresizeFullTitleWidget)
{
    m_tester1->resizeFullTitleWidget();

    m_tester->setDocTabBarWidget(new QWidget(m_tester));
    m_tester1->resizeFullTitleWidget();
}

TEST_F(TestMainWindow, testwindowContainSheet)
{
    MainWindow *pMainwindow = new MainWindow(QStringList());
    m_tester->m_list << pMainwindow;

    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
    m_tester->windowContainSheet(sheet);
}

TEST_F(TestMainWindow, testallowCreateWindow)
{
    m_tester->allowCreateWindow();
}

TEST_F(TestMainWindow, testactivateSheetIfExist)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    m_tester->activateSheetIfExist("");
    m_tester->activateSheetIfExist(strPath);
}

TEST_F(TestMainWindow, testcreateWindow)
{
    MainWindow *pMainwindow = new MainWindow(QStringList());
    m_tester->m_list << pMainwindow;

    m_tester->createWindow(QStringList());
}

TEST_F(TestMainWindow, testcreateWindow1)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);

    m_tester->createWindow(sheet);
}

TEST_F(TestMainWindow, testshowDefaultSize)
{
    m_tester->showDefaultSize();
}

TEST_F(TestMainWindow, testonDelayInit)
{
    m_tester->onDelayInit();
}

TEST_F(TestMainWindow, testonUpdateTitleLabelRect)
{
    m_tester->onUpdateTitleLabelRect();
}

TEST_F(TestMainWindow, testupdateOrderWidgets)
{
    m_tester->updateOrderWidgets(QList<QWidget *>());
}

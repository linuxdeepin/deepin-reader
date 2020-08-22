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
#include "ut_mainwindow.h"
#include "MainWindow.h"

Ut_MainWindow::Ut_MainWindow()
{
}

void Ut_MainWindow::SetUp()
{
    m_mainWindow = MainWindow::createWindow();
    m_mainWindow->show();
}

void Ut_MainWindow::TearDown()
{
}

#ifdef UT_MAINWINDOW_TEST
TEST_F(Ut_MainWindow, MainWindowTest)
{
    EXPECT_EQ(m_mainWindow->hasSheet(nullptr), false);
}
#endif

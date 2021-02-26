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

#define private public
#define protected public

#include "MainWindow.h"
#include "DocSheet.h"
#include "Application.h"

#undef private
#undef protected

Ut_MainWindow::Ut_MainWindow()
{
}

void Ut_MainWindow::SetUp()
{
    ut_application::SetUp();
}

void Ut_MainWindow::TearDown()
{
}

TEST_F(Ut_MainWindow, MainWindowTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "MainWindowTest"), nullptr);
    DocSheet *sheet2 = new DocSheet(Dr::PDF, filePath(UT_FILE_TEST_FILE_2, "MainWindowTest"), nullptr);

    MainWindow *mainWindow = MainWindow::createWindow(sheet);
    MainWindow *mainWindow_empty = MainWindow::createWindow();
    MainWindow *mainWindow_muti = MainWindow::createWindow(QStringList() << filePath(UT_FILE_TEST_FILE_1, "MainWindowTest") << filePath(UT_FILE_DJVU, "MainWindowTest"));
    mainWindow_muti->addSheet(sheet2);
    mainWindow_muti->addFile(filePath(UT_FILE_TEST_FILE_3, "MainWindowTest"));

    EXPECT_EQ(mainWindow_empty->hasSheet(nullptr), false);
    EXPECT_EQ(mainWindow_muti->hasSheet(sheet), false);

    EXPECT_EQ(mainWindow->allowCreateWindow(), true);
    EXPECT_FALSE(mainWindow->windowContainSheet(nullptr));
    EXPECT_TRUE(mainWindow->windowContainSheet(sheet));

    //mainWindow->onShortCut(Dr::key_ctrl_shift_slash);
    mainWindow->showDefaultSize();
    EXPECT_TRUE(mainWindow->hasSheet(sheet));
    mainWindow->activateSheetIfExist(filePath(UT_FILE_PDF, "MainWindowTest"));
    mainWindow->updateOrderWidgets(QList<QWidget *>());
    mainWindow->activateSheet(sheet);
    mainWindow_muti->activateSheet(sheet);
    mainWindow->onMainWindowFull();
    mainWindow->onMainWindowExitFull();
    mainWindow->resizeFullTitleWidget();
    mainWindow->onUpdateTitleLabelRect();

    EXPECT_TRUE(mainWindow->closeWithSave());
    EXPECT_TRUE(mainWindow_empty->closeWithSave());
    EXPECT_TRUE(mainWindow_muti->closeWithSave());

    exec();
}

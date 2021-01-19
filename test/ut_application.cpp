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
#include "ut_application.h"

#define private public
#define protected public

#include "DocSheet.h"
#include "Application.h"
#include "MainWindow.h"

#undef private
#undef protected


#include <DApplicationSettings>
#include <DLog>
#include <QDebug>
#include <QTimer>

ut_application::ut_application()
{

}

ut_application::~ut_application()
{

}

void ut_application::SetUp()
{
    Test::SetUp();

    int argc = 0;
    char arg = ' ';
    char *argPointer = &arg;
    char **argPointerPointer = &argPointer;
    a = new Application(argc, argPointerPointer);

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)
}

void ut_application::TearDown()
{
}

void ut_application::exec(int secs)
{
    QTimer::singleShot(1000 * secs, [this]() {
        a->exit();
    });

    a->exec();
}

QString ut_application::filePath(QString fileName, QString dirName)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + dirName + "/" + fileName;
    if (!QFile(filePath).exists() && QFile(":/files/" + fileName).exists()) {
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + dirName);
        QFile(":/files/" + fileName).copy(filePath);
    }

    return filePath;
}

#ifdef UT_APPLICATION_TEST
TEST_F(ut_application, ApplicationTest)
{
    a->showAnnotTextWidgetSig();
    exec();
}
#endif

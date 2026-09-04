// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Application.h"
#include "ut_defines.h"
#include "Application.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE

#if defined(CMAKE_SAFETYTEST_ARG_ON)
#include <sanitizer/asan_interface.h>
#endif

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");

    // 单元测试与业务数据隔离：将 QStandardPaths 重定向到测试目录(~/.qttest)，
    QStandardPaths::setTestModeEnabled(true);

    Application application(argc, argv);

    // 清理上次测试残留的数据库，保证用例从干净状态开始
    const QString utDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(utDataDir);
    QFile::remove(QDir(utDataDir).filePath("user.db"));
    QFile::remove(QDir(utDataDir).filePath("user.db-wal"));
    QFile::remove(QDir(utDataDir).filePath("user.db-shm"));

    ::testing::InitGoogleTest(&argc, argv);

    int c = RUN_ALL_TESTS();

#if defined(CMAKE_SAFETYTEST_ARG_ON)
    __sanitizer_set_report_path("asan.log");
#endif

    return c;
}

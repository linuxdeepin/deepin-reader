// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Application.h"
#include "ut_defines.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <memory>

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE

#if defined(CMAKE_SAFETYTEST_ARG_ON)
#include <sanitizer/asan_interface.h>
#endif

// 测试目标固定以 -fprofile-arcs -ftest-coverage 编译；全量测试下第三方库
// (djvulibre/pdfium/GL) 退出阶段的全局析构会 SIGSEGV/SIGABRT，且 gcov 的
// atexit 回写排在析构之后随之丢失。测试 XML 落盘后手动 dump 覆盖率并
// _exit 跳过退出阶段析构，保证 .gcda 数据与退出码稳定
// 强引用 __gcov_dump：weak 引用不会让链接器从 libgcov.a 拉入成员，
// 导致 dump 永远为 null；tests 目标固定 -fprofile-arcs，符号必然可解析
extern "C" void __gcov_dump(void);

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    // 单元测试与业务数据隔离：将 QStandardPaths 重定向到测试目录(~/.qttest)，
    QStandardPaths::setTestModeEnabled(true);

    // Application/QApplication 必须存活到 main 结束（QPrinter 构造依赖
    // QCoreApplication 存在），用 static unique_ptr 延长生命周期
    static std::unique_ptr<QCoreApplication> app = std::make_unique<Application>(argc, argv);

    // 清理上次测试残留的数据库，保证用例从干净状态开始
    const QString utDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(utDataDir);
    QFile::remove(QDir(utDataDir).filePath("user.db"));
    QFile::remove(QDir(utDataDir).filePath("user.db-wal"));
    QFile::remove(QDir(utDataDir).filePath("user.db-shm"));

    ::testing::InitGoogleTest(&argc, argv);

    int c = RUN_ALL_TESTS();

    if (__gcov_dump)
        __gcov_dump();
    fflush(stdout);
    fflush(stderr);
    ::_exit(c);
}

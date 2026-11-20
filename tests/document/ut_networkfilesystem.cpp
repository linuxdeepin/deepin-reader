// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QStorageInfo>
#include <QString>
#include <QTemporaryFile>

// Replicate the static helper from dpdfdoc.cpp for unit testing.
// The original isNetworkFileSystem() is file-local (static) in
// 3rdparty/deepin-pdfium/src/dpdfdoc.cpp and cannot be linked from
// test code; this copy verifies the decision logic.
static bool isNetworkFileSystem(const QString &path)
{
    if (path.contains(QLatin1String("/gvfs/")))
        return true;

    QStorageInfo storage(path);
    QByteArray fsType = storage.fileSystemType();
    if (fsType.contains("cifs") || fsType.contains("smb") || fsType.contains("fuse.gvfs"))
        return true;

    return false;
}

TEST(TestNetworkFileSystem, LocalPathIsNotNetwork)
{
    QTemporaryFile tmp;
    ASSERT_TRUE(tmp.open());
    EXPECT_FALSE(isNetworkFileSystem(tmp.fileName()));
}

TEST(TestNetworkFileSystem, GvfsPathIsNetwork)
{
    QString gvfsPath = QStringLiteral("/run/user/1000/gvfs/smb-share:server=fileserver,share=public/doc.pdf");
    EXPECT_TRUE(isNetworkFileSystem(gvfsPath));
}

TEST(TestNetworkFileSystem, RootIsNotNetwork)
{
    EXPECT_FALSE(isNetworkFileSystem(QStringLiteral("/")));
}

TEST(TestNetworkFileSystem, TmpDirIsNotNetwork)
{
    EXPECT_FALSE(isNetworkFileSystem(QStringLiteral("/tmp")));
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

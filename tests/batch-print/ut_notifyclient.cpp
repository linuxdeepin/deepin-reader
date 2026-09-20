// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notifyclient.h"
#include "errormessages.h"

#include <gtest/gtest.h>
#include <QDBusArgument>
#include <QDBusMessage>
#include <QGuiApplication>
#include <QVariant>

static int argc = 1;
static char *argv[] = { const_cast<char *>("ut_notifyclient"), nullptr };

class NotifyClientTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static QGuiApplication *app = nullptr;
        if (!app)
            app = new QGuiApplication(argc, argv);
    }
};

// Notification body parameter assertions: verify buildBody produces correct
// content for each scenario. This verifies the fix for review item 2.

TEST_F(NotifyClientTest, AllSuccessBodyContainsSuccess) {
    QString body = NotifyClient::buildBody(3, 3, QStringList());
    EXPECT_FALSE(body.isEmpty());
    EXPECT_FALSE(body.contains(QStringLiteral("failed")));
}

TEST_F(NotifyClientTest, AllFailedBodyContainsFailed) {
    QStringList failed;
    failed << QStringLiteral("file1.pdf") << QStringLiteral("file2.pdf");
    QString body = NotifyClient::buildBody(2, 0, failed);
    EXPECT_FALSE(body.isEmpty());
    EXPECT_TRUE(body.contains(QStringLiteral("failed")));
    EXPECT_TRUE(body.contains(QStringLiteral("file1.pdf")));
    EXPECT_TRUE(body.contains(QStringLiteral("file2.pdf")));
}

TEST_F(NotifyClientTest, PartialSuccessBodyContainsBoth) {
    QStringList failed;
    failed << QStringLiteral("file2.docx");
    QString body = NotifyClient::buildBody(3, 2, failed);
    EXPECT_FALSE(body.isEmpty());
    EXPECT_TRUE(body.contains(QStringLiteral("file2.docx")));
}

TEST_F(NotifyClientTest, ManyFailedFilesTruncated) {
    QStringList failed;
    for (int i = 0; i < 10; ++i)
        failed << QStringLiteral("file%1.pdf").arg(i);
    QString body = NotifyClient::buildBody(10, 0, failed);
    // Should contain first 5 files
    EXPECT_TRUE(body.contains(QStringLiteral("file0.pdf")));
    EXPECT_TRUE(body.contains(QStringLiteral("file4.pdf")));
    // Should contain truncation notice
    EXPECT_TRUE(body.contains(QStringLiteral("5 more")));
}

TEST_F(NotifyClientTest, EmptyListBody) {
    QString body = NotifyClient::buildBody(0, 0, QStringList());
    EXPECT_FALSE(body.isEmpty());
}

TEST_F(NotifyClientTest, NotifyResultAllSuccessDoesNotCrash) {
    NotifyClient::notifyResult(3, 3, QStringList());
    SUCCEED();
}

TEST_F(NotifyClientTest, NotifyResultAllFailedDoesNotCrash) {
    QStringList failed;
    failed << QStringLiteral("file1.pdf") << QStringLiteral("file2.pdf");
    NotifyClient::notifyResult(2, 0, failed);
    SUCCEED();
}

TEST_F(NotifyClientTest, NotifyResultPartialSuccessDoesNotCrash) {
    QStringList failed;
    failed << QStringLiteral("file2.docx");
    NotifyClient::notifyResult(3, 2, failed);
    SUCCEED();
}

TEST_F(NotifyClientTest, NotifyResultManyFailedDoesNotCrash) {
    QStringList failed;
    for (int i = 0; i < 10; ++i)
        failed << QStringLiteral("file%1.pdf").arg(i);
    NotifyClient::notifyResult(10, 0, failed);
    SUCCEED();
}

TEST_F(NotifyClientTest, NotifyResultEmptyListDoesNotCrash) {
    NotifyClient::notifyResult(0, 0, QStringList());
    SUCCEED();
}

// Regression test: buildBody with total==0 and an error message in failedFiles
// must NOT surface the error text — buildBody is designed for result statistics
// only, not environment errors. This is exactly the gap that let the original
// bug slip through: every existing test only exercised success/failure branches.
TEST_F(NotifyClientTest, BuildBodyTotalZeroDoesNotContainErrorMessage) {
    QStringList errList;
    errList << ErrorMessages::cupsUnavailable();
    QString body = NotifyClient::buildBody(0, 0, errList);
    EXPECT_FALSE(body.isEmpty());
    // The error message must NOT appear in buildBody output for total==0.
    EXPECT_FALSE(body.contains(ErrorMessages::cupsUnavailable()));
    // buildBody(0,0,...) falls into the "All 0 file(s) printed successfully." branch.
    EXPECT_TRUE(body.contains(QStringLiteral("successfully")));
}

// Environment error path uses notifyError, not buildBody/notifyResult.
TEST_F(NotifyClientTest, NotifyErrorDoesNotCrash) {
    NotifyClient::notifyError(ErrorMessages::cupsUnavailable());
    SUCCEED();
}

TEST_F(NotifyClientTest, NotifyErrorNoDefaultPrinterDoesNotCrash) {
    NotifyClient::notifyError(ErrorMessages::noDefaultPrinter());
    SUCCEED();
}

// Regression test for the "no error prompt without printer" bug:
// NotifyClient used to pass a QVariantList to QDBusInterface::call(), which
// marshalled the whole argument list as ONE 'av' parameter. The deepin
// notification daemon rejects signature 'av' with UnknownMethod, so every
// notification silently fell back to invisible stderr. The message must
// instead carry 8 separate arguments (signature susssasa{sv}i).
TEST_F(NotifyClientTest, BuildNotifyMessageHasEightSeparateArguments) {
    QDBusMessage msg = NotifyClient::buildNotifyMessage(
        QStringLiteral("test body"));
    QVariantList args = msg.arguments();
    ASSERT_EQ(args.size(), 8);
}

TEST_F(NotifyClientTest, BuildNotifyMessageArgumentTypesMatchSpec) {
    QDBusMessage msg = NotifyClient::buildNotifyMessage(
        QStringLiteral("test body"));
    QVariantList args = msg.arguments();
    ASSERT_EQ(args.size(), 8);

    EXPECT_EQ(args.at(0).metaType().id(), QMetaType::QString);
    EXPECT_EQ(args.at(1).metaType().id(), QMetaType::UInt);
    EXPECT_EQ(args.at(2).metaType().id(), QMetaType::QString);
    EXPECT_EQ(args.at(3).metaType().id(), QMetaType::QString);
    EXPECT_EQ(args.at(4).metaType().id(), QMetaType::QString);
    EXPECT_EQ(args.at(5).metaType().id(), QMetaType::QStringList);
    EXPECT_EQ(args.at(6).metaType().id(), QMetaType::QVariantMap);
    EXPECT_EQ(args.at(7).metaType().id(), QMetaType::Int);
    EXPECT_FALSE(args.at(0).toString().isEmpty());
    EXPECT_EQ(args.at(4).toString(), QStringLiteral("test body"));
    EXPECT_EQ(args.at(7).toInt(), -1);
}

TEST_F(NotifyClientTest, BuildNotifyMessageTargetsNotificationService) {
    QDBusMessage msg = NotifyClient::buildNotifyMessage(
        QStringLiteral("test body"));
    EXPECT_EQ(msg.service(),
              QStringLiteral("org.freedesktop.Notifications"));
    EXPECT_EQ(msg.path(),
              QStringLiteral("/org/freedesktop/Notifications"));
    EXPECT_EQ(msg.interface(),
              QStringLiteral("org.freedesktop.Notifications"));
    EXPECT_EQ(msg.member(), QStringLiteral("Notify"));
}

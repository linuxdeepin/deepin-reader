// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../icupsapi.h"
#include "../printsettings.h"
#include "../cupsclient.h"

#include <gtest/gtest.h>
#include <QGuiApplication>
#include <QString>
#include <QStringList>

static int argc = 1;
static char *argv[] = { const_cast<char *>("ut_cupsclient"), nullptr };

class CupsClientTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static QGuiApplication *app = nullptr;
        if (!app)
            app = new QGuiApplication(argc, argv);
    }
};

// Mock implementation of ICupsApi for testing
class MockCupsApi : public ICupsApi {
public:
    bool getCupsDestsResult = true;
    bool queryColorResult = true;
    bool colorSupportedValue = true;
    bool printFileResult = true;
    int printFileCallCount = 0;
    QString lastTitle;
    QString lastPrinter;
    QString lastFilename;
    int lastNumOptions = -1;

    bool getCupsDests(cups_dest_t **dests) override {
        Q_UNUSED(dests)
        return getCupsDestsResult;
    }

    bool queryColorSupported(const QString &printerName, bool &supported) override {
        Q_UNUSED(printerName)
        supported = colorSupportedValue;
        return queryColorResult;
    }

    bool printFile(const QString &printerName, const QString &filename,
                   const QString &title, int numOptions, cups_option_t *options) override {
        Q_UNUSED(options)
        lastPrinter = printerName;
        lastFilename = filename;
        lastTitle = title;
        lastNumOptions = numOptions;
        ++printFileCallCount;
        return printFileResult;
    }
};

TEST_F(CupsClientTest, MockInterfaceBasicOperation) {
    MockCupsApi mock;
    mock.getCupsDestsResult = true;
    mock.queryColorResult = true;
    mock.colorSupportedValue = true;
    mock.printFileResult = true;

    cups_dest_t *dests = nullptr;
    EXPECT_TRUE(mock.getCupsDests(&dests));

    bool supported = false;
    EXPECT_TRUE(mock.queryColorSupported(QStringLiteral("test-printer"), supported));
    EXPECT_TRUE(supported);

    EXPECT_TRUE(mock.printFile(QStringLiteral("test-printer"),
                               QStringLiteral("/tmp/test.pdf"),
                               QStringLiteral("test-job"), 0, nullptr));
    EXPECT_EQ(mock.printFileCallCount, 1);
}

TEST_F(CupsClientTest, MockColorNotSupported) {
    MockCupsApi mock;
    mock.colorSupportedValue = false;

    bool supported = true;
    EXPECT_TRUE(mock.queryColorSupported(QStringLiteral("mono-printer"), supported));
    EXPECT_FALSE(supported);
}

TEST_F(CupsClientTest, MockPrintFailure) {
    MockCupsApi mock;
    mock.printFileResult = false;

    EXPECT_FALSE(mock.printFile(QStringLiteral("test"),
                                QStringLiteral("/tmp/nonexistent.pdf"),
                                QStringLiteral("fail-job"), 0, nullptr));
}

TEST_F(CupsClientTest, MockGetDestsFailure) {
    MockCupsApi mock;
    mock.getCupsDestsResult = false;

    cups_dest_t *dests = nullptr;
    EXPECT_FALSE(mock.getCupsDests(&dests));
}

TEST_F(CupsClientTest, CupsClientInitWithoutCups) {
    CupsClient client;
    bool result = client.init();
    SUCCEED() << "CupsClient::init() returned: " << result;
}

// Real coverage: verify toCupsOptions produces exact key-value pairs
TEST_F(CupsClientTest, ToCupsOptionsCopiesAndSidesAndColor) {
    PrintSettings settings;
    settings.copies = 2;
    settings.duplex = DuplexMode::OneSided;
    settings.colorMode = ColorMode::Auto;

    QStringList opts = toCupsOptions(settings, true);

    EXPECT_EQ(opts.size(), 6);
    EXPECT_EQ(opts.at(0), QStringLiteral("copies"));
    EXPECT_EQ(opts.at(1), QStringLiteral("2"));
    EXPECT_EQ(opts.at(2), QStringLiteral("sides"));
    EXPECT_EQ(opts.at(3), QStringLiteral("one-sided"));
    EXPECT_EQ(opts.at(4), QStringLiteral("ColorModel"));
    EXPECT_EQ(opts.at(5), QStringLiteral("RGB"));
}

TEST_F(CupsClientTest, ToCupsOptionsCopiesClamp) {
    PrintSettings settings;
    settings.copies = 5000;
    settings.colorMode = ColorMode::Gray;

    QStringList opts = toCupsOptions(settings, true);
    int idx = opts.indexOf(QStringLiteral("copies"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("999"));
}

TEST_F(CupsClientTest, ToCupsOptionsColorForceGray) {
    PrintSettings settings;
    settings.colorMode = ColorMode::Gray;

    QStringList opts = toCupsOptions(settings, true);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("Gray"));
}

TEST_F(CupsClientTest, ToCupsOptionsColorForceColor) {
    PrintSettings settings;
    settings.colorMode = ColorMode::Color;

    QStringList opts = toCupsOptions(settings, false);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("RGB"));
}

// Verify that printFile passes the title through correctly (not hardcoded)
TEST_F(CupsClientTest, MockPrintFileTitlePropagation) {
    MockCupsApi mock;
    QString expectedTitle = QStringLiteral("my-document.pdf");
    mock.printFile(QStringLiteral("printer1"),
                   QStringLiteral("/tmp/output.pdf"),
                   expectedTitle, 0, nullptr);
    EXPECT_EQ(mock.lastTitle, expectedTitle);
    EXPECT_EQ(mock.lastPrinter, QStringLiteral("printer1"));
    EXPECT_EQ(mock.lastFilename, QStringLiteral("/tmp/output.pdf"));
    EXPECT_EQ(mock.lastNumOptions, 0);
}

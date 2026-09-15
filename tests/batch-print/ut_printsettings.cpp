// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "printsettings.h"

#include <gtest/gtest.h>
#include <QGuiApplication>

#include <QStringList>

static int argc = 1;
static char *argv[] = { const_cast<char *>("ut_printsettings"), nullptr };

class PrintSettingsTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static QGuiApplication *app = nullptr;
        if (!app)
            app = new QGuiApplication(argc, argv);
    }
};

TEST_F(PrintSettingsTest, CopiesMapping) {
    PrintSettings s;
    s.copies = 3;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("copies"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("3"));
}

TEST_F(PrintSettingsTest, CopiesClampHigh) {
    PrintSettings s;
    s.copies = 9999;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("copies"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("999"));
}

TEST_F(PrintSettingsTest, CopiesClampLow) {
    PrintSettings s;
    s.copies = 0;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("copies"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("1"));
}

TEST_F(PrintSettingsTest, CopiesClampNegative) {
    PrintSettings s;
    s.copies = -5;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("copies"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("1"));
}

TEST_F(PrintSettingsTest, SidesOneSided) {
    PrintSettings s;
    s.duplex = DuplexMode::OneSided;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("sides"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("one-sided"));
}

TEST_F(PrintSettingsTest, SidesLongEdge) {
    PrintSettings s;
    s.duplex = DuplexMode::LongEdge;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("sides"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("two-sided-long-edge"));
}

TEST_F(PrintSettingsTest, SidesShortEdge) {
    PrintSettings s;
    s.duplex = DuplexMode::ShortEdge;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("sides"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("two-sided-short-edge"));
}

TEST_F(PrintSettingsTest, ColorModelAutoWithColorSupport) {
    PrintSettings s;
    s.colorMode = ColorMode::Auto;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("RGB"));
}

TEST_F(PrintSettingsTest, ColorModelAutoWithoutColorSupport) {
    PrintSettings s;
    s.colorMode = ColorMode::Auto;
    QStringList opts = toCupsOptions(s, false);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("Gray"));
}

TEST_F(PrintSettingsTest, ColorModelColor) {
    PrintSettings s;
    s.colorMode = ColorMode::Color;
    QStringList opts = toCupsOptions(s, false);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("RGB"));
}

TEST_F(PrintSettingsTest, ColorModelGray) {
    PrintSettings s;
    s.colorMode = ColorMode::Gray;
    QStringList opts = toCupsOptions(s, true);
    int idx = opts.indexOf(QStringLiteral("ColorModel"));
    ASSERT_GE(idx, 0);
    EXPECT_EQ(opts.at(idx + 1), QStringLiteral("Gray"));
}

TEST_F(PrintSettingsTest, DefaultValues) {
    PrintSettings s;
    EXPECT_EQ(s.copies, 1);
    EXPECT_EQ(s.duplex, DuplexMode::OneSided);
    EXPECT_EQ(s.colorMode, ColorMode::Auto);
    EXPECT_EQ(s.orientation, Orientation::Auto);
    EXPECT_EQ(s.watermarkEnabled, false);
    EXPECT_EQ(s.watermarkAngle, 30);
    EXPECT_EQ(s.watermarkSize, 100);
    EXPECT_EQ(s.watermarkOpacity, 30);
    EXPECT_EQ(s.watermarkLayout, WatermarkLayout::Center);
    EXPECT_EQ(s.watermarkTextType, WatermarkTextType::Confidential);
}

// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "formatconverter.h"

#include <gtest/gtest.h>
#include <QGuiApplication>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

static int argc = 1;
static char *argv[] = { const_cast<char *>("ut_formatconverter"), nullptr };

class FormatConverterTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static QGuiApplication *app = nullptr;
        if (!app)
            app = new QGuiApplication(argc, argv);
    }
};

TEST_F(FormatConverterTest, NonexistentFileFails) {
    QString outputPdfPath;
    QString errorMsg;
    EXPECT_FALSE(FormatConverter::convertToPdf(
        QStringLiteral("/nonexistent/file.pdf"),
        QStringLiteral("/tmp"), outputPdfPath, errorMsg));
    EXPECT_FALSE(errorMsg.isEmpty());
}

TEST_F(FormatConverterTest, PdfPassthroughReturnsOriginalPath) {
    QTemporaryFile tempFile(QStringLiteral("XXXXXX.pdf"));
    ASSERT_TRUE(tempFile.open());
    {
        QTextStream stream(&tempFile);
        stream << "%PDF-1.0\n1 0 obj\n<< /Type /Catalog >>\nendobj\n";
        stream << "trailer\n<< /Root 1 0 R >>\n%%EOF\n";
    }
    tempFile.close();
    ASSERT_TRUE(QFileInfo::exists(tempFile.fileName()));

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString outputPdfPath;
    QString errorMsg;
    bool result = FormatConverter::convertToPdf(tempFile.fileName(), tempDir.path(),
                                                 outputPdfPath, errorMsg);
    EXPECT_TRUE(result);
    EXPECT_EQ(outputPdfPath, tempFile.fileName());
}

TEST_F(FormatConverterTest, UnknownFormatFails) {
    QTemporaryFile tempFile(QStringLiteral("XXXXXX.unknown"));
    ASSERT_TRUE(tempFile.open());
    tempFile.write("dummy content");
    tempFile.close();

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString outputPdfPath;
    QString errorMsg;
    EXPECT_FALSE(FormatConverter::convertToPdf(tempFile.fileName(), tempDir.path(),
                                                outputPdfPath, errorMsg));
    EXPECT_FALSE(errorMsg.isEmpty());
}

// DJVU geometry conversion: pixel dimensions at a given DPI must convert to
// correct millimeter page sizes. This verifies the fix for review item 1.
TEST_F(FormatConverterTest, DjvuGeometryConversion300dpi) {
    // A US Letter page at 300 dpi: 2550 x 3300 pixels
    QSizeF mm = FormatConverter::computePageSizeMm(2550.0, 3300.0, 300);
    // US Letter = 215.9 x 279.4 mm
    EXPECT_NEAR(mm.width(), 215.9, 0.5);
    EXPECT_NEAR(mm.height(), 279.4, 0.5);
}

TEST_F(FormatConverterTest, DjvuGeometryConversion72dpi) {
    // At 72 dpi, pixels == points, so 595 x 842 px → 210 x 297 mm (A4)
    QSizeF mm = FormatConverter::computePageSizeMm(595.0, 842.0, 72);
    EXPECT_NEAR(mm.width(), 210.0, 0.5);
    EXPECT_NEAR(mm.height(), 297.2, 0.5);
}

TEST_F(FormatConverterTest, DjvuGeometryConversionFallbackDpi) {
    // dpi <= 0 should fall back to 300
    QSizeF mm = FormatConverter::computePageSizeMm(2550.0, 3300.0, 0);
    EXPECT_NEAR(mm.width(), 215.9, 0.5);
    EXPECT_NEAR(mm.height(), 279.4, 0.5);
}

TEST_F(FormatConverterTest, DjvuGeometryConversionInvalidSize) {
    // Zero or negative dimensions should fall back to A4
    QSizeF mm = FormatConverter::computePageSizeMm(0.0, 0.0, 300);
    EXPECT_EQ(mm.width(), 210.0);
    EXPECT_EQ(mm.height(), 297.0);
}

TEST_F(FormatConverterTest, DjvuGeometryConversionNonA4Landscape) {
    // A3 landscape at 300 dpi: 4961 x 3508 pixels
    QSizeF mm = FormatConverter::computePageSizeMm(4961.0, 3508.0, 300);
    // A3 = 297 x 420 mm (landscape)
    EXPECT_NEAR(mm.width(), 419.9, 1.0);
    EXPECT_NEAR(mm.height(), 296.9, 1.0);
}

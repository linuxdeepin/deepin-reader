// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OfdModel.h"

#ifdef OFD_SUPPORT_ENABLED

#include "Model.h"
#include "ut_defines.h"

#include <QFile>
#include <QImage>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using namespace deepin_reader;

namespace {

QString ofdFilePath()
{
    QString path = QString(UTSOURCEDIR) + "/files/normal.ofd";
    if (QFile(path).exists())
        return path;

    path = QCoreApplication::applicationDirPath() + "/files/normal.ofd";
    return path;
}

bool hasOfdFile()
{
    return QFile(ofdFilePath()).exists();
}

} // namespace

class TestOfdModel : public ::testing::Test
{
public:
    void SetUp() override
    {
        if (!hasOfdFile())
            GTEST_SKIP() << "normal.ofd not available, skipping OFD model tests";

        Document::Error error = Document::NoError;
        m_doc.reset(OfdDocument::loadDocument(ofdFilePath(), error));
        ASSERT_NE(m_doc, nullptr);
        EXPECT_EQ(error, Document::NoError);
    }

    void TearDown() override {}

    QString m_path;
    std::unique_ptr<OfdDocument> m_doc;
};

TEST_F(TestOfdModel, loadDocument)
{
    EXPECT_GT(m_doc->pageCount(), 0);
}

TEST_F(TestOfdModel, pageSize)
{
    Page *page = m_doc->page(0);
    ASSERT_NE(page, nullptr);

    const QSizeF size = page->sizeF();
    EXPECT_GT(size.width(), 0.0);
    EXPECT_GT(size.height(), 0.0);

    delete page;
}

TEST_F(TestOfdModel, pageOutOfRange)
{
    EXPECT_EQ(m_doc->page(-1), nullptr);
    EXPECT_EQ(m_doc->page(m_doc->pageCount()), nullptr);
}

TEST_F(TestOfdModel, renderPage)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QImage image = page->render(400, 400);
    ASSERT_FALSE(image.isNull());
    EXPECT_GT(image.width(), 0);
    EXPECT_GT(image.height(), 0);

    // 渲染结果不应是纯白页面
    bool hasContentPixel = false;
    for (int y = 0; y < image.height() && !hasContentPixel; ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) < 240 || qGreen(pixel) < 240 || qBlue(pixel) < 240) {
                hasContentPixel = true;
                break;
            }
        }
    }
    EXPECT_TRUE(hasContentPixel);
}

TEST_F(TestOfdModel, renderInvalidSize)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    EXPECT_TRUE(page->render(0, 0).isNull());
    EXPECT_TRUE(page->render(-10, 100).isNull());
}

TEST_F(TestOfdModel, loadMissingFile)
{
    Document::Error error = Document::NoError;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(QStringLiteral("/nonexistent/path/to.ofd"), error));
    EXPECT_EQ(doc, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

TEST_F(TestOfdModel, loadBrokenFile)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const QString brokenPath = dir.filePath(QStringLiteral("broken.ofd"));
    QFile file(brokenPath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.write("this is not an ofd zip archive");
    file.close();

    Document::Error error = Document::NoError;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(brokenPath, error));
    EXPECT_EQ(doc, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

#endif // OFD_SUPPORT_ENABLED

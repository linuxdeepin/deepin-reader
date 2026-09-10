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

#include <algorithm>
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

TEST_F(TestOfdModel, semanticFullText)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QString text = page->text(QRectF());
    EXPECT_FALSE(text.isEmpty());
    EXPECT_TRUE(text.contains(QStringLiteral("电子发票")));
}

TEST_F(TestOfdModel, semanticSearch)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QVector<PageSection> matches = page->search(QStringLiteral("电子发票"), true, false);
    ASSERT_FALSE(matches.isEmpty());
    ASSERT_FALSE(matches.first().isEmpty());
    EXPECT_TRUE(matches.first().first().rect.isValid());

    EXPECT_TRUE(page->search(QStringLiteral("不存在的文本"), false, false).isEmpty());
}

TEST_F(TestOfdModel, semanticWholeWordSearch)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    EXPECT_FALSE(page->search(QStringLiteral("电子"), false, false).isEmpty());
    EXPECT_TRUE(page->search(QStringLiteral("电子"), false, true).isEmpty());
}

TEST_F(TestOfdModel, semanticWordsAndAreaText)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QList<Word> words = page->words();
    ASSERT_FALSE(words.isEmpty());

    const auto character = std::find_if(words.cbegin(), words.cend(), [](const Word &word) {
        return word.text == QStringLiteral("电");
    });
    ASSERT_NE(character, words.cend());
    EXPECT_TRUE(character->boundingBox.isValid());
    EXPECT_GT(character->boundingBox.width(), 0.0);
    EXPECT_GT(character->boundingBox.height(), 0.0);

    const QRectF selection = character->boundingBox.adjusted(-0.1, -0.1, 0.1, 0.1);
    EXPECT_TRUE(page->text(selection).contains(character->text));
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

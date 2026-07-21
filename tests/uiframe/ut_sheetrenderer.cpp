// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SheetRenderer.h"
#include "DocSheet.h"
#include "PDFModel.h"
#include "dpdfpage.h"
#include "stub.h"

#include <DWidget>
#include <gtest/gtest.h>
#include <QMutex>
#include <QImage>
#include <QPointF>
#include <QRectF>

DWIDGET_USE_NAMESPACE

using deepin_reader::PDFPage;

static QString g_funcName;

int document_pageCount_stub()
{
    return 2;
}

QString document_label_stub(int index)
{
    if (index == 0)
        return "1";
    if (index == 1)
        return "iii";
    return QString();
}

class TestSheetRenderer : public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

protected:
    DocSheet *m_sheet = nullptr;
    SheetRenderer *m_tester = nullptr;
    DWidget *m_parent = nullptr;
};

void TestSheetRenderer::SetUp()
{
    m_parent = new DWidget;
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    m_sheet = new DocSheet(Dr::FileType::PDF, strPath, m_parent);
    m_tester = m_sheet->m_renderer;
    ASSERT_NE(m_tester, nullptr);
}

void TestSheetRenderer::TearDown()
{
    // Clear pages added during tests so the renderer destructor does not
    // queue a close task referencing deleted Page objects.
    if (m_tester != nullptr)
        m_tester->m_pages.clear();
    delete m_parent;
    g_funcName.clear();
}

TEST_F(TestSheetRenderer, initTest)
{
}

TEST_F(TestSheetRenderer, testOpenedEmpty)
{
    EXPECT_FALSE(m_tester->opened());
}

TEST_F(TestSheetRenderer, testGetPageCountEmpty)
{
    EXPECT_EQ(m_tester->getPageCount(), 0);
}

TEST_F(TestSheetRenderer, testGetImageInvalidIndex)
{
    QImage img = m_tester->getImage(0, 100, 100);
    EXPECT_TRUE(img.isNull());
}

TEST_F(TestSheetRenderer, testGetLinkAtPointInvalid)
{
    auto link = m_tester->getLinkAtPoint(0, QPointF(0, 0));
    EXPECT_FALSE(link.isValid());
}

TEST_F(TestSheetRenderer, testGetWordsInvalid)
{
    EXPECT_TRUE(m_tester->getWords(0).isEmpty());
}

TEST_F(TestSheetRenderer, testGetAnnotationsInvalid)
{
    EXPECT_TRUE(m_tester->getAnnotations(0).isEmpty());
}

TEST_F(TestSheetRenderer, testGetPageSizeInvalid)
{
    EXPECT_EQ(m_tester->getPageSize(0), QSizeF());
}

TEST_F(TestSheetRenderer, testGetTextInvalid)
{
    EXPECT_TRUE(m_tester->getText(0, QRectF(0, 0, 10, 10)).isEmpty());
}

TEST_F(TestSheetRenderer, testSearchInvalid)
{
    EXPECT_TRUE(m_tester->search(0, QString("test"), false, false).isEmpty());
}

TEST_F(TestSheetRenderer, testAddHighlightAnnotationInvalid)
{
    EXPECT_EQ(m_tester->addHighlightAnnotation(0, QList<QRectF>(), QString("test"), QColor()), nullptr);
}

TEST_F(TestSheetRenderer, testAddIconAnnotationInvalid)
{
    EXPECT_EQ(m_tester->addIconAnnotation(0, QRectF(0, 0, 10, 10), QString("test")), nullptr);
}

TEST_F(TestSheetRenderer, testMoveIconAnnotationInvalid)
{
    EXPECT_EQ(m_tester->moveIconAnnotation(0, nullptr, QRectF(0, 0, 10, 10)), nullptr);
}

TEST_F(TestSheetRenderer, testRemoveAnnotationInvalid)
{
    EXPECT_FALSE(m_tester->removeAnnotation(0, nullptr));
}

TEST_F(TestSheetRenderer, testUpdateAnnotationInvalid)
{
    EXPECT_FALSE(m_tester->updateAnnotation(0, nullptr, QString(), QColor()));
}

TEST_F(TestSheetRenderer, testInLinkInvalid)
{
    EXPECT_FALSE(m_tester->inLink(0, QPointF(0, 0)));
}

TEST_F(TestSheetRenderer, testHasWidgetAnnotsInvalid)
{
    EXPECT_FALSE(m_tester->hasWidgetAnnots(0));
}

TEST_F(TestSheetRenderer, testOutlineNoDocument)
{
    EXPECT_TRUE(m_tester->outline().isEmpty());
}

TEST_F(TestSheetRenderer, testPropertiesNoDocument)
{
    EXPECT_TRUE(m_tester->properties().isEmpty());
}

TEST_F(TestSheetRenderer, testSaveNoDocument)
{
    EXPECT_FALSE(m_tester->save());
}

TEST_F(TestSheetRenderer, testSaveAsEmptyPath)
{
    EXPECT_FALSE(m_tester->saveAs(QString()));
}

TEST_F(TestSheetRenderer, testSaveAsNoDocument)
{
    EXPECT_FALSE(m_tester->saveAs(QString("/tmp/test_saveas.xps")));
}

TEST_F(TestSheetRenderer, testPageLableIndexEmpty)
{
    EXPECT_EQ(m_tester->pageLableIndex(QString("iii")), -1);
}

TEST_F(TestSheetRenderer, testPageHasLableNoDocument)
{
    EXPECT_FALSE(m_tester->pageHasLable());
}

TEST_F(TestSheetRenderer, testPageNum2LableEmpty)
{
    EXPECT_EQ(m_tester->pageNum2Lable(0), QString("1"));
}

TEST_F(TestSheetRenderer, testHandleOpenedWithDocument)
{
    // Disconnect signal to prevent DocSheet::onOpened cascade that requires a real document
    m_tester->disconnect(m_tester, SIGNAL(sigOpened(int)), nullptr, nullptr);

    QMutex *mutex = new QMutex;
    DPdfPage *pdfPage = new DPdfPage(nullptr, 0, 96, 96);
    deepin_reader::Page *page = new PDFPage(mutex, pdfPage);

    QList<deepin_reader::Page *> pages;
    pages.append(page);

    m_tester->handleOpened(deepin_reader::Document::NoError, nullptr, pages);
    EXPECT_FALSE(m_tester->opened());  // document is null
    EXPECT_EQ(m_tester->getPageCount(), 1);

    // Clear pages so renderer destructor doesn't queue a close task with dangling page
    m_tester->m_pages.clear();
    delete page;
    delete pdfPage;
    delete mutex;
}

TEST_F(TestSheetRenderer, testPageNum2LableAfterLoad)
{
    m_tester->disconnect(m_tester, SIGNAL(sigOpened(int)), nullptr, nullptr);

    QMutex *mutex = new QMutex;
    DPdfPage *pdfPage = new DPdfPage(nullptr, 0, 96, 96);
    deepin_reader::Page *page = new PDFPage(mutex, pdfPage);

    QList<deepin_reader::Page *> pages;
    pages.append(page);

    m_tester->handleOpened(deepin_reader::Document::NoError, nullptr, pages);

    // No labels loaded; page label map empty
    EXPECT_FALSE(m_tester->pageHasLable());
    EXPECT_EQ(m_tester->pageNum2Lable(0), QString("1"));

    m_tester->m_pages.clear();
    delete page;
    delete pdfPage;
    delete mutex;
}

TEST_F(TestSheetRenderer, testGetImageWithPage)
{
    m_tester->disconnect(m_tester, SIGNAL(sigOpened(int)), nullptr, nullptr);

    QMutex *mutex = new QMutex;
    DPdfPage *pdfPage = new DPdfPage(nullptr, 0, 96, 96);
    deepin_reader::Page *page = new PDFPage(mutex, pdfPage);
    QList<deepin_reader::Page *> pages;
    pages.append(page);

    m_tester->handleOpened(deepin_reader::Document::NoError, nullptr, pages);
    QImage img = m_tester->getImage(0, 100, 100);
    Q_UNUSED(img);

    m_tester->m_pages.clear();
    delete page;
    delete pdfPage;
    delete mutex;
}

TEST_F(TestSheetRenderer, testLoadPageLableDirectly)
{
    // Test that loadPageLable is safe when document is null
    m_tester->loadPageLable();
    SUCCEED();
}

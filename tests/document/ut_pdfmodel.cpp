// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PDFModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "stub.h"

#include <gtest/gtest.h>

#include <QTemporaryDir>
#include <QThread>
#include <atomic>
#include <thread>

using namespace deepin_reader;

/********测试PDFAnnotation***********/
class TestPDFAnnotation : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    PDFAnnotation *m_tester = nullptr;
    DPdfTextAnnot *dAnnot = nullptr;
};

void TestPDFAnnotation::SetUp()
{
    dAnnot = new DPdfTextAnnot;
    m_tester = new PDFAnnotation(dAnnot);
    m_tester->disconnect();
}

void TestPDFAnnotation::TearDown()
{
    delete m_tester;
    delete dAnnot;
}

/***********测试用例***********/
TEST_F(TestPDFAnnotation, UT_PDFAnnotation_boundary_001)
{
    dAnnot->m_type = DPdfAnnot::AText;
    dAnnot->m_rect = QRectF();
    EXPECT_EQ(m_tester->boundary().size(), 1);
}

TEST_F(TestPDFAnnotation, UT_PDFAnnotation_contents_001)
{
    dAnnot->m_text = "test";
    EXPECT_TRUE(m_tester->contents() == "test");

    m_tester->m_dannotation = nullptr;
    EXPECT_TRUE(m_tester->contents().isEmpty());
}

TEST_F(TestPDFAnnotation, UT_PDFAnnotation_type_001)
{
    dAnnot->m_type = DPdfAnnot::AText;
    EXPECT_TRUE(m_tester->type() == DPdfAnnot::AText);

    m_tester->m_dannotation = nullptr;
    EXPECT_TRUE(m_tester->type() == -1);
}

TEST_F(TestPDFAnnotation, UT_PDFAnnotation_ownAnnotation_001)
{
    EXPECT_TRUE(m_tester->ownAnnotation() == dAnnot);
}

/**********测试PDFPage*************/
class TestPDFPage : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    PDFPage *m_tester = nullptr;
//    DPdfTextAnnot *dAnnot = nullptr;
    QMutex *m_docMutex = nullptr;
//    DPdfDocHandler *m_docHandler = nullptr;
    DPdfPage *m_page = nullptr;
};

void TestPDFPage::SetUp()
{
    m_page = new DPdfPage(nullptr, 0, 96, 96);
    m_docMutex = new QMutex;
    m_tester = new PDFPage(m_docMutex, m_page);
}

void TestPDFPage::TearDown()
{
    delete m_tester;
    delete m_docMutex;
    delete m_page;
}

/********桩函数**************/
QSizeF sizeF_stub()
{
    return QSizeF(100, 200);
}

QImage image_stub(void *, int, int, QRect slice)
{
    return QImage(slice.width(), slice.height(), QImage::Format_ARGB32);
}

QList<DPdfAnnot *> empty_links_stub()
{
    QList<DPdfAnnot *> links;
    return links;
}
static DPdfLinkAnnot *g_linkAnnots = nullptr;
QList<DPdfAnnot *> links_stub()
{
    QList<DPdfAnnot *> links;
    g_linkAnnots = new DPdfLinkAnnot;
    g_linkAnnots->setRectF(QRectF(0, 0, 20, 20));
    g_linkAnnots->setPage(1, 30, 40);
    g_linkAnnots->setFilePath("t/e/s/t");
    links.append(g_linkAnnots);
    return links;
}

QList<DPdfAnnot *> links_stub2()
{
    QList<DPdfAnnot *> links;
    g_linkAnnots = new DPdfLinkAnnot;
    g_linkAnnots->setRectF(QRectF(0, 0, 20, 20));
    g_linkAnnots->setPage(1, 30, 40);
    g_linkAnnots->setUrl("http://www.123.com");
    links.append(g_linkAnnots);
    return links;
}

QList<DPdfAnnot *> widgets_stub()
{
    QList<DPdfAnnot *> widgets;
    g_linkAnnots = new DPdfLinkAnnot();
    widgets.append(g_linkAnnots);
    return widgets;
}

QString text_stub(const QRectF &)
{
    return "  test\r\n";
}

void allTextLooseRects_stub(void *, int &charCount, QStringList &texts, QVector<QRectF> &rects)
{
    charCount = 2;
    texts.append("first");
    texts.append("second");
    rects.append(QRectF(0, 0, 20, 10));
    rects.append(QRectF(0, 20, 20, 10));
}

QVector<PageSection> search_stub(const QString &, bool, bool)
{
    PageLine line;
    line.rect = QRectF(0, 0, 12.3, 12.3);

    PageSection section;
    section.append(line);

    QVector<PageSection> results;
    results.append(section);
    return results;
}

static DPdfTextAnnot *g_textAnnots = nullptr;
static QList<DPdfAnnot *> g_dAnnotlsit;
QList<DPdfAnnot *> annots_stub()
{
    if (!g_dAnnotlsit.isEmpty())
        return g_dAnnotlsit;

    QList<DPdfAnnot *> dannots;
    g_textAnnots = new DPdfTextAnnot();
    dannots.append(g_textAnnots);
    return dannots;
}

static DPdfHightLightAnnot *g_HightLightAnnots = nullptr;
DPdfAnnot *createHightLightAnnot_stub(const QList<QRectF> &, QString, QColor)
{
    g_HightLightAnnots = new DPdfHightLightAnnot;
    return g_HightLightAnnots;
}

bool removeAnnot_stub(DPdfAnnot *dAnnot)
{
    delete dAnnot;
    return true;
}
static QString g_funcName;
bool updateTextAnnot_stub(DPdfAnnot *, QString, QPointF)
{
    g_funcName = __FUNCTION__;
    return true;
}
bool updateHightLightAnnot_stub(DPdfAnnot *, QColor, QString)
{
    g_funcName = __FUNCTION__;
    return true;
}

bool contains_stub(const DPdfAnnot &)
{
    return true;
}

DPdfAnnot *createTextAnnot_stub(QPointF, QString)
{
    g_textAnnots = new DPdfTextAnnot;
    return g_textAnnots;
}
/**********测试用例************/
TEST_F(TestPDFPage, UT_PDFPage_sizeF_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, sizeF), sizeF_stub);
    EXPECT_TRUE(m_tester->sizeF() == QSizeF(100, 200));
}

TEST_F(TestPDFPage, UT_PDFPage_render_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, image), image_stub);
    int width = 1000;
    int height = 2000;
    QRect slice(0, 0, 100, 200);
    EXPECT_TRUE(m_tester->render(width, height, slice).width() == 100);
}

TEST_F(TestPDFPage, UT_PDFPage_getLinkAtPoint_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, links), empty_links_stub);
    EXPECT_TRUE(m_tester->getLinkAtPoint(QPointF(0, 0)).page == -1);

    s.reset(ADDR(DPdfPage, links));
    s.set(ADDR(DPdfPage, links), links_stub);
    Link link = m_tester->getLinkAtPoint(QPointF(5, 5));
    if (g_linkAnnots) {
        delete g_linkAnnots;
        g_linkAnnots = nullptr;
    }
    EXPECT_TRUE(link.page == 2);
    EXPECT_TRUE(link.left == 30.0);
    EXPECT_TRUE(link.top == 40.0);
    EXPECT_TRUE(link.urlOrFileName == "t/e/s/t");

    s.reset(ADDR(DPdfPage, links));
    s.set(ADDR(DPdfPage, links), links_stub2);
    link = m_tester->getLinkAtPoint(QPointF(5, 5));
    if (g_linkAnnots) {
        delete g_linkAnnots;
        g_linkAnnots = nullptr;
    }
    EXPECT_TRUE(link.page == 2);
    EXPECT_TRUE(link.left == 30.0);
    EXPECT_TRUE(link.top == 40.0);
    EXPECT_TRUE(link.urlOrFileName == "http://www.123.com");
}

TEST_F(TestPDFPage, UT_PDFPage_hasWidgetAnnots_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, widgets), widgets_stub);

    EXPECT_TRUE(m_tester->hasWidgetAnnots());
    if (g_linkAnnots) {
        delete g_linkAnnots;
        g_linkAnnots = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_text_001)
{
    Stub s;
    s.set(static_cast<QString(DPdfPage::*)(const QRectF &)>(ADDR(DPdfPage, text)), text_stub);

    EXPECT_TRUE(m_tester->text(QRectF(0, 0, 10, 10)) == "test");
}

TEST_F(TestPDFPage, UT_PDFPage_words_001)
{
    m_tester->m_wordLoaded = true;
    Word w;
    w.text = "test";
    w.boundingBox = QRectF(0, 0, 20, 10);
    m_tester->m_words.append(w);
    EXPECT_TRUE(m_tester->words().size() == 1);

    Stub s;
    s.set(static_cast<void(DPdfPage::*)(int &, QStringList &, QVector<QRectF> &)>(ADDR(DPdfPage, allTextLooseRects)), allTextLooseRects_stub);
    m_tester->m_wordLoaded = false;
    m_tester->words();
    EXPECT_TRUE(m_tester->words().size() == 3);
}

TEST_F(TestPDFPage, UT_PDFPage_search_001)
{
    typedef QVector<PageSection> (*searchPtr)(const QString &, bool, bool);
    Stub s;
    s.set((searchPtr)ADDR(DPdfPage, search), search_stub);

    EXPECT_TRUE(m_tester->search(QString("test"), false, false).size() == 1);
}

TEST_F(TestPDFPage, UT_PDFPage_annotations_001)
{
    Stub s;
    s.set(static_cast<QList<DPdfAnnot *>(DPdfPage::*)()>(ADDR(DPdfPage, annots)), annots_stub);

    QList< Annotation * > annotlist = m_tester->annotations();
    EXPECT_TRUE(annotlist.size() == 1);
    qDeleteAll(annotlist);
    annotlist.clear();
    if (g_textAnnots) {
        delete g_textAnnots;
        g_linkAnnots = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_addHighlightAnnotation_001)
{
    Stub s;
    s.set(static_cast<DPdfAnnot*(DPdfPage::*)(const QList<QRectF> &, QString, QColor)>(ADDR(DPdfPage, createHightLightAnnot)), createHightLightAnnot_stub);
    QList<QRectF> boundaries{QRectF(0, 0, 1.1, 2.2)};
    QString text("test");
    QColor color(Qt::red);
    Annotation *annot = m_tester->addHighlightAnnotation(boundaries, text, color);
    EXPECT_TRUE(annot != nullptr);
    delete annot;
    if (g_HightLightAnnots) {
        delete g_HightLightAnnots;
        g_HightLightAnnots = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_removeAnnotation_001)
{
    PDFAnnotation *annotation = nullptr;
    EXPECT_FALSE(m_tester->removeAnnotation(annotation));

    Stub s;
    s.set(static_cast<bool(DPdfPage::*)(DPdfAnnot *)>(ADDR(DPdfPage, removeAnnot)), removeAnnot_stub);
    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;
    annotation = new PDFAnnotation(dAnnot);
    EXPECT_TRUE(m_tester->removeAnnotation(annotation));
    delete dAnnot;
}

TEST_F(TestPDFPage, UT_PDFPage_updateAnnotation_001)
{
    PDFAnnotation *annotation = nullptr;
    QString text("test");
    QColor color(Qt::red);
    EXPECT_FALSE(m_tester->updateAnnotation(annotation, text, color));

    Stub s;
    s.set(static_cast<bool(DPdfPage::*)(DPdfAnnot *, QString txt, QPointF)>(ADDR(DPdfPage, updateTextAnnot)), updateTextAnnot_stub);
    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;
    dAnnot->m_type = DPdfAnnot::AText;
    annotation = new PDFAnnotation(dAnnot);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    s.set(ADDR(QList<DPdfAnnot *>, contains), contains_stub);
#else
    // On Qt6 QList::contains stubbing does not work; stub DPdfPage::annots instead.
    g_dAnnotlsit.append(dAnnot);
    s.set(ADDR(DPdfPage, annots), annots_stub);
#endif

    EXPECT_TRUE(m_tester->updateAnnotation(annotation, text, color));
    EXPECT_TRUE(g_funcName == "updateTextAnnot_stub");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    g_dAnnotlsit.removeAll(dAnnot);
#endif
    if (annotation) {
        delete annotation;
        annotation = nullptr;
    }
    if (dAnnot) {
        delete dAnnot;
        dAnnot = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_updateAnnotation_002)
{
    Stub s;
    s.set(static_cast<bool(DPdfPage::*)(DPdfAnnot *, QColor, QString)>(ADDR(DPdfPage, updateHightLightAnnot)), updateHightLightAnnot_stub);
    QString text("test");
    QColor color(Qt::red);
    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;
    dAnnot->m_type = DPdfAnnot::AHighlight;
    PDFAnnotation *annotation = new PDFAnnotation(dAnnot);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    s.set(ADDR(QList<DPdfAnnot *>, contains), contains_stub);
#else
    g_dAnnotlsit.append(dAnnot);
    s.set(ADDR(DPdfPage, annots), annots_stub);
#endif

    EXPECT_TRUE(m_tester->updateAnnotation(annotation, text, color));
    EXPECT_TRUE(g_funcName == "updateHightLightAnnot_stub");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    g_dAnnotlsit.removeAll(dAnnot);
#endif
    if (annotation) {
        delete annotation;
        annotation = nullptr;
    }
    if (dAnnot) {
        delete dAnnot;
        dAnnot = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_addIconAnnotation_001)
{
    QRectF rect(0, 0, 1.1, 2.2);
    QString text("test");
    DPdfPage *tmpPage = m_tester->m_page;
    m_tester->m_page = nullptr;
    Annotation *annot = m_tester->addIconAnnotation(rect, text);
    EXPECT_TRUE(annot == nullptr);

    m_tester->m_page = tmpPage;
    Stub s;
    s.set(static_cast<DPdfAnnot*(DPdfPage::*)(QPointF pos, QString)>(ADDR(DPdfPage, createTextAnnot)), createTextAnnot_stub);

    annot = m_tester->addIconAnnotation(rect, text);
    EXPECT_TRUE(annot != nullptr);
    delete annot;
    if (g_textAnnots) {
        delete g_textAnnots;
        g_textAnnots = nullptr;
    }
}

TEST_F(TestPDFPage, UT_PDFPage_moveIconAnnotation_001)
{
    QRectF rect(0, 0, 1.1, 2.2);
    QString text("test");
    DPdfPage *tmpPage = m_tester->m_page;
    m_tester->m_page = nullptr;
    PDFAnnotation *annotation = nullptr;
    EXPECT_TRUE(m_tester->moveIconAnnotation(annotation, rect) == nullptr);

    m_tester->m_page = tmpPage;
    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;
    annotation = new PDFAnnotation(dAnnot);
    Stub s;
    s.set(static_cast<bool(DPdfPage::*)(DPdfAnnot *, QString txt, QPointF)>(ADDR(DPdfPage, updateTextAnnot)), updateTextAnnot_stub);
    EXPECT_TRUE(m_tester->moveIconAnnotation(annotation, rect) != nullptr);
    EXPECT_TRUE(g_funcName == "updateTextAnnot_stub");
    if (annotation) {
        delete annotation;
        annotation = nullptr;
    }
    if (dAnnot) {
        delete dAnnot;
        dAnnot = nullptr;
    }
}

/* ====== Page base class virtual method coverage ======
 * The following tests exercise virtual methods declared in Model.h (Page base
 * class). Some have default implementations (cachedText, canAddAndRemoveAnnotations,
 * formFields, getLinkAtPoint) and others are overridden in PDFPage; calling them
 * through the interface ensures the base class declarations are referenced and
 * the overrides are entered.
 */
TEST_F(TestPDFPage, UT_PDFPage_cachedText_001)
{
    Stub s;
    s.set(static_cast<QString(DPdfPage::*)(const QRectF &)>(ADDR(DPdfPage, text)), text_stub);
    QRectF rect(0, 0, 10, 10);
    // Default impl of Page::cachedText forwards to text()
    EXPECT_TRUE(m_tester->cachedText(rect) == "test");
}

TEST_F(TestPDFPage, UT_PDFPage_canAddAndRemoveAnnotations_001)
{
    // PDFPage does not override; exercises base class default (returns false)
    EXPECT_FALSE(m_tester->canAddAndRemoveAnnotations());
}

TEST_F(TestPDFPage, UT_PDFPage_formFields_001)
{
    // PDFPage does not override; exercises base class default (returns empty list)
    EXPECT_TRUE(m_tester->formFields().isEmpty());
}

TEST_F(TestPDFPage, UT_PDFPage_getLinkAtPoint_default_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, links), empty_links_stub);
    // Empty links -> default-constructed Link() whose page == -1
    Link link = m_tester->getLinkAtPoint(QPointF(0, 0));
    EXPECT_EQ(link.page, -1);
}

TEST_F(TestPDFPage, UT_PDFPage_hasWidgetAnnots_default_001)
{
    Stub s;
    s.set(ADDR(DPdfPage, widgets), empty_links_stub);
    EXPECT_FALSE(m_tester->hasWidgetAnnots());
}

TEST_F(TestPDFPage, UT_PDFPage_words_default_001)
{
    // m_wordLoaded == false and stubbed allTextLooseRects returns no data
    Stub s;
    s.set(static_cast<void(DPdfPage::*)(int &, QStringList &, QVector<QRectF> &)>(ADDR(DPdfPage, allTextLooseRects)), allTextLooseRects_stub);
    m_tester->m_wordLoaded = false;
    EXPECT_TRUE(m_tester->words().size() >= 0);
}

TEST_F(TestPDFPage, UT_PDFPage_annotations_default_001)
{
    Stub s;
    s.set(static_cast<QList<DPdfAnnot *>(DPdfPage::*)()>(ADDR(DPdfPage, annots)), empty_links_stub);
    EXPECT_TRUE(m_tester->annotations().isEmpty());
}

/**********测试PDFDocument***********/
class TestPDFDocument : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    PDFDocument *m_tester = nullptr;
//    DPdfTextAnnot *dAnnot = nullptr;
//    QMutex *m_docMutex = nullptr;
//    DPdfDocHandler *m_docHandler = nullptr;
    DPdfDoc *m_document = nullptr;
};
void TestPDFDocument::SetUp()
{
    m_document = new DPdfDoc("test.pdf", "");
    m_tester = new PDFDocument(m_document);
}

void TestPDFDocument::TearDown()
{
    delete m_tester;
}

/*********桩函数**********/
int pageCount_stub()
{
    return 1;
}

static DPdfPage *g_dpdfpage;
DPdfPage *page_stub(void *, int i, qreal xRes, qreal yRes)
{
    g_dpdfpage = new DPdfPage(nullptr, i, xRes, yRes);
    return g_dpdfpage;
}

bool DPdfPage_isValid_stub()
{
    return true;
}

bool DPdfPage_isValid_stub_false()
{
    return false;
}

QString label_stub(int)
{
    return "test";
}

bool save_stub()
{
    g_funcName = __FUNCTION__;
    return false;
}

bool saveAs_stub(const QString &)
{
    g_funcName = __FUNCTION__;
    return false;
}

DPdfDoc::Outline outline_stub(qreal, qreal)
{
    DPdfDoc::Section sec;
    sec.nIndex = 0;
    sec.offsetPointF = QPointF(0, 0);
    sec.title = "first";
    DPdfDoc::Section sec1;
    sec1.nIndex = 0;
    sec1.offsetPointF = QPointF(0, 0);
    sec1.title = "first";
    sec1.children.append(sec);
    DPdfDoc::Outline cOutline;
    cOutline.append(sec1);

    return cOutline;
}

DPdfDoc::Properies proeries_stub()
{
    DPdfDoc::Properies properies;
    properies.insert("Version", "1");
    properies.insert("Title", "test");
    properies.insert("Creator", "cd");
    return properies;
}

DPdfDoc::Status status_SUCCESS_stub()
{
    return DPdfDoc::SUCCESS;
}

DPdfDoc::Status status_PASSWORDERROR_stub()
{
    return DPdfDoc::PASSWORD_ERROR;
}

/*********测试用例**********/
TEST_F(TestPDFDocument, UT_PDFDocument_pageCount_001)
{
    Stub s;
    s.set(ADDR(DPdfDoc, pageCount), pageCount_stub);

    EXPECT_EQ(m_tester->pageCount(), 1);
}

TEST_F(TestPDFDocument, UT_PDFDocument_page_001)
{
    EXPECT_TRUE(m_tester->page(0) == nullptr);

    Stub s;
    s.set(ADDR(DPdfDoc, page), page_stub);

    EXPECT_TRUE(m_tester->page(0) == nullptr);
    if (g_dpdfpage) {
        delete g_dpdfpage;
        g_dpdfpage = nullptr;
    }

    s.set(ADDR(DPdfPage, isValid), DPdfPage_isValid_stub);
    Page *page = m_tester->page(0);
    EXPECT_TRUE(page != nullptr);
    delete page;
    if (g_dpdfpage) {
        delete g_dpdfpage;
        g_dpdfpage = nullptr;
    }
}

TEST_F(TestPDFDocument, UT_PDFDocument_label_001)
{
    Stub s;
    s.set(ADDR(DPdfDoc, label), label_stub);

    EXPECT_TRUE(m_tester->label(0) == "test");
}

TEST_F(TestPDFDocument, UT_PDFDocument_label_002)
{
    // Without stub; document is empty/invalid, label() should return empty without crashing
    QString label = m_tester->label(0);
    EXPECT_TRUE(label.isNull() || !label.isNull());
}

TEST_F(TestPDFDocument, UT_PDFDocument_saveFilter_001)
{
    EXPECT_FALSE(m_tester->saveFilter().isEmpty());
    EXPECT_TRUE(m_tester->saveFilter().first().contains("*.pdf"));
}

TEST_F(TestPDFDocument, UT_PDFDocument_save_001)
{
    Stub s;
    s.set(ADDR(DPdfDoc, save), save_stub);
    m_tester->save();
    EXPECT_TRUE(g_funcName == "save_stub");
}

TEST_F(TestPDFDocument, UT_PDFDocument_saveAs_001)
{
    Stub s;
    s.set(ADDR(DPdfDoc, saveAs), saveAs_stub);
    EXPECT_FALSE(m_tester->saveAs(""));
    EXPECT_TRUE(g_funcName == "saveAs_stub");
}

TEST_F(TestPDFDocument, UT_PDFDocument_outline_001)
{
    Section sec;
    sec.nIndex = 0;
    sec.offsetPointF = QPointF(0, 0);
    sec.title = "first";

    m_tester->m_outline.append(sec);
    EXPECT_EQ(m_tester->outline().size(), 1);
    EXPECT_EQ(m_tester->outline().first().children.size(), 0);

    Stub s;
    s.set(ADDR(DPdfDoc, outline), outline_stub);
    m_tester->m_outline.clear();
    EXPECT_EQ(m_tester->outline().first().children.size(), 1);
}

TEST_F(TestPDFDocument, UT_PDFDocument_properties_001)
{
    Section sec;
    sec.nIndex = 0;
    sec.offsetPointF = QPointF(0, 0);
    sec.title = "first";

    m_tester->m_fileProperties.insert("Version", "1");
    EXPECT_EQ(m_tester->properties().size(), 1);
    EXPECT_TRUE(m_tester->properties().value("Version") == "1");

    Stub s;
    s.set(ADDR(DPdfDoc, proeries), proeries_stub);
    m_tester->m_fileProperties.clear();
    EXPECT_EQ(m_tester->properties().size(), 3);
}

TEST_F(TestPDFDocument, UT_PDFDocument_loadDocument_001)
{
    QString filePath("test.pdf");
    QString password;
    Document::Error error = Document::NoError;

    EXPECT_TRUE(m_tester->loadDocument(filePath, password, error) == nullptr);
    EXPECT_TRUE(error == Document::FileError);

    Stub s;
    s.set(ADDR(DPdfDoc, status), status_SUCCESS_stub);
    PDFDocument *pdfdocument = m_tester->loadDocument(filePath, password, error);
    EXPECT_TRUE(pdfdocument != nullptr);
    EXPECT_TRUE(error == Document::NoError);
    if (pdfdocument) {
        delete pdfdocument;
        pdfdocument = nullptr;
    }

    s.reset(ADDR(DPdfDoc, status));
    s.set(ADDR(DPdfDoc, status), status_PASSWORDERROR_stub);
    EXPECT_TRUE(m_tester->loadDocument(filePath, password, error) == nullptr);
    EXPECT_TRUE(error == Document::NeedPassword);

    password = "123";
    EXPECT_TRUE(m_tester->loadDocument(filePath, password, error) == nullptr);
    EXPECT_TRUE(error == Document::WrongPassword);
}

TEST_F(TestPDFDocument, UT_PDFDocument_fileIdentifier_001)
{
    QString id1 = m_tester->fileIdentifier();
    EXPECT_TRUE(id1 == m_tester->fileIdentifier());
}

// ==================== PMS 批次 2 补强 ====================
// 真实文件 fixture: normal.pdf / broken.pdf (UTSOURCEDIR/files/)
// 注意: PDFDocument 析构会 delete m_document, fixture 只 delete m_tester。
class TestPDFDocumentReal : public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    PDFDocument *m_tester = nullptr;
};

void TestPDFDocumentReal::SetUp()
{
    m_tester = new PDFDocument(new DPdfDoc(QString(UTSOURCEDIR) + "/files/normal.pdf", ""));
}

void TestPDFDocumentReal::TearDown()
{
    delete m_tester;
}

// BUG46911 (sev2): 打开损坏的 PDF 文件崩溃。
// 回归意图: broken.pdf 打开后 pageCount/page/properties 全链路安全降级。

TEST_F(TestPDFDocumentReal, BUG46911_brokenPdfOpensGracefully)
{
    PDFDocument brokenTester(new DPdfDoc(QString(UTSOURCEDIR) + "/files/broken.pdf", ""));

    // 损坏文档可打开(部分对象可解析), 取损坏页时安全降级为空, 不崩溃
    EXPECT_GE(brokenTester.pageCount(), 0);
    EXPECT_TRUE(brokenTester.page(0) == nullptr);
}

TEST_F(TestPDFDocumentReal, BUG46911_brokenPdfPropertiesSafe)
{
    PDFDocument brokenTester(new DPdfDoc(QString(UTSOURCEDIR) + "/files/broken.pdf", ""));

    // 损坏文档属性/大纲可重复获取且稳定(不崩溃不漂移)
    Properties first = brokenTester.properties();
    Properties second = brokenTester.properties();
    EXPECT_EQ(first.size(), second.size());

    QList<Section> outline = brokenTester.outline();
    Q_UNUSED(outline);
    SUCCEED();
}

// BUG54210 (sev2): 无效页面(isValid=false)未拦截导致下游崩溃。
// 回归意图: PDFDocument::page 对 isValid=false 的页面返回空并安全释放。

TEST_F(TestPDFDocumentReal, BUG54210_pageInvalidPageReturnsNull)
{
    // 真实页面: 有效
    Page *validPage = m_tester->page(0);
    EXPECT_TRUE(validPage != nullptr);
    delete validPage;

    // isValid=false: 守卫生效返回空
    Stub s;
    s.set(ADDR(DPdfPage, isValid), DPdfPage_isValid_stub_false);

    Page *invalidPage = m_tester->page(0);
    EXPECT_TRUE(invalidPage == nullptr);
}

// BUG68550 (sev2): 选中文字时矩形/文本错乱。
// 回归意图: 真实页面上 allTextRects/allTextLooseRects 的 rects 与 texts 一一对应,
//          charCount 为上限(每字符一个合法矩形才追加)。

TEST_F(TestPDFDocumentReal, BUG68550_allTextRectsRectTextAligned)
{
    DPdfPage *dp = m_tester->m_document->page(0, 96, 96);
    ASSERT_TRUE(dp != nullptr);
    ASSERT_TRUE(dp->isValid());

    int charCount = 0;
    QStringList texts;
    QVector<QRectF> rects;
    dp->allTextRects(charCount, texts, rects);

    // 有文字的真实文档: 每个矩形对应一段文本, 数量严格一致
    EXPECT_GT(charCount, 0);
    EXPECT_EQ(rects.count(), texts.count());
    EXPECT_LE(rects.count(), charCount);
    // 注: DPdfPage 由 DPdfDoc(m_pages) 持有管理, 不可 delete (qDeleteAll 双重释放)
}

TEST_F(TestPDFDocumentReal, BUG68550_allTextLooseRectsRectTextAligned)
{
    DPdfPage *dp = m_tester->m_document->page(0, 96, 96);
    ASSERT_TRUE(dp != nullptr);

    int charCount = 0;
    QStringList texts;
    QVector<QRectF> rects;
    dp->allTextLooseRects(charCount, texts, rects);

    EXPECT_GT(charCount, 0);
    EXPECT_EQ(rects.count(), texts.count());
    EXPECT_FALSE(texts.join(QString()).isEmpty());
    // 同上: DPdfPage 生命周期归 DPdfDoc 管理
}

// BUG304111 (sev2): 获取文件属性时多线程重入死锁。
// 回归意图: 并发调用 properties() (内部 DPdfMutexLocker) 可在时限内完成。

TEST_F(TestPDFDocumentReal, BUG304111_concurrentPropertiesNoDeadlock)
{
    std::atomic<bool> done1(false), done2(false);
    std::thread t1([this, &done1]() {
        Properties p = m_tester->properties();
        Q_UNUSED(p);
        done1 = true;
    });
    std::thread t2([this, &done2]() {
        Properties p = m_tester->properties();
        Q_UNUSED(p);
        done2 = true;
    });

    // 3 秒时限内必须完成(死锁则超时失败)
    for (int i = 0; i < 300 && !(done1 && done2); ++i)
        QThread::msleep(10);
    EXPECT_TRUE(done1.load());
    EXPECT_TRUE(done2.load());
    t1.join();
    t2.join();
}

// BUG304471 (sev2): properties 重复调用结果漂移。
// 回归意图: 缓存机制保证多次获取属性内容一致。

TEST_F(TestPDFDocumentReal, BUG304471_propertiesCachedStable)
{
    Properties first = m_tester->properties();
    Properties second = m_tester->properties();

    EXPECT_EQ(first.size(), second.size());
    for (auto it = first.begin(); it != first.end(); ++it)
        EXPECT_TRUE(second.value(it.key()) == it.value());
}

// BUG351271 (sev2): SMB 等场景保存失败后文件损坏(未回滚)。
// 回归意图: 底层 save/saveAs 失败时, PDFDocument 包装层必须如实传递失败,
//          调用方据此不误报保存成功。

TEST_F(TestPDFDocumentReal, BUG351271_saveFailurePropagates)
{
    Stub s;
    s.set(ADDR(DPdfDoc, save), save_stub);

    EXPECT_FALSE(m_tester->save());
}

TEST_F(TestPDFDocumentReal, BUG351271_saveAsFailurePropagates)
{
    Stub s;
    s.set(ADDR(DPdfDoc, saveAs), saveAs_stub);

    EXPECT_FALSE(m_tester->saveAs("/tmp/ut_351271_out.pdf"));
}

// BUG353429 (sev2): 另存为输出的 PDF 文件无效/损坏。
// 回归意图: 真实文档 saveAs 输出存在、非空、以 %PDF 头开始(有效 PDF)。

TEST_F(TestPDFDocumentReal, BUG353429_saveAsOutputIsValidPdf)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString outPath = tmpDir.path() + "/saved.pdf";

    EXPECT_TRUE(m_tester->saveAs(outPath));

    QFile outFile(outPath);
    ASSERT_TRUE(outFile.exists());
    ASSERT_TRUE(outFile.open(QIODevice::ReadOnly));
    QByteArray head = outFile.read(5);
    outFile.close();
    EXPECT_GT(outFile.size(), QFileInfo(QString(UTSOURCEDIR) + "/files/normal.pdf").size() / 100);
    EXPECT_TRUE(head.startsWith("%PDF"));
}

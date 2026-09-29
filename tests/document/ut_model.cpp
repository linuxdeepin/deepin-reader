// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Model.h"
#include "PDFModel.h"
#include "DjVuModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "stub.h"

#include <QProcess>
#include <QDir>
#include <QtGlobal>
#include <QStandardPaths>
#include <QCoreApplication>

#include <gtest/gtest.h>
using namespace deepin_reader;

/***********桩函数**********/
PDFDocument *loadpdfDocument_stub(const QString &, const QString &, deepin_reader::Document::Error &error)
{
    error = Document::FileError;
    return nullptr;
}

DjVuDocument *loaddjvuDocument_stub(const QString &, deepin_reader::Document::Error &error, const QString & = "")
{
    error = Document::FileError;
    return nullptr;
}

bool copy_stub(void *, const QString &)
{
    return true;
}

void start_stub(const QString &, QProcess::OpenMode)
{
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void start_stub_qt6(const QString &, const QStringList &, QProcess::OpenMode)
{
}
#endif

bool waitForStarted_true_stub(int)
{
    return true;
}

bool waitForStarted_false_stub(int)
{
    return false;
}

bool waitForFinished_true_stub(int)
{
    return true;
}

bool waitForFinished_false_stub(int)
{
    return false;
}

bool exists_stub()
{
    return true;
}

/***********测试用例***********/
TEST(UT_DocumentFactory_getDocument, UT_DocumentFactory_getDocument_001)
{
    int fileType = Dr::Unknown;
    QString filePath = "test.txt";
    QString convertedFileDir = "";
    QString password = "";
    QProcess **pprocess = nullptr;
    deepin_reader::Document::Error error;
    deepin_reader::Document *pdocument = nullptr;

    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, pprocess, error);
    EXPECT_EQ(pdocument, nullptr);
}

TEST(UT_DocumentFactory_getDocument, UT_DocumentFactory_getDocument_002)
{
    Stub s;
    s.set(ADDR(PDFDocument, loadDocument), loadpdfDocument_stub);

    int fileType = Dr::PDF;
    QString filePath = "test.pdf";
    QString convertedFileDir;
    QString password;
    QProcess **pprocess = nullptr;
    deepin_reader::Document::Error error = Document::NoError;
    deepin_reader::Document *pdocument = nullptr;

    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, pprocess, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

TEST(UT_DocumentFactory_getDocument, UT_DocumentFactory_getDocument_003)
{
    Stub s;
    s.set(ADDR(DjVuDocument, loadDocument), loaddjvuDocument_stub);

    int fileType = Dr::DJVU;
    QString filePath = "test.djvu";
    QString convertedFileDir;
    QString password;
    QProcess **pprocess = nullptr;
    deepin_reader::Document::Error error = Document::NoError;
    deepin_reader::Document *pdocument = nullptr;

    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, pprocess, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::FileError);
}


TEST(UT_DocumentFactory_getDocument, UT_DocumentFactory_getDocument_004)
{
    int fileType = Dr::DOCX;
    QString filePath = "test.docx";
    QString convertedFileDir = QCoreApplication::applicationDirPath();
    QString password;
    QProcess p;
    QProcess *process = nullptr;
    deepin_reader::Document::Error error = Document::NoError;
    deepin_reader::Document *pdocument = nullptr;

    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);

    process = &p;
    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);

    Stub s;
    s.set(static_cast<bool(QFile::*)(const QString &)>(ADDR(QFile, copy)), copy_stub);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.set(static_cast<void(QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub_qt6);
#else
    s.set(static_cast<void(QProcess::*)(const QString &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub);
#endif
    Stub s1;

    s1.set(ADDR(QProcess, waitForStarted), waitForStarted_false_stub);
    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_TRUE(pdocument == nullptr);
    EXPECT_TRUE(process == nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);

    s1.reset(ADDR(QProcess, waitForStarted));
    s1.set(ADDR(QProcess, waitForStarted), waitForStarted_true_stub);
    s1.set(ADDR(QProcess, waitForFinished), waitForFinished_false_stub);
    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_TRUE(pdocument == nullptr);
    EXPECT_TRUE(process == nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);

    s1.reset(ADDR(QProcess, waitForFinished));
    s1.set(ADDR(QProcess, waitForFinished), waitForFinished_true_stub);
    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_TRUE(pdocument == nullptr);
    EXPECT_TRUE(process == nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);

    s1.set(static_cast<bool(QDir::*)()const>(ADDR(QDir, exists)), exists_stub);
    s1.set(static_cast<bool(QFile::*)()const>(ADDR(QFile, exists)), exists_stub);
    s1.set(ADDR(PDFDocument, loadDocument), loadpdfDocument_stub);
    pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_TRUE(pdocument == nullptr);
    EXPECT_TRUE(process == nullptr);
    EXPECT_EQ(error, Document::FileError);
}

// SearchResult helper tests
TEST(UT_SearchResult, sectionBoundingRectEmptySection)
{
    PageSection empty;
    QRectF rect = SearchResult::sectionBoundingRect(empty);
    EXPECT_TRUE(rect.isNull());
}

TEST(UT_SearchResult, sectionBoundingRectSingleLine)
{
    PageSection section;
    PageLine line;
    line.rect = QRectF(10, 20, 100, 30);
    section.append(line);

    QRectF rect = SearchResult::sectionBoundingRect(section);
    EXPECT_EQ(rect, QRectF(10, 20, 100, 30));
}

TEST(UT_SearchResult, sectionBoundingRectMultipleLines)
{
    PageSection section;
    PageLine l1;
    l1.rect = QRectF(0, 0, 50, 10);
    PageLine l2;
    l2.rect = QRectF(20, 30, 60, 15);
    section.append(l1);
    section.append(l2);

    QRectF rect = SearchResult::sectionBoundingRect(section);
    EXPECT_EQ(rect, QRectF(0, 0, 80, 45));
}

TEST(UT_SearchResult, setctionsFillTextEmptySections)
{
    SearchResult result;
    result.page = 1;
    bool ok = result.setctionsFillText([](int, QRectF) { return QString("text"); });
    EXPECT_FALSE(ok);
}

TEST(UT_SearchResult, setctionsFillTextWithContent)
{
    SearchResult result;
    result.page = 2;  // page is 1-based, so index is 1

    PageSection section;
    PageLine line;
    line.rect = QRectF(0, 0, 10, 10);
    section.append(line);
    result.sections.append(section);

    bool ok = result.setctionsFillText([](int index, QRectF) {
        EXPECT_EQ(index, 1);
        return QString("filled");
    });
    EXPECT_TRUE(ok);
    EXPECT_EQ(result.sections.at(0).at(0).text, QString("filled"));
}

TEST(UT_SearchResult, setctionsFillTextEmptyCallbackReturnsFalse)
{
    SearchResult result;
    result.page = 1;

    PageSection section;
    PageLine line;
    line.rect = QRectF(0, 0, 10, 10);
    section.append(line);
    result.sections.append(section);

    bool ok = result.setctionsFillText([](int, QRectF) { return QString(); });
    EXPECT_FALSE(ok);
}

/* ====== Document / Page / Annotation base class coverage ======
 * The following tests exercise:
 *  - Document::label() default behavior on a subclass that does not override it
 *    (DjVuDocument) to hit the base class implementation in Model.h
 *  - The virtual destructors of Document, Page and Annotation to ensure they
 *    are invoked (and registered for coverage) on real subclasses
 */

TEST(UT_DocumentBase, UT_Document_label_default_001)
{
    // DjVuDocument does not override label(); exercises Model.h default impl
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.djvu";
    deepin_reader::Document::Error error;
    DjVuDocument *doc = DjVuDocument::loadDocument(strPath, error);
    ASSERT_NE(doc, nullptr);
    // Default impl returns QString() -> empty
    EXPECT_TRUE(doc->label(0).isNull());
    delete doc;
}

TEST(UT_DocumentBase, UT_Document_destructor_001)
{
    // Exercise PDFDocument destructor (Document base dtor invoked virtually)
    DPdfDoc *d = new DPdfDoc("test.pdf", "");
    PDFDocument *pdfDoc = new PDFDocument(d);
    delete pdfDoc;
}

TEST(UT_DocumentBase, UT_Page_destructor_001)
{
    // Exercise PDFPage destructor (Page base dtor invoked virtually)
    DPdfPage *dpage = new DPdfPage(nullptr, 0, 96, 96);
    QMutex *mutex = new QMutex;
    PDFPage *page = new PDFPage(mutex, dpage);
    delete page;
    delete mutex;
}

TEST(UT_DocumentBase, UT_Annotation_destructor_001)
{
    // Exercise PDFAnnotation destructor (Annotation base dtor invoked virtually)
    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;
    PDFAnnotation *annot = new PDFAnnotation(dAnnot);
    annot->disconnect();
    delete annot;
    delete dAnnot;
}

/* ========== PMS 回归用例（sev1/2 bug 补强，批次1） ========== */

/* 说明：calculateTimeout / getHtmlToPdfPath 为 Model.cpp 文件内 static 自由函数，
 * 无法从测试侧直接调用，经公开入口 DocumentFactory::getDocument 的 DOCX 转换管线
 * 间接驱动（管线内 calculateTimeout×3、getHtmlToPdfPath×1）。
 */

// PMS: https://pms.uniontech.com/bug-view-332133.html  commit: b1746b32
TEST(UT_DocumentFactory_getDocument, BUG332133_docxTimeout_dynamicTimeoutNoHang)
{
    // 大文件 docx 转换超时修复：动态超时（按 MB 计算）替代固定超时，
    // 常规文件取基准超时且各阶段失败时优雅返回 ConvertFailed，不得挂起
    Stub s;
    s.set(static_cast<bool(QFile::*)(const QString &)>(ADDR(QFile, copy)), copy_stub);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.set(static_cast<void(QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub_qt6);
#else
    s.set(static_cast<void(QProcess::*)(const QString &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub);
#endif
    Stub s1;
    s1.set(ADDR(QProcess, waitForStarted), waitForStarted_true_stub);
    s1.set(ADDR(QProcess, waitForFinished), waitForFinished_true_stub);

    int fileType = Dr::DOCX;
    QString filePath = UTSOURCEDIR;
    filePath += "/files/normal.docx";
    QString convertedFileDir = QCoreApplication::applicationDirPath();
    QString password;
    QProcess p;
    QProcess *process = &p;
    Document::Error error = Document::NoError;

    Document *pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);  // word/ 目录未生成（unzip 桩不落盘）
    EXPECT_EQ(process, nullptr);
}

// PMS: https://pms.uniontech.com/bug-view-332133.html  commit: b1746b32
TEST(UT_DocumentFactory_getDocument, BUG332133_docxTimeout_overflowGuard)
{
    // 溢出保护：超大文件 sizeInMB 超过安全上限时超时取 MAX_TIMEOUT_MS，
    // 不得整型溢出/崩溃（300MB 稀疏文件 > 各阶段 maxSafeSize：unzip 285/pandoc 108/htmltopdf 48）
    QString bigPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ut_big_sparse.docx";
    QFile bigFile(bigPath);
    if (bigFile.exists())
        bigFile.remove();
    ASSERT_TRUE(bigFile.open(QIODevice::WriteOnly));
    ASSERT_TRUE(bigFile.resize(300LL * 1024 * 1024));  // 300MB 稀疏文件，瞬间创建
    bigFile.close();

    Stub s;
    s.set(static_cast<bool(QFile::*)(const QString &)>(ADDR(QFile, copy)), copy_stub);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.set(static_cast<void(QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub_qt6);
#else
    s.set(static_cast<void(QProcess::*)(const QString &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub);
#endif
    Stub s1;
    s1.set(ADDR(QProcess, waitForStarted), waitForStarted_true_stub);
    s1.set(ADDR(QProcess, waitForFinished), waitForFinished_true_stub);

    int fileType = Dr::DOCX;
    QString convertedFileDir = QCoreApplication::applicationDirPath();
    QString password;
    QProcess p;
    QProcess *process = &p;
    Document::Error error = Document::NoError;

    Document *pdocument = DocumentFactory::getDocument(fileType, bigPath, convertedFileDir, password, &process, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::ConvertFailed);
    EXPECT_EQ(process, nullptr);

    bigFile.remove();
}

// PMS: https://pms.uniontech.com/bug-view-304083.html  commit: f260a63f
TEST(UT_DocumentFactory_getDocument, BUG304083_docxPipeline_gracefulFailNoCrash)
{
    // docx 打不开修复（getHtmlToPdfPath 多级路径查找）：完整转换管线（unzip->pandoc->htmltopdf）
    // 各阶段失败必须优雅返回不崩溃，getHtmlToPdfPath 真实路径查找正常执行
    Stub s;
    s.set(static_cast<bool(QFile::*)(const QString &)>(ADDR(QFile, copy)), copy_stub);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.set(static_cast<void(QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub_qt6);
#else
    s.set(static_cast<void(QProcess::*)(const QString &, QProcess::OpenMode)>(ADDR(QProcess, start)), start_stub);
#endif
    Stub s1;
    s1.set(ADDR(QProcess, waitForStarted), waitForStarted_true_stub);
    s1.set(ADDR(QProcess, waitForFinished), waitForFinished_true_stub);
    Stub s2;
    s2.set(static_cast<bool(QDir::*)()const>(ADDR(QDir, exists)), exists_stub);
    s2.set(static_cast<bool(QFile::*)()const>(ADDR(QFile, exists)), exists_stub);
    Stub s3;
    s3.set(ADDR(PDFDocument, loadDocument), loadpdfDocument_stub);

    int fileType = Dr::DOCX;
    QString filePath = UTSOURCEDIR;
    filePath += "/files/normal.docx";
    QString convertedFileDir = QCoreApplication::applicationDirPath();
    QString password;
    QProcess p;
    QProcess *process = &p;
    Document::Error error = Document::NoError;

    Document *pdocument = DocumentFactory::getDocument(fileType, filePath, convertedFileDir, password, &process, error);
    EXPECT_EQ(pdocument, nullptr);
    EXPECT_EQ(error, Document::FileError);  // 转换产物加载失败（stub FileError）
    EXPECT_EQ(process, nullptr);
}

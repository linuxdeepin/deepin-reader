// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "formatconverter.h"
#include "errormessages.h"

#include "Global.h"
#include "Model.h"

#include <QFileInfo>
#include <QPdfWriter>
#include <QPageSize>
#include <QPainter>
#include <QDir>
#include <QDebug>
#include <QScopedPointer>

using deepin_reader::Document;
using deepin_reader::DocumentFactory;
using deepin_reader::Page;

static const int FallbackDpi = 300;

static Dr::FileType detectFileTypeWithFallback(const QString &filePath)
{
    Dr::FileType type = Dr::fileType(filePath);
    if (type != Dr::Unknown)
        return type;

    QString suffix = QFileInfo(filePath).suffix().toLower();
    if (suffix == QStringLiteral("pdf"))
        return Dr::PDF;
    if (suffix == QStringLiteral("docx"))
        return Dr::DOCX;
    if (suffix == QStringLiteral("djvu") || suffix == QStringLiteral("djv"))
        return Dr::DJVU;
#ifdef XPS_SUPPORT_ENABLED
    if (suffix == QStringLiteral("xps") || suffix == QStringLiteral("oxps"))
        return Dr::XPS;
#endif
    return Dr::Unknown;
}

QSizeF FormatConverter::computePageSizeMm(double widthPx, double heightPx, int dpi)
{
    if (dpi <= 0)
        dpi = FallbackDpi;

    double widthPt = widthPx * 72.0 / dpi;
    double heightPt = heightPx * 72.0 / dpi;
    double widthMm = widthPt * 25.4 / 72.0;
    double heightMm = heightPt * 25.4 / 72.0;

    if (widthMm <= 0 || heightMm <= 0)
        return QSizeF(210.0, 297.0);

    return QSizeF(widthMm, heightMm);
}

bool FormatConverter::convertToPdf(const QString &filePath, const QString &outputDir,
                                   QString &outputPdfPath, QString &errorMsg)
{
    QFileInfo fi(filePath);
    if (!fi.exists() || !fi.isReadable()) {
        errorMsg = ErrorMessages::convertFailed(fi.fileName());
        return false;
    }

    Dr::FileType type = detectFileTypeWithFallback(filePath);

    switch (type) {
    case Dr::PDF: {
        outputPdfPath = filePath;
        return true;
    }
    case Dr::DOCX: {
        QString convertedDir = outputDir;
        if (convertedDir.isEmpty())
            convertedDir = fi.absolutePath();

        Document::Error error = Document::NoError;
        QProcess *proc = nullptr;
        Document *doc = DocumentFactory::getDocument(Dr::DOCX, filePath, convertedDir,
                                                     QString(), &proc, error);
        if (doc) {
            delete doc;
            outputPdfPath = convertedDir + QStringLiteral("/temp.pdf");
            if (!QFileInfo::exists(outputPdfPath)) {
                errorMsg = ErrorMessages::convertFailed(fi.fileName());
                return false;
            }
            return true;
        }
        errorMsg = ErrorMessages::convertFailed(fi.fileName());
        return false;
    }
    case Dr::DJVU: {
        Document::Error error = Document::NoError;
        Document *doc = DocumentFactory::getDocument(Dr::DJVU, filePath, QString(),
                                                     QString(), nullptr, error);
        if (!doc) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            return false;
        }

        int pageCount = doc->pageCount();
        if (pageCount <= 0) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            delete doc;
            return false;
        }

        outputPdfPath = outputDir + QStringLiteral("/temp.pdf");
        QPdfWriter pdfWriter(outputPdfPath);
        pdfWriter.setResolution(FallbackDpi);

        // Set first page size BEFORE creating QPainter — QPdfWriter applies
        // page size when a new page begins, and the first page begins at
        // painter construction. Setting it afterwards has no effect on page 0.
        QScopedPointer<Page> firstPage(doc->page(0));
        if (firstPage) {
            QSizeF firstPageSizePx = firstPage->sizeF();
            int firstDpi = firstPage->resolution();
            if (firstDpi <= 0)
                firstDpi = FallbackDpi;
            double firstWidthPt = firstPageSizePx.width() * 72.0 / firstDpi;
            double firstHeightPt = firstPageSizePx.height() * 72.0 / firstDpi;
            double firstWidthMm = firstWidthPt * 25.4 / 72.0;
            double firstHeightMm = firstHeightPt * 25.4 / 72.0;
            if (firstWidthMm <= 0 || firstHeightMm <= 0) {
                firstWidthMm = 210.0;
                firstHeightMm = 297.0;
            }
            pdfWriter.setPageSize(QPageSize(QSizeF(firstWidthMm, firstHeightMm),
                                            QPageSize::Millimeter));
        }

        QPainter painter(&pdfWriter);
        if (!painter.isActive()) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            delete doc;
            return false;
        }

        bool anyPageRendered = false;
        for (int i = 0; i < pageCount; ++i) {
            QScopedPointer<Page> page(i == 0 ? firstPage.take() : doc->page(i));
            if (!page) {
                if (i > 0)
                    pdfWriter.newPage();
                continue;
            }

            QSizeF pageSizePx = page->sizeF();
            int dpi = page->resolution();
            if (dpi <= 0)
                dpi = FallbackDpi;

            double widthPt = pageSizePx.width() * 72.0 / dpi;
            double heightPt = pageSizePx.height() * 72.0 / dpi;
            double widthMm = widthPt * 25.4 / 72.0;
            double heightMm = heightPt * 25.4 / 72.0;

            if (widthMm <= 0 || heightMm <= 0) {
                widthMm = 210.0;
                heightMm = 297.0;
            }

            if (i > 0) {
                pdfWriter.setPageSize(QPageSize(QSizeF(widthMm, heightMm), QPageSize::Millimeter));
                pdfWriter.newPage();
            }

            int renderWidthPx = qRound(widthPt * FallbackDpi / 72.0);
            int renderHeightPx = qRound(heightPt * FallbackDpi / 72.0);
            if (renderWidthPx <= 0 || renderHeightPx <= 0) {
                renderWidthPx = static_cast<int>(8.27 * FallbackDpi);
                renderHeightPx = static_cast<int>(11.69 * FallbackDpi);
            }

            QImage image = page->render(renderWidthPx, renderHeightPx);

            if (!image.isNull()) {
                anyPageRendered = true;
                painter.drawImage(0, 0, image);
            }
        }

        painter.end();
        delete doc;

        if (!anyPageRendered) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            return false;
        }
        return true;
    }
    case Dr::XPS: {
#ifdef XPS_SUPPORT_ENABLED
        Document::Error error = Document::NoError;
        Document *doc = DocumentFactory::getDocument(Dr::XPS, filePath, QString(),
                                                     QString(), nullptr, error);
        if (!doc) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            return false;
        }

        outputPdfPath = outputDir + QStringLiteral("/temp.pdf");
        bool ok = doc->saveAs(outputPdfPath);
        delete doc;
        if (!ok || !QFileInfo::exists(outputPdfPath)) {
            errorMsg = ErrorMessages::convertFailed(fi.fileName());
            return false;
        }
        return true;
#else
        errorMsg = ErrorMessages::convertFailed(fi.fileName());
        return false;
#endif
    }
    default:
        errorMsg = ErrorMessages::convertFailed(fi.fileName());
        return false;
    }
}

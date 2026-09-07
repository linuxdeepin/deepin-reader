// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OfdModel.h"

#ifdef OFD_SUPPORT_ENABLED

#include "ddlog.h"

#include <QApplication>
#include <QFile>
#include <QRectF>
#include <QScreen>

namespace deepin_reader {

static constexpr qreal kMillimetresPerInch = 25.4;

OfdDocument *OfdDocument::loadDocument(const QString &filePath, Document::Error &error)
{
    qCInfo(appLog) << "Loading OFD document from:" << filePath;

    rofd_load_options_t loadOptions;
    rofd_load_options_init(&loadOptions, sizeof(loadOptions));

    rofd_document_t *document = nullptr;
    rofd_error_t *rofdError = nullptr;
    const QByteArray nativePath = QFile::encodeName(filePath);

    rofd_status_t status = rofd_document_open(nativePath.constData(), &loadOptions, &document, &rofdError);
    if (status != ROFD_STATUS_OK || nullptr == document) {
        qCWarning(appLog) << "Failed to open OFD document:" << filePath
                          << "status:" << status
                          << "message:" << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        error = Document::FileError;
        return nullptr;
    }

    rofd_renderer_t *renderer = nullptr;
    status = rofd_renderer_new(nullptr, &renderer, &rofdError);
    if (status != ROFD_STATUS_OK || nullptr == renderer) {
        qCWarning(appLog) << "Failed to create OFD renderer, status:" << status
                          << "message:" << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        rofd_document_free(document);
        error = Document::FileError;
        return nullptr;
    }

    error = Document::NoError;
    return new OfdDocument(filePath, document, renderer);
}

OfdDocument::OfdDocument(const QString &filePath, rofd_document_t *document, rofd_renderer_t *renderer)
    : m_filePath(filePath)
    , m_document(document)
    , m_renderer(renderer)
{
    size_t count = 0;
    if (ROFD_STATUS_OK != rofd_document_get_page_count(m_document, &count, nullptr)) {
        qCWarning(appLog) << "Failed to query OFD page count:" << m_filePath;
        count = 0;
    }
    m_pageCount = static_cast<int>(count);

    QScreen *srn = QApplication::screens().value(0);
    if (nullptr != srn) {
        m_xRes = srn->logicalDotsPerInchX(); // 获取屏幕的横纵向逻辑dpi
        m_yRes = srn->logicalDotsPerInchY();
    }

    qCInfo(appLog) << "OFD document loaded, pages:" << m_pageCount << "dpi:" << m_xRes << m_yRes;
}

OfdDocument::~OfdDocument()
{
    qCDebug(appLog) << "Destroying OFD document:" << m_filePath;
    rofd_renderer_free(m_renderer);
    rofd_document_free(m_document);
}

int OfdDocument::pageCount() const
{
    return m_pageCount;
}

Page *OfdDocument::page(int index) const
{
    if (index < 0 || index >= m_pageCount) {
        qCWarning(appLog) << "OFD page index out of range:" << index << "count:" << m_pageCount;
        return nullptr;
    }

    rofd_page_t *pageHandle = nullptr;
    rofd_error_t *rofdError = nullptr;
    rofd_status_t status = rofd_document_get_page(m_document, static_cast<size_t>(index), &pageHandle, &rofdError);
    if (status != ROFD_STATUS_OK || nullptr == pageHandle) {
        qCWarning(appLog) << "Failed to load OFD page:" << index
                          << "message:" << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        return nullptr;
    }

    return new OfdPage(this, pageHandle, index);
}

QStringList OfdDocument::saveFilter() const
{
    return QStringList() << QLatin1String("OFD (*.ofd)");
}

bool OfdDocument::save() const
{
    // OFD 后端为只读，文档不会产生需要落盘的修改
    return true;
}

bool OfdDocument::saveAs(const QString &filePath) const
{
    qCInfo(appLog) << "Saving OFD document copy to:" << filePath;

    if (QFile::exists(filePath) && !QFile::remove(filePath)) {
        qCWarning(appLog) << "Failed to remove existing target file:" << filePath;
        return false;
    }

    if (!QFile::copy(m_filePath, filePath)) {
        qCWarning(appLog) << "Failed to copy OFD document to:" << filePath;
        return false;
    }

    return true;
}

Properties OfdDocument::properties() const
{
    Properties props;
    props["Format"] = QStringLiteral("OFD");
    props["FilePath"] = m_filePath;
    return props;
}

QImage OfdDocument::renderPage(rofd_page_t *pageHandle, int width, int height, const QRect &slice) const
{
    if (nullptr == pageHandle || width <= 0 || height <= 0) {
        qCWarning(appLog) << "Invalid OFD render request, handle:" << pageHandle << "size:" << width << height;
        return QImage();
    }

    rofd_rect_t pageRect = {0.0, 0.0, 0.0, 0.0};
    if (ROFD_STATUS_OK != rofd_page_get_size_mm(pageHandle, &pageRect, nullptr) || pageRect.width_mm <= 0.0) {
        qCWarning(appLog) << "Failed to query OFD page size for rendering";
        return QImage();
    }

    // rofd 按 毫米 -> 像素 的单一比例渲染，目标整页宽度为 width 像素
    const double pixelsPerMm = static_cast<double>(width) / pageRect.width_mm;

    rofd_render_options_t options;
    rofd_render_options_init(&options, sizeof(options));
    options.dpi = pixelsPerMm * kMillimetresPerInch;
    options.scale = 1.0;

    int32_t pixelWidth = 0;
    int32_t pixelHeight = 0;
    rofd_error_t *rofdError = nullptr;
    if (ROFD_STATUS_OK != rofd_renderer_get_pixel_size(m_renderer, pageHandle, &options, &pixelWidth, &pixelHeight, &rofdError)
        || pixelWidth <= 0 || pixelHeight <= 0) {
        qCWarning(appLog) << "Failed to compute OFD pixel size:"
                          << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        return QImage();
    }

    QImage image(pixelWidth, pixelHeight, QImage::Format_ARGB32_Premultiplied);
    if (image.isNull()) {
        qCWarning(appLog) << "Failed to allocate OFD render image:" << pixelWidth << pixelHeight;
        return QImage();
    }
    image.fill(Qt::white);

    cairo_surface_t *surface = cairo_image_surface_create_for_data(image.bits(),
                                                                   CAIRO_FORMAT_ARGB32,
                                                                   pixelWidth,
                                                                   pixelHeight,
                                                                   image.bytesPerLine());
    if (CAIRO_STATUS_SUCCESS != cairo_surface_status(surface)) {
        qCWarning(appLog) << "Failed to create Cairo surface for OFD render";
        cairo_surface_destroy(surface);
        return QImage();
    }

    cairo_t *cr = cairo_create(surface);
    if (CAIRO_STATUS_SUCCESS != cairo_status(cr)) {
        qCWarning(appLog) << "Failed to create Cairo context for OFD render";
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return QImage();
    }

    rofd_render_report_t *report = nullptr;
    rofd_status_t status = rofd_renderer_render_page_cairo(m_renderer, pageHandle, cr, &options, &report, &rofdError);

    if (nullptr != report) {
        size_t diagnosticCount = 0;
        if (ROFD_STATUS_OK == rofd_render_report_get_count(report, &diagnosticCount, nullptr) && diagnosticCount > 0) {
            for (size_t i = 0; i < diagnosticCount; ++i) {
                rofd_render_diagnostic_t diagnostic;
                diagnostic.struct_size = sizeof(diagnostic);
                if (ROFD_STATUS_OK == rofd_render_report_get_diagnostic(report, i, &diagnostic, nullptr)) {
                    qCWarning(appLog) << "OFD render diagnostic, kind:" << diagnostic.kind
                                      << "object:" << diagnostic.object_id
                                      << "message:" << (diagnostic.message ? diagnostic.message : "");
                }
            }
        }
        rofd_render_report_free(report);
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    if (status != ROFD_STATUS_OK) {
        qCWarning(appLog) << "OFD page render failed, status:" << status
                          << "message:" << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        return QImage();
    }

    // rofd 的 clip 只限制绘制范围、不改变坐标映射，切片通过整页渲染后裁剪实现
    if (slice.isValid()) {
        const QRect bounded = slice.intersected(image.rect());
        if (bounded.isValid() && bounded.size() != image.size()) {
            return image.copy(bounded);
        }
    }

    return image;
}

OfdPage::OfdPage(const OfdDocument *document, rofd_page_t *pageHandle, int pageIndex)
    : m_document(document)
    , m_page(pageHandle)
    , m_pageIndex(pageIndex)
{
    rofd_rect_t pageRect = {0.0, 0.0, 0.0, 0.0};
    if (ROFD_STATUS_OK == rofd_page_get_size_mm(m_page, &pageRect, nullptr)) {
        m_sizePixel = QSizeF(pageRect.width_mm * m_document->xRes() / kMillimetresPerInch,
                             pageRect.height_mm * m_document->yRes() / kMillimetresPerInch);
    } else {
        qCWarning(appLog) << "Failed to query OFD page size, page:" << m_pageIndex;
    }
}

OfdPage::~OfdPage()
{
    rofd_page_free(m_page);
}

QSizeF OfdPage::sizeF() const
{
    return m_sizePixel;
}

QImage OfdPage::render(int width, int height, const QRect &slice) const
{
    if (nullptr == m_document || nullptr == m_page) {
        return QImage();
    }
    return m_document->renderPage(m_page, width, height, slice);
}

QString OfdPage::text(const QRectF &rect) const
{
    // rofd C ABI 暂不提供文本提取接口
    Q_UNUSED(rect)
    return QString();
}

QVector<PageSection> OfdPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    // rofd C ABI 暂不提供文本搜索接口
    Q_UNUSED(text)
    Q_UNUSED(matchCase)
    Q_UNUSED(wholeWords)
    return QVector<PageSection>();
}

} // namespace deepin_reader

#endif // OFD_SUPPORT_ENABLED

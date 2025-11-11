// Copyright (C) 2019 ~ 2025 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XpsDocumentAdapter.h"

#include "ddlog.h"

#include <QByteArray>
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QRectF>
#include <functional>

#ifdef signals
#pragma push_macro("signals")
#undef signals
#endif
#ifdef slots
#pragma push_macro("slots")
#undef slots
#endif

extern "C" {
#include <libgxps/gxps.h>
#include <cairo.h>
#include <gio/gio.h>
}

#ifdef slots
#pragma pop_macro("slots")
#endif
#ifdef signals
#pragma pop_macro("signals")
#endif

namespace deepin_reader {
namespace {

template <typename T>
class GObjectPtr
{
public:
    GObjectPtr() = default;
    explicit GObjectPtr(T *ptr)
        : m_ptr(ptr)
    {
    }
    ~GObjectPtr()
    {
        reset();
    }

    GObjectPtr(const GObjectPtr &) = delete;
    GObjectPtr &operator=(const GObjectPtr &) = delete;

    GObjectPtr(GObjectPtr &&other) noexcept
    {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }

    GObjectPtr &operator=(GObjectPtr &&other) noexcept
    {
        if (this != &other) {
            reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    void reset(T *ptr = nullptr)
    {
        if (m_ptr) {
            g_object_unref(m_ptr);
        }
        m_ptr = ptr;
    }

    T *release()
    {
        T *tmp = m_ptr;
        m_ptr = nullptr;
        return tmp;
    }

    T *get() const
    {
        return m_ptr;
    }

    explicit operator bool() const
    {
        return m_ptr != nullptr;
    }

    T *operator->() const
    {
        return m_ptr;
    }

private:
    T *m_ptr = nullptr;
};

struct GErrorPtr
{
    GErrorPtr() = default;
    ~GErrorPtr()
    {
        reset();
    }

    GError **outPtr()
    {
        reset();
        return &m_error;
    }

    GError *get() const
    {
        return m_error;
    }

    void reset()
    {
        if (m_error) {
            g_error_free(m_error);
            m_error = nullptr;
        }
    }

private:
    GError *m_error = nullptr;
};

struct CairoSurfacePtr
{
    CairoSurfacePtr() = default;
    explicit CairoSurfacePtr(cairo_surface_t *surface)
        : m_surface(surface)
    {
    }
    ~CairoSurfacePtr()
    {
        reset();
    }

    CairoSurfacePtr(const CairoSurfacePtr &) = delete;
    CairoSurfacePtr &operator=(const CairoSurfacePtr &) = delete;

    CairoSurfacePtr(CairoSurfacePtr &&other) noexcept
    {
        m_surface = other.m_surface;
        other.m_surface = nullptr;
    }

    CairoSurfacePtr &operator=(CairoSurfacePtr &&other) noexcept
    {
        if (this != &other) {
            reset();
            m_surface = other.m_surface;
            other.m_surface = nullptr;
        }
        return *this;
    }

    cairo_surface_t *get() const
    {
        return m_surface;
    }

    cairo_surface_t *operator->() const
    {
        return m_surface;
    }

    explicit operator bool() const
    {
        return m_surface != nullptr;
    }

    void reset(cairo_surface_t *surface = nullptr)
    {
        if (m_surface) {
            cairo_surface_destroy(m_surface);
        }
        m_surface = surface;
    }

private:
    cairo_surface_t *m_surface = nullptr;
};

struct CairoContextPtr
{
    CairoContextPtr() = default;
    explicit CairoContextPtr(cairo_t *context)
        : m_context(context)
    {
    }
    ~CairoContextPtr()
    {
        reset();
    }

    CairoContextPtr(const CairoContextPtr &) = delete;
    CairoContextPtr &operator=(const CairoContextPtr &) = delete;

    CairoContextPtr(CairoContextPtr &&other) noexcept
    {
        m_context = other.m_context;
        other.m_context = nullptr;
    }

    CairoContextPtr &operator=(CairoContextPtr &&other) noexcept
    {
        if (this != &other) {
            reset();
            m_context = other.m_context;
            other.m_context = nullptr;
        }
        return *this;
    }

    cairo_t *get() const
    {
        return m_context;
    }

    cairo_t *operator->() const
    {
        return m_context;
    }

    explicit operator bool() const
    {
        return m_context != nullptr;
    }

    void reset(cairo_t *context = nullptr)
    {
        if (m_context) {
            cairo_destroy(m_context);
        }
        m_context = context;
    }

private:
    cairo_t *m_context = nullptr;
};

QString toQString(const gchar *value)
{
    if (!value) {
        return QString();
    }
    return QString::fromUtf8(value);
}

QString normalizedString(const gchar *value, const QString &fallback = QString())
{
    QString result = toQString(value).trimmed();
    return result.isEmpty() ? fallback : result;
}

QDateTime fromUnixTime(time_t value)
{
    if (value <= 0) {
        return QDateTime();
    }
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(value), Qt::UTC).toLocalTime();
}

void logGError(const QString &prefix, GError *error)
{
    if (!error) {
        qCWarning(appLog) << prefix << "- Unknown error";
        return;
    }
    qCWarning(appLog) << prefix << "- domain:" << error->domain << "code:" << error->code << "message:" << error->message;
}

} // namespace

class XpsDocumentAdapter::Handle
{
public:
    Handle(GXPSFile *fileHandle, GXPSDocument *documentHandle)
        : file(fileHandle)
        , document(documentHandle)
    {
    }

    ~Handle()
    {
        if (document) {
            g_object_unref(document);
            document = nullptr;
        }
        if (file) {
            g_object_unref(file);
            file = nullptr;
        }
    }

    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;

    Handle(Handle &&other) noexcept
    {
        file = other.file;
        document = other.document;
        other.file = nullptr;
        other.document = nullptr;
    }

    Handle &operator=(Handle &&other) noexcept
    {
        if (this != &other) {
            if (document) {
                g_object_unref(document);
            }
            if (file) {
                g_object_unref(file);
            }
            file = other.file;
            document = other.document;
            other.file = nullptr;
            other.document = nullptr;
        }
        return *this;
    }

    GXPSFile *file = nullptr;
    GXPSDocument *document = nullptr;
};

XpsDocumentAdapter::XpsDocumentAdapter(const QString &filePath,
                                       GXPSFile *fileHandle,
                                       GXPSDocument *documentHandle)
    : m_filePath(filePath)
    , m_handle(new Handle(fileHandle, documentHandle))
{
}

XpsDocumentAdapter::~XpsDocumentAdapter() = default;

XpsDocumentAdapter *XpsDocumentAdapter::loadDocument(const QString &filePath, Document::Error &error)
{
    error = Document::FileError;

    if (filePath.isEmpty()) {
        qCWarning(appLog) << "XPS load requested with empty file path";
        return nullptr;
    }

    QByteArray utf8 = QFile::encodeName(filePath);
    GObjectPtr<GFile> gioFile(g_file_new_for_path(utf8.constData()));
    if (!gioFile) {
        qCWarning(appLog) << "Failed to create GFile for" << filePath;
        return nullptr;
    }

    GErrorPtr fileError;
    GObjectPtr<GXPSFile> xpsFile(gxps_file_new(gioFile.get(), fileError.outPtr()));
    if (!xpsFile) {
        logGError(QStringLiteral("gxps_file_new failed"), fileError.get());
        if (fileError.get() && g_error_matches(fileError.get(), GXPS_FILE_ERROR, GXPS_FILE_ERROR_INVALID)) {
            error = Document::FileDamaged;
        }
        return nullptr;
    }

    guint documentCount = gxps_file_get_n_documents(xpsFile.get());
    if (documentCount == 0) {
        qCWarning(appLog) << "XPS file contains no documents" << filePath;
        return nullptr;
    }

    GErrorPtr docError;
    GObjectPtr<GXPSDocument> document(gxps_file_get_document(xpsFile.get(), 0, docError.outPtr()));
    if (!document) {
        logGError(QStringLiteral("gxps_file_get_document failed"), docError.get());
        if (docError.get() && g_error_matches(docError.get(), GXPS_FILE_ERROR, GXPS_FILE_ERROR_INVALID)) {
            error = Document::FileDamaged;
        }
        return nullptr;
    }

    auto *adapter = new XpsDocumentAdapter(filePath, xpsFile.release(), document.release());
    adapter->ensurePageCache();
    adapter->ensureProperties();
    adapter->ensureOutline();

    error = Document::NoError;
    return adapter;
}

int XpsDocumentAdapter::pageCount() const
{
    ensurePageCache();
    QMutexLocker lock(&m_mutex);
    return m_pageSizes.size();
}

Page *XpsDocumentAdapter::page(int index) const
{
    if (index < 0 || index >= pageCount()) {
        qCWarning(appLog) << "Requested XPS page out of range" << index;
        return nullptr;
    }
    return new XpsPageAdapter(this, index);
}

QStringList XpsDocumentAdapter::saveFilter() const
{
    return QStringList() << QStringLiteral("XPS files (*.xps)");
}

bool XpsDocumentAdapter::save() const
{
    qCWarning(appLog) << "XPS save() not implemented";
    return false;
}

bool XpsDocumentAdapter::saveAs(const QString &filePath) const
{
    Q_UNUSED(filePath)
    qCWarning(appLog) << "XPS saveAs() not implemented";
    return false;
}

Outline XpsDocumentAdapter::outline() const
{
    ensureOutline();
    QMutexLocker lock(&m_mutex);
    return m_outline;
}

Properties XpsDocumentAdapter::properties() const
{
    ensureProperties();
    QMutexLocker lock(&m_mutex);
    return m_properties;
}

QSizeF XpsDocumentAdapter::pageSize(int pageIndex) const
{
    ensurePageCache();
    QMutexLocker lock(&m_mutex);
    if (pageIndex < 0 || pageIndex >= m_pageSizes.size()) {
        return QSizeF();
    }
    return m_pageSizes.at(pageIndex);
}

QImage XpsDocumentAdapter::renderPage(int pageIndex, int width, int height, const QRect &slice) const
{
    if (!m_handle || !m_handle->document) {
        qCWarning(appLog) << "Attempting to render XPS page with invalid document handle";
        return QImage();
    }

    if (width <= 0 || height <= 0) {
        qCWarning(appLog) << "Invalid render target size" << width << height;
        return QImage();
    }

    ensurePageCache();

    QMutexLocker lock(&m_mutex);
    if (pageIndex < 0 || pageIndex >= m_pageSizes.size()) {
        qCWarning(appLog) << "Render request out of bounds" << pageIndex;
        return QImage();
    }

    qCDebug(appLog) << "Rendering XPS page" << pageIndex << "@" << width << "x" << height << "slice" << slice;

    GErrorPtr pageError;
    GObjectPtr<GXPSPage> page(gxps_document_get_page(m_handle->document, static_cast<guint>(pageIndex), pageError.outPtr()));
    if (!page) {
        logGError(QStringLiteral("gxps_document_get_page failed"), pageError.get());
        return QImage();
    }

    const QSizeF logicalSize = m_pageSizes.at(pageIndex);
    if (logicalSize.isEmpty() || logicalSize.width() <= 0.0 || logicalSize.height() <= 0.0) {
        qCWarning(appLog) << "Invalid logical size for XPS page" << logicalSize;
        return QImage();
    }

    const double pixelsPerDocX = static_cast<double>(width) / logicalSize.width();
    const double pixelsPerDocY = static_cast<double>(height) / logicalSize.height();

    if (pixelsPerDocX <= 0.0 || pixelsPerDocY <= 0.0) {
        qCWarning(appLog) << "Invalid pixels-per-document ratio" << pixelsPerDocX << pixelsPerDocY;
        return QImage();
    }

    const QRect fullSlice(0, 0, width, height);
    const QRect requestedSlice = slice.isValid() ? slice : fullSlice;

    if (requestedSlice.width() <= 0 || requestedSlice.height() <= 0) {
        qCWarning(appLog) << "Requested slice has non-positive size" << requestedSlice;
        return QImage();
    }

    const QSize targetSize(requestedSlice.size());

    QImage image(targetSize.width(), targetSize.height(), QImage::Format_ARGB32_Premultiplied);
    if (image.isNull()) {
        qCWarning(appLog) << "Failed to allocate image for XPS render";
        return image;
    }
    image.fill(Qt::transparent);

    CairoSurfacePtr surface(cairo_image_surface_create_for_data(image.bits(),
                                                               CAIRO_FORMAT_ARGB32,
                                                               targetSize.width(),
                                                               targetSize.height(),
                                                               image.bytesPerLine()));
    if (!surface || cairo_surface_status(surface.get()) != CAIRO_STATUS_SUCCESS) {
        qCWarning(appLog) << "Failed to create Cairo surface for XPS render";
        return QImage();
    }

    CairoContextPtr context(cairo_create(surface.get()));
    if (!context || cairo_status(context.get()) != CAIRO_STATUS_SUCCESS) {
        qCWarning(appLog) << "Failed to create Cairo context for XPS render";
        return QImage();
    }

    cairo_set_source_rgb(context.get(), 1.0, 1.0, 1.0);
    cairo_paint(context.get());

    // Scaling via cairo_matrix keeps high-DPI print requests accurate, no device-scale override needed here.
    cairo_matrix_t matrix;
    cairo_set_antialias(context.get(), CAIRO_ANTIALIAS_BEST);

    const double translateX = slice.isValid() ? -static_cast<double>(requestedSlice.left()) : 0.0;
    const double translateY = slice.isValid() ? -static_cast<double>(requestedSlice.top()) : 0.0;

    cairo_matrix_init(&matrix,
                      pixelsPerDocX, 0.0,
                      0.0, pixelsPerDocY,
                      translateX,
                      translateY);
    cairo_set_matrix(context.get(), &matrix);

    GErrorPtr renderError;
    if (!gxps_page_render(page.get(), context.get(), renderError.outPtr())) {
        logGError(QStringLiteral("gxps_page_render failed"), renderError.get());
        return QImage();
    }

    cairo_surface_flush(surface.get());
    return image.convertToFormat(QImage::Format_RGB32);
}

void XpsDocumentAdapter::ensurePageCache() const
{
    if (!m_handle || !m_handle->document) {
        return;
    }
    if (!m_pageSizes.isEmpty()) {
        return;
    }

    QMutexLocker lock(&m_mutex);
    if (!m_pageSizes.isEmpty()) {
        return;
    }

    const guint count = gxps_document_get_n_pages(m_handle->document);
    m_pageSizes.resize(static_cast<int>(count));

    for (guint i = 0; i < count; ++i) {
        gdouble width = 0.0;
        gdouble height = 0.0;
        gboolean gotSize = gxps_document_get_page_size(m_handle->document, i, &width, &height);

        if (!gotSize || width <= 0.0 || height <= 0.0) {
            GErrorPtr pageError;
            GObjectPtr<GXPSPage> page(gxps_document_get_page(m_handle->document, i, pageError.outPtr()));
            if (page) {
                gdouble fallbackWidth = 0.0;
                gdouble fallbackHeight = 0.0;
                gxps_page_get_size(page.get(), &fallbackWidth, &fallbackHeight);
                if (fallbackWidth > 0.0 && fallbackHeight > 0.0) {
                    width = fallbackWidth;
                    height = fallbackHeight;
                } else {
                    width = 612.0;
                    height = 792.0;
                }
            } else {
                width = 612.0;
                height = 792.0;
            }
        }

        m_pageSizes[static_cast<int>(i)] = QSizeF(width, height);
    }

    qCInfo(appLog) << "XPS document page cache initialized, pages:" << count;
}

void XpsDocumentAdapter::ensureProperties() const
{
    if (!m_handle || !m_handle->file) {
        return;
    }
    if (!m_properties.isEmpty()) {
        return;
    }

    ensurePageCache();

    QMutexLocker lock(&m_mutex);
    if (!m_properties.isEmpty()) {
        return;
    }

    Properties props;
    props["Format"] = QStringLiteral("XPS");
    props["FilePath"] = m_filePath;

    GErrorPtr coreError;
    GObjectPtr<GXPSCoreProperties> core(gxps_file_get_core_properties(m_handle->file, coreError.outPtr()));
    if (!core) {
        if (coreError.get()) {
            logGError(QStringLiteral("gxps_file_get_core_properties failed"), coreError.get());
        }
    } else {
        const auto setPropertyWithLog = [&](const QString &key, const gchar *value) {
            const QString text = normalizedString(value);
            if (text.isEmpty()) {
                qCDebug(appLog) << "XPS metadata missing or empty field:" << key;
            }
            props[key] = text;
        };

        setPropertyWithLog(QStringLiteral("Title"), gxps_core_properties_get_title(core.get()));
        setPropertyWithLog(QStringLiteral("Author"), gxps_core_properties_get_creator(core.get()));
        setPropertyWithLog(QStringLiteral("Subject"), gxps_core_properties_get_subject(core.get()));
        setPropertyWithLog(QStringLiteral("KeyWords"), gxps_core_properties_get_keywords(core.get()));
        setPropertyWithLog(QStringLiteral("Description"), gxps_core_properties_get_description(core.get()));
        setPropertyWithLog(QStringLiteral("Version"), gxps_core_properties_get_version(core.get()));
        setPropertyWithLog(QStringLiteral("Creator"), gxps_core_properties_get_last_modified_by(core.get()));

        const QDateTime created = fromUnixTime(gxps_core_properties_get_created(core.get()));
        const QDateTime modified = fromUnixTime(gxps_core_properties_get_modified(core.get()));
        if (created.isValid()) {
            props["CreationDate"] = created;
        } else {
            qCDebug(appLog) << "XPS metadata CreationDate missing or invalid";
        }
        if (modified.isValid()) {
            props["ModificationDate"] = modified;
        } else {
            qCDebug(appLog) << "XPS metadata ModificationDate missing or invalid";
        }
    }

    props["PageCount"] = m_pageSizes.size();
    const QSizeF firstSize = m_pageSizes.isEmpty() ? QSizeF() : m_pageSizes.first();
    props["Width"] = firstSize.width();
    props["Height"] = firstSize.height();

    m_properties = props;
}

void XpsDocumentAdapter::ensureOutline() const
{
    if (!m_handle || !m_handle->document) {
        return;
    }
    if (!m_outline.isEmpty()) {
        return;
    }

    QMutexLocker lock(&m_mutex);
    if (!m_outline.isEmpty()) {
        return;
    }

    GObjectPtr<GXPSDocumentStructure> structure(gxps_document_get_structure(m_handle->document));
    if (!structure) {
        qCDebug(appLog) << "XPS document has no structure section, outline unavailable";
        return;
    }

    if (!gxps_document_structure_has_outline(structure.get())) {
        qCDebug(appLog) << "XPS document structure present but outline is empty";
        return;
    }

    GXPSOutlineIter iter;
    if (!gxps_document_structure_outline_iter_init(&iter, structure.get())) {
        return;
    }

    auto anchorPointFor = [&](int pageIdx, const gchar *anchor) -> QPointF {
        if (pageIdx < 0 || !anchor) {
            return QPointF();
        }

        GErrorPtr pageError;
        GObjectPtr<GXPSPage> page(gxps_document_get_page(m_handle->document, static_cast<guint>(pageIdx), pageError.outPtr()));
        if (!page) {
            logGError(QStringLiteral("gxps_document_get_page failed while building outline"), pageError.get());
            return QPointF();
        }

        GErrorPtr anchorError;
        cairo_rectangle_t area {0, 0, 0, 0};
        if (!gxps_page_get_anchor_destination(page.get(), anchor, &area, anchorError.outPtr())) {
            if (anchorError.get()) {
                logGError(QStringLiteral("gxps_page_get_anchor_destination failed while building outline"), anchorError.get());
            }
            return QPointF();
        }
        return QPointF(area.x, area.y);
    };

    std::function<Section(GXPSOutlineIter *)> buildSection = [&](GXPSOutlineIter *it) -> Section {
        Section section;
        section.title = toQString(gxps_outline_iter_get_description(it));

        GXPSLinkTarget *target = gxps_outline_iter_get_target(it);
        if (target && gxps_link_target_is_internal(target)) {
            const gchar *anchor = gxps_link_target_get_anchor(target);
            if (anchor) {
                const int pageIdx = gxps_document_get_page_for_anchor(m_handle->document, anchor);
                section.nIndex = pageIdx;
                section.offsetPointF = anchorPointFor(pageIdx, anchor);
            }
        }

        GXPSOutlineIter childIter;
        if (gxps_outline_iter_children(&childIter, it)) {
            do {
                section.children.append(buildSection(&childIter));
            } while (gxps_outline_iter_next(&childIter));
        }

        return section;
    };

    Outline result;
    do {
        result.append(buildSection(&iter));
    } while (gxps_outline_iter_next(&iter));

    if (result.isEmpty()) {
        qCDebug(appLog) << "XPS outline iterator completed but no sections were generated";
    }

    m_outline = result;
}

XpsPageAdapter::XpsPageAdapter(const XpsDocumentAdapter *document, int pageIndex)
    : m_document(document)
    , m_pageIndex(pageIndex)
{
}

XpsPageAdapter::~XpsPageAdapter() = default;

QSizeF XpsPageAdapter::sizeF() const
{
    if (!m_document) {
        return QSizeF();
    }
    return m_document->pageSize(m_pageIndex);
}

QImage XpsPageAdapter::render(int width, int height, const QRect &slice) const
{
    if (!m_document) {
        return QImage();
    }
    return m_document->renderPage(m_pageIndex, width, height, slice);
}

Link XpsPageAdapter::getLinkAtPoint(const QPointF &point)
{
    Q_UNUSED(point)
    return Link();
}

QString XpsPageAdapter::text(const QRectF &rect) const
{
    Q_UNUSED(rect)
    return QString();
}

QVector<PageSection> XpsPageAdapter::search(const QString &text, bool matchCase, bool wholeWords) const
{
    Q_UNUSED(text)
    Q_UNUSED(matchCase)
    Q_UNUSED(wholeWords)
    return {};
}

QList<Annotation *> XpsPageAdapter::annotations() const
{
    return {};
}

QList<Word> XpsPageAdapter::words()
{
    return {};
}

} // namespace deepin_reader


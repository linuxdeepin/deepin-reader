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

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>

namespace deepin_reader {

static constexpr qreal kMillimetresPerInch = 25.4;

namespace {

QString takeRofdString(rofd_string_t *text)
{
    if (nullptr == text) {
        return QString();
    }

    const char *data = rofd_string_get_data(text);
    const size_t length = rofd_string_get_length(text);
    QString result;
    if (nullptr != data && length <= static_cast<size_t>(std::numeric_limits<int>::max())) {
        result = QString::fromUtf8(data, static_cast<int>(length));
    }
    rofd_string_free(text);
    return result;
}

void logSemanticError(const char *operation, int pageIndex, rofd_status_t status, rofd_error_t *error)
{
    qCWarning(appLog) << operation << "failed for OFD page:" << pageIndex
                      << "status:" << status
                      << "message:" << (error ? rofd_error_get_message(error) : "unknown");
    rofd_error_free(error);
}

} // namespace

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

    loadMetadata();
    warningDetails();
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
    warningDetails();
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

Outline OfdDocument::outline() const
{
    QMutexLocker lock(&m_outlineMutex);
    if (m_outlineLoaded)
        return m_outline;
    // Empty and failed snapshots are cached as well as populated ones.
    m_outlineLoaded = true;
    m_outline = [this]() -> Outline {
        rofd_outline_t *raw = nullptr;
        rofd_error_t *error = nullptr;
        const rofd_status_t status = rofd_document_get_outline(m_document, &raw, &error);
        const std::unique_ptr<rofd_outline_t, decltype(&rofd_outline_free)> snapshot(raw, rofd_outline_free);
        if (status != ROFD_STATUS_OK || !snapshot) {
            logSemanticError("Outline loading", -1, status, error);
            return {};
        }
        rofd_error_free(error);
        size_t count = 0;
        if (rofd_outline_get_count(raw, &count, nullptr) != ROFD_STATUS_OK
            || count > static_cast<size_t>(std::numeric_limits<int>::max())) {
            qCWarning(appLog) << "Invalid OFD outline node count";
            return {};
        }

        Outline nodes(static_cast<int>(count));
        QVector<int> parents(static_cast<int>(count), -1);
        for (int i = 0; i < nodes.size(); ++i) {
            rofd_outline_node_t node = {};
            node.struct_size = sizeof(node);
            if (rofd_outline_get_node(raw, static_cast<size_t>(i), &node, nullptr) != ROFD_STATUS_OK)
                continue;
            Section &section = nodes[i];
            section.title = QString::fromUtf8(node.title ? node.title : "");
            section.expanded = node.expanded != 0;
            // A preorder parent must precede its child. Invalid relations become
            // inert roots, retaining the node and any valid descendants.
            if (node.parent != ROFD_NO_INDEX) {
                if (node.parent >= static_cast<size_t>(i))
                    continue;
                parents[i] = static_cast<int>(node.parent);
            }
            for (size_t actionIndex = 0; actionIndex < node.action_count; ++actionIndex) {
                rofd_action_t action = {};
                action.struct_size = sizeof(action);
                if (rofd_outline_get_action(raw, static_cast<size_t>(i), actionIndex, &action, nullptr) != ROFD_STATUS_OK)
                    continue;
                rofd_destination_t destination = {};
                destination.struct_size = sizeof(destination);
                const rofd_destination_t *target = nullptr;
                if (action.event == ROFD_ACTION_EVENT_CLICK && action.kind == ROFD_ACTION_GOTO
                    && rofd_outline_get_action_destination(raw, static_cast<size_t>(i), actionIndex,
                                                           &destination, nullptr) == ROFD_STATUS_OK) {
                    target = &destination;
                }
                section.navigation = navigationTarget(action, target);
                if (!section.navigation)
                    continue;
                if (section.navigation->destination) {
                    const NavigationDestination &value = *section.navigation->destination;
                    section.nIndex = value.pageIndex;
                    section.offsetPointF = QPointF(value.left.value_or(0), value.top.value_or(0));
                }
                break;
            }
        }

        // Each child is complete before its parent. Reverse each collected group
        // once to restore the original preorder without recursive traversal or
        // repeated insertion at the front of a wide sibling list.
        Outline roots;
        for (int i = nodes.size(); i-- > 0;) {
            std::reverse(nodes[i].children.begin(), nodes[i].children.end());
            if (parents[i] >= 0)
                nodes[parents[i]].children.append(std::move(nodes[i]));
            else
                roots.append(std::move(nodes[i]));
        }
        std::reverse(roots.begin(), roots.end());
        return roots;
    }();
    const Outline result = m_outline;
    lock.unlock();
    warningDetails();
    return result;
}

std::optional<NavigationTarget> OfdDocument::navigationTarget(const rofd_action_t &action,
                                                            const rofd_destination_t *destination) const
{
    if (action.event != ROFD_ACTION_EVENT_CLICK)
        return std::nullopt;
    if (action.kind == ROFD_ACTION_URI) {
        NavigationTarget target;
        target.uri = resolveNavigationUri(QString::fromUtf8(action.uri ? action.uri : ""),
                                          QString::fromUtf8(action.uri_base ? action.uri_base : ""));
        return target.isValid() ? std::make_optional(target) : std::nullopt;
    }
    if (action.kind != ROFD_ACTION_GOTO || !destination
        || !(destination->flags & ROFD_DESTINATION_HAS_PAGE_INDEX)
        || destination->page_index >= static_cast<size_t>(m_pageCount)
        || destination->page_index > static_cast<size_t>(std::numeric_limits<int>::max())) {
        return std::nullopt;
    }

    NavigationDestination value;
    value.pageIndex = static_cast<int>(destination->page_index);
    switch (destination->kind) {
    case ROFD_DESTINATION_XYZ: value.mode = DestinationMode::XYZ; break;
    case ROFD_DESTINATION_FIT: value.mode = DestinationMode::Fit; break;
    case ROFD_DESTINATION_FIT_H: value.mode = DestinationMode::FitH; break;
    case ROFD_DESTINATION_FIT_V: value.mode = DestinationMode::FitV; break;
    case ROFD_DESTINATION_FIT_R: value.mode = DestinationMode::FitR; break;
    default: return std::nullopt;
    }
    const auto pageRect = navigationPageRect(value.pageIndex);
    if (!pageRect)
        return std::nullopt;
    if (destination->flags & ROFD_DESTINATION_HAS_LEFT)
        value.left = (destination->left_mm - pageRect->x_mm) * m_xRes / kMillimetresPerInch;
    if (destination->flags & ROFD_DESTINATION_HAS_TOP)
        value.top = (destination->top_mm - pageRect->y_mm) * m_yRes / kMillimetresPerInch;
    if (destination->flags & ROFD_DESTINATION_HAS_RIGHT)
        value.right = (destination->right_mm - pageRect->x_mm) * m_xRes / kMillimetresPerInch;
    if (destination->flags & ROFD_DESTINATION_HAS_BOTTOM)
        value.bottom = (destination->bottom_mm - pageRect->y_mm) * m_yRes / kMillimetresPerInch;
    if (destination->flags & ROFD_DESTINATION_HAS_ZOOM)
        value.zoom = destination->zoom;
    if (!value.isValid())
        return std::nullopt;
    NavigationTarget target;
    target.destination = value;
    return target;
}

std::optional<rofd_rect_t> OfdDocument::navigationPageRect(int pageIndex) const
{
    QMutexLocker lock(&m_navigationGeometryMutex);
    const auto cached = m_navigationPageRects.constFind(pageIndex);
    if (cached != m_navigationPageRects.cend())
        return cached.value();

    // Query a temporary rofd page directly: constructing OfdPage here would
    // couple navigation conversion to page-link loading and risk recursion.
    rofd_page_t *raw = nullptr;
    rofd_error_t *error = nullptr;
    rofd_status_t status = rofd_document_get_page(m_document, static_cast<size_t>(pageIndex), &raw, &error);
    const std::unique_ptr<rofd_page_t, decltype(&rofd_page_free)> page(raw, rofd_page_free);
    rofd_rect_t rect = {};
    if (status == ROFD_STATUS_OK && page) {
        rofd_error_free(error);
        error = nullptr;
        status = rofd_page_get_size_mm(raw, &rect, &error);
    }
    std::optional<rofd_rect_t> result;
    if (status == ROFD_STATUS_OK && page && std::isfinite(rect.x_mm) && std::isfinite(rect.y_mm)
        && std::isfinite(rect.width_mm) && rect.width_mm > 0
        && std::isfinite(rect.height_mm) && rect.height_mm > 0) {
        result = rect;
        rofd_error_free(error);
    } else {
        logSemanticError("Navigation page geometry", pageIndex, status, error);
    }
    m_navigationPageRects.insert(pageIndex, result);
    lock.unlock();
    // No warning-mutex holder acquires either navigation cache mutex.
    warningDetails();
    return result;
}

Properties OfdDocument::properties() const
{
    Properties props = m_properties;
    // Warnings grow during lazy page/content loading; never cache this snapshot
    // together with the immutable metadata. Each entry owns Code/Path/Message.
    props["Warnings"] = warningDetails();
    return props;
}

QString OfdDocument::fileIdentifier() const
{
    return m_properties.value("DocumentId").toString();
}

void OfdDocument::loadMetadata()
{
    m_properties["Format"] = QStringLiteral("OFD");
    m_properties["FilePath"] = m_filePath;
    m_properties["PageCount"] = m_pageCount;

    rofd_metadata_t *raw = nullptr;
    rofd_error_t *error = nullptr;
    const rofd_status_t status = rofd_document_get_metadata(m_document, &raw, &error);
    const std::unique_ptr<rofd_metadata_t, decltype(&rofd_metadata_free)> metadata(raw, rofd_metadata_free);
    if (status != ROFD_STATUS_OK || !metadata) {
        qCWarning(appLog) << "Failed to read OFD metadata:" << status
                          << (error ? rofd_error_get_message(error) : "unknown");
        rofd_error_free(error);
        return;
    }
    rofd_error_free(error);

    const auto put = [this](const char *key, const char *value) {
        if (value)
            m_properties[QLatin1String(key)] = QString::fromUtf8(value);
    };
    put("DocumentId", rofd_metadata_get_document_id(raw));
    put("Title", rofd_metadata_get_title(raw));
    put("Author", rofd_metadata_get_author(raw));
    put("Subject", rofd_metadata_get_subject(raw));
    put("Description", rofd_metadata_get_abstract(raw));
    put("Creator", rofd_metadata_get_creator(raw));
    put("CreatorVersion", rofd_metadata_get_creator_version(raw));
    // OFD identifies its producing application with Creator/CreatorVersion.
    const QString creator = m_properties.value("Creator").toString();
    if (!creator.isEmpty()) {
        const QString version = m_properties.value("CreatorVersion").toString();
        m_properties["Producer"] = version.isEmpty() ? creator : creator + QLatin1Char(' ') + version;
    }

    const auto putDate = [this, &put](const char *key, const char *rawKey, const char *value) {
        put(rawKey, value);
        if (value) {
            const QDateTime date = QDateTime::fromString(QString::fromUtf8(value), Qt::ISODate);
            if (date.isValid())
                m_properties[QLatin1String(key)] = date;
        }
    };
    putDate("CreationDate", "CreationDateRaw", rofd_metadata_get_creation_date(raw));
    putDate("ModificationDate", "ModificationDateRaw", rofd_metadata_get_modification_date(raw));

    size_t count = 0;
    if (rofd_metadata_get_keyword_count(raw, &count, nullptr) == ROFD_STATUS_OK) {
        QStringList keywords;
        for (size_t i = 0; i < count; ++i) {
            const char *keyword = nullptr;
            if (rofd_metadata_get_keyword(raw, i, &keyword, nullptr) == ROFD_STATUS_OK && keyword)
                keywords.append(QString::fromUtf8(keyword));
        }
        m_properties["KeyWords"] = keywords.join(QStringLiteral("; "));
    }
}

QVariantList OfdDocument::warningDetails() const
{
    QMutexLocker lock(&m_warningMutex);
    rofd_warning_list_t *raw = nullptr;
    rofd_error_t *error = nullptr;
    const rofd_status_t status = rofd_document_get_warnings(m_document, &raw, &error);
    const std::unique_ptr<rofd_warning_list_t, decltype(&rofd_warning_list_free)> warnings(raw, rofd_warning_list_free);
    if (status != ROFD_STATUS_OK || !warnings) {
        qCWarning(appLog) << "Failed to read OFD warnings:" << status
                          << (error ? rofd_error_get_message(error) : "unknown");
        rofd_error_free(error);
        return {};
    }
    rofd_error_free(error);

    QVariantList result;
    size_t count = 0;
    if (rofd_warning_list_get_count(raw, &count, nullptr) != ROFD_STATUS_OK)
        return result;
    for (size_t i = 0; i < count; ++i) {
        rofd_warning_t warning = {};
        warning.struct_size = sizeof(warning);
        if (rofd_warning_list_get_warning(raw, i, &warning, nullptr) != ROFD_STATUS_OK)
            continue;
        const QString path = QString::fromUtf8(warning.path ? warning.path : "");
        const QString message = QString::fromUtf8(warning.message ? warning.message : "");
        result.append(QVariantMap{{"Code", warning.code}, {"Path", path}, {"Message", message}});
        if (i >= m_loggedWarningCount)
            qCWarning(appLog) << "OFD parse warning:" << warning.code << path << message;
    }
    m_loggedWarningCount = count;
    return result;
}

QImage OfdDocument::renderPage(rofd_page_t *pageHandle, int width, int height, const QRect &slice) const
{
    if (nullptr == pageHandle || width <= 0 || height <= 0
        || (!slice.isNull() && !slice.isValid())) {
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
    // A tile only needs canvas geometry; the full-page query enforces a full
    // raster budget and would reject high zoom even for a tiny visible region.
    const auto sizeQuery = slice.isValid() ? rofd_renderer_get_pixel_canvas_size
                                           : rofd_renderer_get_pixel_size;
    if (ROFD_STATUS_OK != sizeQuery(m_renderer, pageHandle, &options, &pixelWidth, &pixelHeight, &rofdError)
        || pixelWidth <= 0 || pixelHeight <= 0) {
        qCWarning(appLog) << "Failed to compute OFD pixel size:"
                          << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        return QImage();
    }

    const QRect canvas(0, 0, pixelWidth, pixelHeight);
    const QRect target = slice.isValid() ? slice : canvas;
    // Do not silently clamp: the caller places the result at the requested
    // origin and expects exactly the requested dimensions.
    if (!canvas.contains(target)) {
        qCWarning(appLog) << "OFD render region outside canvas:" << target << canvas;
        return QImage();
    }
    const int argbStride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, target.width());
    const int maskStride = cairo_format_stride_for_width(CAIRO_FORMAT_A8, target.width());
    if (target.width() > 32767 || target.height() > 32767 || argbStride < 0 || maskStride < 0
        || (quint64(argbStride) * 2 + quint64(maskStride)) * quint64(target.height()) > options.max_raster_bytes) {
        qCWarning(appLog) << "OFD render target exceeds raster limits:" << target.size();
        return QImage();
    }

    QImage image(target.size(), QImage::Format_ARGB32_Premultiplied);
    if (image.isNull()) {
        qCWarning(appLog) << "Failed to allocate OFD render image:" << target.size();
        return QImage();
    }
    image.fill(Qt::white);

    cairo_surface_t *surface = cairo_image_surface_create_for_data(image.bits(),
                                                                   CAIRO_FORMAT_ARGB32,
                                                                   image.width(),
                                                                   image.height(),
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
    rofd_status_t status;
    if (slice.isValid()) {
        rofd_pixel_rect_t viewport;
        rofd_pixel_rect_init(&viewport, sizeof(viewport));
        viewport.x = target.x();
        viewport.y = target.y();
        viewport.width = target.width();
        viewport.height = target.height();
        status = rofd_renderer_render_page_region_cairo(m_renderer, pageHandle, cr, &options,
                                                       &viewport, &report, &rofdError);
    } else {
        status = rofd_renderer_render_page_cairo(m_renderer, pageHandle, cr, &options, &report, &rofdError);
    }
    warningDetails();

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

    cairo_surface_flush(surface);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    if (status != ROFD_STATUS_OK) {
        qCWarning(appLog) << "OFD page render failed, status:" << status
                          << "message:" << (rofdError ? rofd_error_get_message(rofdError) : "unknown");
        rofd_error_free(rofdError);
        return QImage();
    }

    rofd_error_free(rofdError);
    return image;
}

OfdPage::OfdPage(const OfdDocument *document, rofd_page_t *pageHandle, int pageIndex)
    : m_document(document)
    , m_page(pageHandle)
    , m_pageIndex(pageIndex)
{
    if (ROFD_STATUS_OK == rofd_page_get_size_mm(m_page, &m_pageRectMm, nullptr)) {
        m_sizePixel = QSizeF(m_pageRectMm.width_mm * m_document->xRes() / kMillimetresPerInch,
                             m_pageRectMm.height_mm * m_document->yRes() / kMillimetresPerInch);
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

Link OfdPage::getLinkAtPoint(const QPointF &point)
{
    if (!m_document || !m_page || !std::isfinite(point.x()) || !std::isfinite(point.y()))
        return {};
    QMutexLocker lock(&m_linksMutex);
    const bool loading = !m_linksLoaded;
    if (loading) {
        // The completed cache is Qt-owned and never changed, including empty
        // and failed snapshots. Mouse movement must not retry failed parsing.
        m_linksLoaded = true;
        m_links = [this]() -> QList<Link> {
            rofd_link_list_t *raw = nullptr;
            rofd_error_t *error = nullptr;
            const rofd_status_t status = rofd_page_get_links(m_page, &raw, &error);
            const std::unique_ptr<rofd_link_list_t, decltype(&rofd_link_list_free)> snapshot(raw, rofd_link_list_free);
            if (status != ROFD_STATUS_OK || !snapshot) {
                logSemanticError("Link loading", m_pageIndex, status, error);
                return {};
            }
            rofd_error_free(error);
            size_t count = 0;
            if (rofd_link_list_get_count(raw, &count, nullptr) != ROFD_STATUS_OK
                || count > static_cast<size_t>(std::numeric_limits<int>::max())) {
                qCWarning(appLog) << "Invalid OFD link count, page:" << m_pageIndex;
                return {};
            }
            QList<Link> links;
            for (size_t i = 0; i < count; ++i) {
                size_t actionCount = 0;
                if (rofd_link_list_get_action_count(raw, i, &actionCount, nullptr) != ROFD_STATUS_OK)
                    continue;
                Link link;
                for (size_t actionIndex = 0; actionIndex < actionCount; ++actionIndex) {
                    rofd_action_t action = {};
                    action.struct_size = sizeof(action);
                    if (rofd_link_list_get_action(raw, i, actionIndex, &action, nullptr) != ROFD_STATUS_OK)
                        continue;
                    rofd_destination_t destination = {};
                    destination.struct_size = sizeof(destination);
                    const rofd_destination_t *target = nullptr;
                    if (action.event == ROFD_ACTION_EVENT_CLICK && action.kind == ROFD_ACTION_GOTO
                        && rofd_link_list_get_action_destination(raw, i, actionIndex, &destination, nullptr) == ROFD_STATUS_OK) {
                        target = &destination;
                    }
                    link.navigation = m_document->navigationTarget(action, target);
                    if (link.navigation)
                        break;
                }
                if (!link.navigation)
                    continue;
                size_t regionCount = 0;
                if (rofd_link_list_get_region_count(raw, i, &regionCount, nullptr) != ROFD_STATUS_OK)
                    continue;
                // addRect uses the same winding for each positive rectangle:
                // overlapping areas stay clickable, but separated gaps do not.
                link.boundary.setFillRule(Qt::WindingFill);
                for (size_t regionIndex = 0; regionIndex < regionCount; ++regionIndex) {
                    rofd_rect_t region = {};
                    if (rofd_link_list_get_region(raw, i, regionIndex, &region, nullptr) != ROFD_STATUS_OK)
                        continue;
                    const QRectF rect = toPixels(region);
                    if (std::isfinite(rect.x()) && std::isfinite(rect.y())
                        && std::isfinite(rect.width()) && rect.width() > 0
                        && std::isfinite(rect.height()) && rect.height() > 0
                        && std::isfinite(rect.right()) && std::isfinite(rect.bottom())) {
                        link.boundary.addRect(rect);
                    }
                }
                if (link.boundary.isEmpty())
                    continue;
                if (link.navigation->destination) {
                    const auto &destination = *link.navigation->destination;
                    link.page = destination.pageIndex + 1;
                    link.left = destination.left.value_or(0);
                    link.top = destination.top.value_or(0);
                } else {
                    link.urlOrFileName = link.navigation->uri.toString(QUrl::FullyEncoded);
                }
                links.append(std::move(link));
            }
            return links;
        }();
    }
    Link result;
    for (const Link &link : std::as_const(m_links)) {
        if (link.boundary.contains(point)) {
            result = link;
            break;
        }
    }
    lock.unlock();
    if (loading)
        m_document->warningDetails();
    return result;
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
    if (nullptr == m_page) {
        return QString();
    }

    rofd_string_t *result = nullptr;
    rofd_error_t *error = nullptr;
    rofd_status_t status = ROFD_STATUS_OK;
    if (rect.isNull()) {
        status = rofd_page_get_text(m_page, &result, &error);
    } else {
        const rofd_rect_t area = toMillimetres(rect.normalized());
        status = rofd_page_get_text_for_area(m_page, &area, &result, &error);
    }

    if (ROFD_STATUS_OK != status || nullptr == result) {
        logSemanticError("Text extraction", m_pageIndex, status, error);
        rofd_string_free(result);
        return QString();
    }

    rofd_error_free(error);
    return takeRofdString(result).simplified();
}

QVector<PageSection> OfdPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    QVector<PageSection> sections;
    if (nullptr == m_page || text.isEmpty()) {
        return sections;
    }

    rofd_find_options_t options;
    rofd_find_options_init(&options, sizeof(options));
    if (matchCase) {
        options.flags |= ROFD_FIND_CASE_SENSITIVE;
    }
    if (wholeWords) {
        options.flags |= ROFD_FIND_WHOLE_WORDS;
    }

    const QByteArray query = text.toUtf8();
    rofd_text_search_t *searchResult = nullptr;
    rofd_error_t *error = nullptr;
    rofd_status_t status = rofd_page_find_text_with_options(m_page,
                                                            query.constData(),
                                                            &options,
                                                            &searchResult,
                                                            &error);
    if (ROFD_STATUS_OK != status || nullptr == searchResult) {
        logSemanticError("Text search", m_pageIndex, status, error);
        rofd_text_search_free(searchResult);
        return sections;
    }
    rofd_error_free(error);

    size_t matchCount = 0;
    error = nullptr;
    status = rofd_text_search_get_count(searchResult, &matchCount, &error);
    if (ROFD_STATUS_OK != status) {
        logSemanticError("Search result enumeration", m_pageIndex, status, error);
        rofd_text_search_free(searchResult);
        return sections;
    }
    rofd_error_free(error);

    sections.reserve(static_cast<int>(qMin(matchCount,
                                           static_cast<size_t>(std::numeric_limits<int>::max()))));
    for (size_t index = 0; index < matchCount; ++index) {
        rofd_text_match_t match;
        match.struct_size = sizeof(match);
        error = nullptr;
        status = rofd_text_search_get_match(searchResult, index, &match, &error);
        if (ROFD_STATUS_OK != status) {
            logSemanticError("Search match extraction", m_pageIndex, status, error);
            continue;
        }
        rofd_error_free(error);

        const QRectF matchRect = toPixels(match.rect_mm);
        if (matchRect.isValid()) {
            sections.append(PageSection{PageLine{QString(), matchRect}});
        }
    }

    rofd_text_search_free(searchResult);
    return sections;
}

QList<Word> OfdPage::words()
{
    QList<Word> words;
    if (nullptr == m_page) {
        return words;
    }

    rofd_string_t *pageText = nullptr;
    rofd_error_t *error = nullptr;
    rofd_status_t status = rofd_page_get_text(m_page, &pageText, &error);
    if (ROFD_STATUS_OK != status || nullptr == pageText) {
        logSemanticError("Page text extraction", m_pageIndex, status, error);
        rofd_string_free(pageText);
        return words;
    }
    rofd_error_free(error);

    rofd_text_layout_t *layout = nullptr;
    error = nullptr;
    status = rofd_page_get_text_layout(m_page, &layout, &error);
    if (ROFD_STATUS_OK != status || nullptr == layout) {
        logSemanticError("Text layout extraction", m_pageIndex, status, error);
        rofd_string_free(pageText);
        rofd_text_layout_free(layout);
        return words;
    }
    rofd_error_free(error);

    size_t characterCount = 0;
    error = nullptr;
    status = rofd_text_layout_get_count(layout, &characterCount, &error);
    if (ROFD_STATUS_OK != status) {
        logSemanticError("Text layout enumeration", m_pageIndex, status, error);
        rofd_string_free(pageText);
        rofd_text_layout_free(layout);
        return words;
    }
    rofd_error_free(error);

    const char *utf8 = rofd_string_get_data(pageText);
    const size_t utf8Length = rofd_string_get_length(pageText);
    for (size_t index = 0; index < characterCount; ++index) {
        rofd_text_char_t character;
        character.struct_size = sizeof(character);
        error = nullptr;
        status = rofd_text_layout_get_char(layout, index, &character, &error);
        if (ROFD_STATUS_OK != status) {
            logSemanticError("Text character extraction", m_pageIndex, status, error);
            continue;
        }
        rofd_error_free(error);

        if (character.flags & ROFD_TEXT_CHAR_SYNTHESIZED_SEPARATOR) {
            continue;
        }
        if (nullptr == utf8
            || 0 == character.utf8_length
            || character.utf8_offset > utf8Length
            || character.utf8_length > utf8Length - character.utf8_offset
            || character.utf8_length > static_cast<size_t>(std::numeric_limits<int>::max())) {
            qCWarning(appLog) << "Invalid OFD text character span, page:" << m_pageIndex
                              << "index:" << index;
            continue;
        }

        const QRectF boundingBox = toPixels(character.rect_mm);
        if (!boundingBox.isValid()) {
            continue;
        }

        words.append(Word(QString::fromUtf8(utf8 + character.utf8_offset,
                                            static_cast<int>(character.utf8_length)),
                          boundingBox));
    }

    rofd_string_free(pageText);
    rofd_text_layout_free(layout);
    return words;
}

rofd_rect_t OfdPage::toMillimetres(const QRectF &rect) const
{
    const qreal xScale = kMillimetresPerInch / m_document->xRes();
    const qreal yScale = kMillimetresPerInch / m_document->yRes();
    return rofd_rect_t{m_pageRectMm.x_mm + rect.x() * xScale,
                       m_pageRectMm.y_mm + rect.y() * yScale,
                       rect.width() * xScale,
                       rect.height() * yScale};
}

QRectF OfdPage::toPixels(const rofd_rect_t &rect) const
{
    const qreal xScale = m_document->xRes() / kMillimetresPerInch;
    const qreal yScale = m_document->yRes() / kMillimetresPerInch;
    return QRectF((rect.x_mm - m_pageRectMm.x_mm) * xScale,
                  (rect.y_mm - m_pageRectMm.y_mm) * yScale,
                  rect.width_mm * xScale,
                  rect.height_mm * yScale);
}

} // namespace deepin_reader

#endif // OFD_SUPPORT_ENABLED

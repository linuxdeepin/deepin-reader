// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OFDMODEL_H
#define OFDMODEL_H

#include "Model.h"

#ifdef OFD_SUPPORT_ENABLED

#include <QImage>
#include <QMutex>
#include <QSizeF>

#include <rofd.h>

namespace deepin_reader {

class OfdPage;

class OfdDocument : public Document
{
    Q_OBJECT
public:
    static OfdDocument *loadDocument(const QString &filePath, Document::Error &error);

    ~OfdDocument() override;

    int pageCount() const override;
    Page *page(int index) const override;
    QStringList saveFilter() const override;
    bool save() const override;
    bool saveAs(const QString &filePath) const override;
    Outline outline() const override;
    Properties properties() const override;
    QString fileIdentifier() const override;

    QString filePath() const { return m_filePath; }
    qreal xRes() const { return m_xRes; }
    qreal yRes() const { return m_yRes; }

    // 供 OfdPage 使用的渲染接口
    QImage renderPage(rofd_page_t *pageHandle, int width, int height, const QRect &slice) const;

private:
    friend class OfdPage;

    OfdDocument(const QString &filePath, rofd_document_t *document, rofd_renderer_t *renderer);
    void loadMetadata();
    QVariantList warningDetails() const;
    // Shared by outline and page-link snapshots. All borrowed action strings
    // are copied; GOTO callers supply their snapshot's destination record.
    std::optional<NavigationTarget> navigationTarget(const rofd_action_t &action,
                                                     const rofd_destination_t *destination = nullptr) const;
    std::optional<rofd_rect_t> navigationPageRect(int pageIndex) const;

    QString m_filePath;
    rofd_document_t *m_document = nullptr;
    rofd_renderer_t *m_renderer = nullptr;
    Properties m_properties;
    mutable QMutex m_warningMutex;
    mutable size_t m_loggedWarningCount = 0;
    mutable QMutex m_outlineMutex;
    mutable bool m_outlineLoaded = false;
    mutable Outline m_outline;
    mutable QMutex m_navigationGeometryMutex;
    mutable QMap<int, std::optional<rofd_rect_t>> m_navigationPageRects;
    int m_pageCount = 0;
    qreal m_xRes = 96.0;
    qreal m_yRes = 96.0;
};

class OfdPage : public Page
{
    Q_OBJECT
public:
    OfdPage(const OfdDocument *document, rofd_page_t *pageHandle, int pageIndex);
    ~OfdPage() override;

    QSizeF sizeF() const override;
    QImage render(int width, int height, const QRect &slice = QRect()) const override;
    QString text(const QRectF &rect) const override;
    QVector<PageSection> search(const QString &text, bool matchCase, bool wholeWords) const override;
    QList<Word> words() override;
    Link getLinkAtPoint(const QPointF &point) override;

private:
    rofd_rect_t toMillimetres(const QRectF &rect) const;
    QRectF toPixels(const rofd_rect_t &rect) const;

    const OfdDocument *m_document;
    rofd_page_t *m_page = nullptr;
    int m_pageIndex = -1;
    rofd_rect_t m_pageRectMm = {0.0, 0.0, 0.0, 0.0};
    QSizeF m_sizePixel;
    QMutex m_linksMutex;
    bool m_linksLoaded = false;
    QList<Link> m_links;
};

} // namespace deepin_reader

#endif // OFD_SUPPORT_ENABLED

#endif // OFDMODEL_H

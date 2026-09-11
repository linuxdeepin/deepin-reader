// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOCUMENTNAVIGATION_H
#define DOCUMENTNAVIGATION_H

#include <QMetaType>
#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QString>
#include <QUrl>

#include <optional>

namespace deepin_reader {

enum class DestinationMode { XYZ, Fit, FitH, FitV, FitR };

struct NavigationDestination {
    // Zero-based page index and unscaled page coordinates. Missing fields retain
    // their presence semantics; in XYZ, absent or zero zoom keeps the current scale.
    int pageIndex = -1;
    DestinationMode mode = DestinationMode::XYZ;
    std::optional<qreal> left, top, right, bottom, zoom;

    bool isValid() const;
};

struct NavigationTarget {
    std::optional<NavigationDestination> destination;
    QUrl uri;

    bool isValid() const;
};

struct NavigationView {
    qreal scale = 1;
    // Unscaled page coordinates; a point is represented by a zero-size rectangle.
    QRectF focusRect;
};

QUrl resolveNavigationUri(const QString &uri, const QString &base = QString());

// Fits use rotated page bounds and half the viewport width in two-page mode.
// The returned scale is bounded to [0.1, maximumScale], and focusRect remains in
// unscaled page coordinates for the caller to map through the actual scene item.
std::optional<NavigationView> navigationView(const NavigationDestination &destination,
                                           const QSizeF &pageSize, const QSizeF &viewportSize,
                                           const QPointF &currentPosition, qreal currentScale,
                                           qreal maximumScale, int rotationDegrees, bool twoPages);

} // namespace deepin_reader

Q_DECLARE_METATYPE(deepin_reader::NavigationTarget)

#endif // DOCUMENTNAVIGATION_H

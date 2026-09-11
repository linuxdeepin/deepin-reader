// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Navigation.h"

#include <algorithm>
#include <cmath>

namespace deepin_reader {

namespace {

constexpr qreal minimumScale = 0.1;

bool finiteOptional(const std::optional<qreal> &value)
{
    return !value || std::isfinite(*value);
}

bool positiveFiniteSize(const QSizeF &size)
{
    return std::isfinite(size.width()) && size.width() > 0
        && std::isfinite(size.height()) && size.height() > 0;
}

bool allowedUri(const QUrl &uri)
{
    if (!uri.isValid() || uri.isEmpty() || uri.isRelative())
        return false;

    const QString scheme = uri.scheme();
    if (scheme == QLatin1String("http") || scheme == QLatin1String("https"))
        return !uri.host().isEmpty();
    if (scheme == QLatin1String("mailto"))
        return !uri.path().trimmed().isEmpty();
    return false;
}

} // namespace

bool NavigationDestination::isValid() const
{
    if (pageIndex < 0 || !finiteOptional(left) || !finiteOptional(top)
        || !finiteOptional(right) || !finiteOptional(bottom) || !finiteOptional(zoom)
        || (zoom && *zoom < 0)) {
        return false;
    }

    switch (mode) {
    case DestinationMode::XYZ:
    case DestinationMode::Fit:
    case DestinationMode::FitH:
    case DestinationMode::FitV:
        return true;
    case DestinationMode::FitR:
        return left && top && right && bottom && *right > *left && *bottom > *top
            && std::isfinite(*right - *left) && std::isfinite(*bottom - *top);
    }
    return false;
}

bool NavigationTarget::isValid() const
{
    if (destination)
        return uri.isEmpty() && destination->isValid();
    return allowedUri(uri);
}

QUrl resolveNavigationUri(const QString &uri, const QString &base)
{
    if (uri.isEmpty())
        return {};

    QUrl resolved(uri, QUrl::StrictMode);
    if (!resolved.isValid())
        return {};
    if (resolved.isRelative()) {
        const QUrl baseUrl(base, QUrl::StrictMode);
        if (!allowedUri(baseUrl)
            || (baseUrl.scheme() != QLatin1String("http")
                && baseUrl.scheme() != QLatin1String("https"))) {
            return {};
        }
        resolved = baseUrl.resolved(resolved);
    }
    return allowedUri(resolved) ? resolved : QUrl();
}

std::optional<NavigationView> navigationView(const NavigationDestination &destination,
                                           const QSizeF &pageSize, const QSizeF &viewportSize,
                                           const QPointF &currentPosition, qreal currentScale,
                                           qreal maximumScale, int rotationDegrees, bool twoPages)
{
    if (!destination.isValid() || !positiveFiniteSize(pageSize) || !positiveFiniteSize(viewportSize)
        || !std::isfinite(currentPosition.x()) || !std::isfinite(currentPosition.y())
        || !std::isfinite(currentScale) || currentScale <= 0
        || !std::isfinite(maximumScale) || maximumScale < minimumScale
        || rotationDegrees % 90 != 0) {
        return std::nullopt;
    }

    const qreal availableWidth = viewportSize.width() / (twoPages ? 2 : 1);
    if (availableWidth <= 0)
        return std::nullopt;

    const bool quarterTurn = rotationDegrees % 180 != 0;
    const qreal pageWidth = quarterTurn ? pageSize.height() : pageSize.width();
    const qreal pageHeight = quarterTurn ? pageSize.width() : pageSize.height();
    QPointF position(destination.left.value_or(currentPosition.x()),
                     destination.top.value_or(currentPosition.y()));
    NavigationView result;
    result.scale = currentScale;

    switch (destination.mode) {
    case DestinationMode::XYZ:
        if (destination.zoom && *destination.zoom > 0)
            result.scale = *destination.zoom;
        break;
    case DestinationMode::Fit:
        result.scale = std::min(availableWidth / pageWidth, viewportSize.height() / pageHeight);
        result.focusRect = QRectF(QPointF(), pageSize);
        break;
    case DestinationMode::FitH:
        result.scale = availableWidth / pageWidth;
        position.setX(0);
        break;
    case DestinationMode::FitV:
        result.scale = viewportSize.height() / pageHeight;
        position.setY(0);
        break;
    case DestinationMode::FitR: {
        const qreal width = *destination.right - *destination.left;
        const qreal height = *destination.bottom - *destination.top;
        result.scale = std::min(availableWidth / (quarterTurn ? height : width),
                                viewportSize.height() / (quarterTurn ? width : height));
        result.focusRect = QRectF(*destination.left, *destination.top, width, height);
        break;
    }
    }

    if (destination.mode == DestinationMode::XYZ || destination.mode == DestinationMode::FitH
        || destination.mode == DestinationMode::FitV) {
        position.setX(std::clamp(position.x(), qreal(0), pageSize.width()));
        position.setY(std::clamp(position.y(), qreal(0), pageSize.height()));
        result.focusRect = QRectF(position, QSizeF(0, 0));
    }
    // Finite positive inputs can still overflow a fitting ratio; the reader limit
    // bounds that ratio before it reaches any caller or graphics transform.
    result.scale = std::clamp(result.scale, minimumScale, maximumScale);
    return result;
}

} // namespace deepin_reader

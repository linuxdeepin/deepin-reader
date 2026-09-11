// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Navigation.h"
#include "Model.h"

#include <QVariant>
#include <gtest/gtest.h>
#include <limits>

using namespace deepin_reader;

namespace {

NavigationDestination destination(DestinationMode mode = DestinationMode::XYZ)
{
    NavigationDestination result;
    result.pageIndex = 0;
    result.mode = mode;
    return result;
}

std::optional<NavigationView> view(const NavigationDestination &target, int rotation = 0,
                                   bool twoPages = false)
{
    return navigationView(target, QSizeF(200, 400), QSizeF(800, 600), QPointF(31, 47),
                          1.25, 10, rotation, twoPages);
}

constexpr qreal infinity = std::numeric_limits<qreal>::infinity();
constexpr qreal notANumber = std::numeric_limits<qreal>::quiet_NaN();

} // namespace

TEST(NavigationValue, preservesMissingAndExplicitZeroFields)
{
    NavigationDestination target;
    EXPECT_EQ(target.pageIndex, -1);
    EXPECT_EQ(target.mode, DestinationMode::XYZ);
    EXPECT_FALSE(target.isValid());
    EXPECT_FALSE(target.left.has_value());
    EXPECT_FALSE(target.top.has_value());
    EXPECT_FALSE(target.right.has_value());
    EXPECT_FALSE(target.bottom.has_value());
    EXPECT_FALSE(target.zoom.has_value());

    target = destination();
    EXPECT_TRUE(target.isValid());
    target.left = target.top = target.zoom = 0;
    EXPECT_TRUE(target.isValid());
    EXPECT_EQ(target.left, std::optional<qreal>(0));
    EXPECT_EQ(target.top, std::optional<qreal>(0));
    EXPECT_EQ(target.zoom, std::optional<qreal>(0));
}

TEST(NavigationValue, validatesEveryModeAndRejectsUnknownMode)
{
    for (auto mode : {DestinationMode::XYZ, DestinationMode::Fit,
                      DestinationMode::FitH, DestinationMode::FitV}) {
        EXPECT_TRUE(destination(mode).isValid());
    }
    EXPECT_FALSE(destination(static_cast<DestinationMode>(999)).isValid());
    auto target = destination();
    target.pageIndex = -2;
    EXPECT_FALSE(target.isValid());
}

TEST(NavigationValue, fitRectangleRequiresFourOrderedFiniteEdges)
{
    auto target = destination(DestinationMode::FitR);
    target.left = 10;
    target.top = 20;
    target.right = 110;
    target.bottom = 220;
    ASSERT_TRUE(target.isValid());
    for (auto field : {&NavigationDestination::left, &NavigationDestination::top,
                       &NavigationDestination::right, &NavigationDestination::bottom}) {
        auto incomplete = target;
        (incomplete.*field).reset();
        EXPECT_FALSE(incomplete.isValid());
    }
    auto invalid = target;
    invalid.right = 10;
    EXPECT_FALSE(invalid.isValid());
    invalid.right = 9;
    EXPECT_FALSE(invalid.isValid());
    invalid = target;
    invalid.bottom = 20;
    EXPECT_FALSE(invalid.isValid());
    invalid.bottom = 19;
    EXPECT_FALSE(invalid.isValid());
    invalid.left = -std::numeric_limits<qreal>::max();
    invalid.right = std::numeric_limits<qreal>::max();
    invalid.bottom = 220;
    EXPECT_FALSE(invalid.isValid());
}

TEST(NavigationValue, rejectsNonfiniteFieldsAndNegativeZoom)
{
    for (auto mode : {DestinationMode::XYZ, DestinationMode::Fit,
                      DestinationMode::FitH, DestinationMode::FitV, DestinationMode::FitR}) {
        auto target = destination(mode);
        target.left = 0;
        target.top = 0;
        target.right = 100;
        target.bottom = 200;
        ASSERT_TRUE(target.isValid());
        for (auto field : {&NavigationDestination::left, &NavigationDestination::top,
                           &NavigationDestination::right, &NavigationDestination::bottom,
                           &NavigationDestination::zoom}) {
            for (qreal value : {notANumber, infinity, -infinity}) {
                auto invalid = target;
                invalid.*field = value;
                EXPECT_FALSE(invalid.isValid());
            }
        }
        target.zoom = -0.5;
        EXPECT_FALSE(target.isValid());
    }
}

TEST(NavigationValue, targetRequiresExactlyOneValidAction)
{
    NavigationTarget target;
    EXPECT_FALSE(target.isValid());
    target.destination = destination();
    EXPECT_TRUE(target.isValid());
    target.uri = QUrl(QStringLiteral("https://example.org/document"));
    EXPECT_FALSE(target.isValid());
    target.destination.reset();
    EXPECT_TRUE(target.isValid());
    target.uri = QUrl(QStringLiteral("file:///tmp/document.ofd"));
    EXPECT_FALSE(target.isValid());
    target.uri = QUrl(QStringLiteral("next.html"));
    EXPECT_FALSE(target.isValid());
    target.uri.clear();
    target.destination = NavigationDestination();
    EXPECT_FALSE(target.isValid());
}

TEST(NavigationValue, targetCanRoundTripThroughItemData)
{
    NavigationTarget target;
    target.destination = destination(DestinationMode::FitH);
    target.destination->top = 0;
    const auto restored = QVariant::fromValue(target).value<NavigationTarget>();
    ASSERT_TRUE(restored.destination.has_value());
    EXPECT_TRUE(restored.isValid());
    EXPECT_EQ(restored.destination->mode, DestinationMode::FitH);
    EXPECT_EQ(restored.destination->top, std::optional<qreal>(0));
    EXPECT_FALSE(restored.destination->left.has_value());
}

TEST(NavigationView, xyzRetainsOmittedPositionAndScale)
{
    const auto target = destination();
    const auto result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 1.25);
    EXPECT_EQ(result->focusRect, QRectF(31, 47, 0, 0));
    EXPECT_FALSE(target.left.has_value());
    EXPECT_FALSE(target.top.has_value());
    EXPECT_FALSE(target.zoom.has_value());
}

TEST(NavigationView, xyzDistinguishesZeroPositionFromMissingPosition)
{
    auto target = destination();
    target.left = 0;
    target.zoom = 0;
    auto result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 1.25);
    EXPECT_EQ(result->focusRect, QRectF(0, 47, 0, 0));
    target.left.reset();
    target.top = 0;
    result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(31, 0, 0, 0));
    target.left = 0;
    target.zoom = 2;
    result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 2);
    EXPECT_EQ(result->focusRect, QRectF(0, 0, 0, 0));
}

TEST(NavigationView, xyzClampsLogicalPointToPageBounds)
{
    auto target = destination();
    target.left = -12;
    target.top = 450;
    const auto result = view(target, 270);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(0, 400, 0, 0));
    const auto retained = navigationView(destination(), QSizeF(200, 400), QSizeF(800, 600),
                                         QPointF(500, -1), 1, 10, 0, false);
    ASSERT_TRUE(retained.has_value());
    EXPECT_EQ(retained->focusRect, QRectF(200, 0, 0, 0));
}

TEST(NavigationView, fitUsesEntireLogicalPageAndRotatedBounds)
{
    for (int rotation : {0, 90, 180, 270, -90, 450}) {
        const auto result = view(destination(DestinationMode::Fit), rotation);
        ASSERT_TRUE(result.has_value());
        EXPECT_DOUBLE_EQ(result->scale, rotation % 180 == 0 ? 1.5 : 2.0);
        EXPECT_EQ(result->focusRect, QRectF(0, 0, 200, 400));
    }
}

TEST(NavigationView, fitHUsesRotatedWidthAndOptionalTop)
{
    auto target = destination(DestinationMode::FitH);
    for (int rotation : {0, 90, 180, 270}) {
        const auto result = view(target, rotation);
        ASSERT_TRUE(result.has_value());
        EXPECT_DOUBLE_EQ(result->scale, rotation % 180 == 0 ? 4.0 : 2.0);
        EXPECT_EQ(result->focusRect, QRectF(0, 47, 0, 0));
    }
    target.top = 0;
    target.left = 91;
    auto result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(0, 0, 0, 0));
    target.top = 999;
    result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(0, 400, 0, 0));
}

TEST(NavigationView, fitVUsesRotatedHeightAndOptionalLeft)
{
    auto target = destination(DestinationMode::FitV);
    for (int rotation : {0, 90, 180, 270}) {
        const auto result = view(target, rotation);
        ASSERT_TRUE(result.has_value());
        EXPECT_DOUBLE_EQ(result->scale, rotation % 180 == 0 ? 1.5 : 3.0);
        EXPECT_EQ(result->focusRect, QRectF(31, 0, 0, 0));
    }
    target.left = 0;
    target.top = 91;
    auto result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(0, 0, 0, 0));
    target.left = -5;
    result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->focusRect, QRectF(0, 0, 0, 0));
}

TEST(NavigationView, fitRUsesEntireLogicalRegionAndRotatedBounds)
{
    auto target = destination(DestinationMode::FitR);
    target.left = 20;
    target.top = 30;
    target.right = 120;
    target.bottom = 230;
    for (int rotation : {0, 90, 180, 270}) {
        const auto result = view(target, rotation);
        ASSERT_TRUE(result.has_value());
        EXPECT_DOUBLE_EQ(result->scale, rotation % 180 == 0 ? 3.0 : 4.0);
        EXPECT_EQ(result->focusRect, QRectF(20, 30, 100, 200));
    }
    target.bottom.reset();
    EXPECT_FALSE(view(target).has_value());
    target.bottom = 30;
    EXPECT_FALSE(view(target).has_value());
}

TEST(NavigationView, twoPagesAllocateHalfViewportWidth)
{
    auto result = view(destination(DestinationMode::Fit), 90, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 1);
    result = view(destination(DestinationMode::FitH), 0, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 2);
    result = view(destination(DestinationMode::FitV), 0, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 1.5);
    auto target = destination(DestinationMode::FitR);
    target.left = target.top = 0;
    target.right = 200;
    target.bottom = 100;
    result = view(target, 0, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 2);
    result = view(destination(), 0, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 1.25);
}

TEST(NavigationView, clampsScalesToReaderLimits)
{
    auto target = destination();
    target.zoom = 0.001;
    auto result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 0.1);
    target.zoom = 100;
    result = view(target);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 10);
    result = navigationView(destination(DestinationMode::Fit), QSizeF(10000, 10000),
                            QSizeF(10, 10), QPointF(), 1, 10, 0, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 0.1);
    result = navigationView(destination(DestinationMode::Fit), QSizeF(1, 1),
                            QSizeF(1000, 1000), QPointF(), 1, 2, 0, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 2);
    result = navigationView(destination(), QSizeF(200, 400), QSizeF(800, 600),
                            QPointF(), 20, 0.1, 0, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->scale, 0.1);
}

TEST(NavigationView, rejectsInvalidPageAndViewportSizes)
{
    for (const auto &size : {QSizeF(), QSizeF(0, 1), QSizeF(1, 0), QSizeF(-1, 1),
                             QSizeF(1, -1), QSizeF(notANumber, 1), QSizeF(1, notANumber),
                             QSizeF(infinity, 1), QSizeF(1, infinity)}) {
        EXPECT_FALSE(navigationView(destination(), size, QSizeF(800, 600), QPointF(),
                                    1, 10, 0, false).has_value());
        EXPECT_FALSE(navigationView(destination(), QSizeF(200, 400), size, QPointF(),
                                    1, 10, 0, false).has_value());
    }
}

TEST(NavigationView, rejectsInvalidCurrentStateLimitsAndRotation)
{
    for (qreal value : {qreal(0), qreal(-1), notANumber, infinity}) {
        EXPECT_FALSE(navigationView(destination(), QSizeF(200, 400), QSizeF(800, 600),
                                    QPointF(), value, 10, 0, false).has_value());
    }
    for (qreal value : {qreal(0.09), qreal(0), qreal(-1), notANumber, infinity}) {
        EXPECT_FALSE(navigationView(destination(), QSizeF(200, 400), QSizeF(800, 600),
                                    QPointF(), 1, value, 0, false).has_value());
    }
    for (const auto &point : {QPointF(notANumber, 0), QPointF(0, notANumber),
                              QPointF(infinity, 0), QPointF(0, infinity)}) {
        EXPECT_FALSE(navigationView(destination(), QSizeF(200, 400), QSizeF(800, 600),
                                    point, 1, 10, 0, false).has_value());
    }
    EXPECT_FALSE(view(destination(), 45).has_value());
    EXPECT_FALSE(view(destination(), -1).has_value());
    EXPECT_FALSE(view(NavigationDestination()).has_value());
    EXPECT_FALSE(view(destination(static_cast<DestinationMode>(999))).has_value());
}

TEST(NavigationUri, acceptsAllowedAbsoluteTargets)
{
    for (const auto &uri : {QStringLiteral("http://example.org/"),
                            QStringLiteral("https://example.org:443/a%20b?q=x#section"),
                            QStringLiteral("mailto:reader@example.org"),
                            QStringLiteral("mailto:reader@example.org?subject=Document")}) {
        const QUrl result = resolveNavigationUri(uri);
        EXPECT_EQ(result, QUrl(uri, QUrl::StrictMode));
        NavigationTarget target;
        target.uri = result;
        EXPECT_TRUE(target.isValid());
    }
}

TEST(NavigationUri, resolvesRelativeTargetsOnlyWithUsableExplicitBase)
{
    EXPECT_EQ(resolveNavigationUri("../next.html#heading", "https://example.org/docs/book/"),
              QUrl("https://example.org/docs/next.html#heading"));
    EXPECT_EQ(resolveNavigationUri("/page", "http://example.org/docs/book"),
              QUrl("http://example.org/page"));
    EXPECT_EQ(resolveNavigationUri("#part", "https://example.org/document.html"),
              QUrl("https://example.org/document.html#part"));
    EXPECT_EQ(resolveNavigationUri("//cdn.example.org/image", "https://example.org/"),
              QUrl("https://cdn.example.org/image"));
    EXPECT_TRUE(resolveNavigationUri("next.html").isEmpty());
    EXPECT_TRUE(resolveNavigationUri("next.html", "docs/").isEmpty());
    EXPECT_TRUE(resolveNavigationUri("next.html", "file:///tmp/docs/").isEmpty());
    EXPECT_TRUE(resolveNavigationUri("next.html", "mailto:user@example.org").isEmpty());
    EXPECT_TRUE(resolveNavigationUri("next.html", "https://").isEmpty());
}

TEST(NavigationUri, rejectsUnsafeMalformedAndEmptyTargets)
{
    for (const auto &uri : {QString(), QStringLiteral("file:///tmp/document.ofd"),
                            QStringLiteral("javascript:alert(1)"), QStringLiteral("data:text/plain,hi"),
                            QStringLiteral("ftp://example.org/file"), QStringLiteral("custom:target"),
                            QStringLiteral("https://"), QStringLiteral("https:example.org"),
                            QStringLiteral("https:///path"), QStringLiteral("http://[bad]"),
                            QStringLiteral("http://example.org/%zz"), QStringLiteral("http://exa mple.org/"),
                            QStringLiteral("https://example.org/a b"), QStringLiteral("\nhttps://example.org/"),
                            QStringLiteral("mailto:"), QStringLiteral("mailto:?subject=Document"),
                            QStringLiteral("mailto:%20")}) {
        EXPECT_TRUE(resolveNavigationUri(uri).isEmpty()) << uri.toStdString();
        EXPECT_TRUE(resolveNavigationUri(uri, "https://example.org/docs/").isEmpty())
            << uri.toStdString();
    }
}

TEST(NavigationCompatibility, preservesLegacyLinkDefaultsAndValidity)
{
    const Link empty;
    EXPECT_EQ(empty.page, -1);
    EXPECT_DOUBLE_EQ(empty.left, 0);
    EXPECT_DOUBLE_EQ(empty.top, 0);
    EXPECT_TRUE(empty.urlOrFileName.isEmpty());
    EXPECT_FALSE(empty.navigation.has_value());
    EXPECT_FALSE(empty.isValid());
    EXPECT_FALSE(Link(QRectF(1, 2, 3, 4), 0).isValid());
    const Link page(QRectF(1, 2, 3, 4), 1);
    EXPECT_TRUE(page.isValid());
    EXPECT_FALSE(page.navigation.has_value());
    EXPECT_TRUE(Link(QPainterPath(), QStringLiteral("legacy-local.pdf")).isValid());
    const Section section;
    EXPECT_EQ(section.nIndex, -1);
    EXPECT_EQ(section.offsetPointF, QPointF());
    EXPECT_FALSE(section.navigation.has_value());
    EXPECT_FALSE(section.expanded.has_value());
}

TEST(NavigationCompatibility, typedTargetTakesExclusivePrecedenceOverLegacyFields)
{
    Link link(QPainterPath(), QStringLiteral("https://legacy.example.org"));
    link.page = 1;
    link.navigation = NavigationTarget();
    EXPECT_FALSE(link.isValid());
    link.navigation->uri = QUrl(QStringLiteral("file:///tmp/document.ofd"));
    EXPECT_FALSE(link.isValid());
    link.navigation->uri.clear();
    link.navigation->destination = destination();
    link.page = -1;
    link.urlOrFileName.clear();
    EXPECT_TRUE(link.isValid());
    link.navigation.reset();
    EXPECT_FALSE(link.isValid());
}

// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QDebug>
#include <DStyledItemDelegate>
#include <QPixmap>
#include <QListView>

// Exercise the production thumbnail cache without linking the full reader.
#define private public
#include "ThumbnailDelegate.h"
#undef private
#include "NightFilter.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QListView view;
    ThumbnailDelegate delegate(&view);
    int failures = 0;
    int checks = 0;
    const auto check = [&](bool result, const char *message) {
        ++checks;
        if (!result) {
            ++failures;
            qCritical() << message;
        }
    };
    check(delegate.nightPixmap(QPixmap()).isNull(), "Null thumbnail must stay null");

    for (qreal dpr : {1.0, 2.0}) {
        for (const QColor &color : {QColor(Qt::white), QColor(Qt::black),
                                   QColor(90, 140, 200), QColor(180, 80, 40, 128)}) {
            QPixmap source(24, 32);
            source.setDevicePixelRatio(dpr);
            source.fill(color);
            const QPixmap actual = delegate.nightPixmap(source);
            // BrowserPage also converts the filter result back to a QPixmap;
            // include Qt's premultiplied-alpha rounding in the comparison.
            const QImage expected = QPixmap::fromImage(NightFilter::applyPage(source.toImage(), {})).toImage();
            check(actual.toImage() == expected, "Thumbnail must use master NightFilter pixels");
            check(actual.devicePixelRatio() == dpr, "Thumbnail must preserve device pixel ratio");
            check(delegate.nightPixmap(source).cacheKey() == actual.cacheKey(),
                  "Unchanged source must reuse cached night thumbnail");
            check(delegate.nightPixmap(source).transformed(QTransform().rotate(90)).toImage()
                      == QPixmap::fromImage(expected).transformed(QTransform().rotate(90)).toImage(),
                  "Rotation must be applied after filtering");
            source.fill(Qt::green);
            check(delegate.nightPixmap(source).toImage() == NightFilter::applyPage(source.toImage(), {}),
                  "Changed source must invalidate night thumbnail cache");
        }
    }
    qInfo() << checks << "checks," << failures << "failures";
    return failures ? 1 : 0;
}

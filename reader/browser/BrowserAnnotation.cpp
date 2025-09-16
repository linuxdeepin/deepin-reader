// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserAnnotation.h"
#include "BrowserPage.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>
#include <QDebug>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

BrowserAnnotation::BrowserAnnotation(QGraphicsItem *parent, QRectF rect, deepin_reader::Annotation *annotation, qreal scalefactor) : QGraphicsItem(parent),
    m_annotation(annotation), m_rect(rect), m_parent(parent), m_scaleFactor(scalefactor)
{
    qCDebug(appLog) << "BrowserAnnotation::BrowserAnnotation() - Starting constructor";
    if (nullptr == m_annotation) {
        qCWarning(appLog) << "BrowserAnnotation: Invalid annotation pointer";
        return;
    }

    qCDebug(appLog) << "BrowserAnnotation created, type:" << m_annotation->type();

    if (m_annotation->type() == deepin_reader::Annotation::AText) {
        qCDebug(appLog) << "BrowserAnnotation::BrowserAnnotation() - Setting Z_ORDER_ICON for text annotation";
        setZValue(deepin_reader::Z_ORDER_ICON);
    } else {
        qCDebug(appLog) << "BrowserAnnotation::BrowserAnnotation() - Setting Z_ORDER_HIGHLIGHT for other annotation";
        setZValue(deepin_reader::Z_ORDER_HIGHLIGHT);
    }
    qCDebug(appLog) << "BrowserAnnotation::BrowserAnnotation() - Constructor completed";
}

BrowserAnnotation::~BrowserAnnotation()
{
    // qCDebug(appLog) << "BrowserAnnotation destroyed";
    m_annotation = nullptr;
}

int BrowserAnnotation::annotationType()
{
    // qCDebug(appLog) << "BrowserAnnotation::annotationType() - Getting annotation type";
    if (nullptr == m_annotation) {
        // qCDebug(appLog) << "BrowserAnnotation::annotationType() - Annotation is null, returning -1";
        return -1;
    }

    int type = m_annotation->type();
    // qCDebug(appLog) << "BrowserAnnotation::annotationType() - Annotation type:" << type;
    return type;
}

QString BrowserAnnotation::annotationText()
{
    // qCDebug(appLog) << "BrowserAnnotation::annotationText() - Getting annotation text";
    if (nullptr == m_annotation) {
        // qCDebug(appLog) << "BrowserAnnotation::annotationText() - Annotation is null, returning empty string";
        return QString();
    }

    QString text = m_annotation->contents();
    // qCDebug(appLog) << "BrowserAnnotation::annotationText() - Annotation text length:" << text.length();
    return text;
}

QRectF BrowserAnnotation::boundingRect() const
{
    // qCDebug(appLog) << "BrowserAnnotation::boundingRect() - Calculating bounding rectangle";
    QRectF rect = QRectF(m_rect.x() * m_scaleFactor,
                  m_rect.y() * m_scaleFactor,
                  m_rect.width() * m_scaleFactor,
                  m_rect.height() * m_scaleFactor);
    // qCDebug(appLog) << "BrowserAnnotation::boundingRect() - Bounding rectangle:" << rect;
    return rect;
}

void BrowserAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // qCDebug(appLog) << "BrowserAnnotation::paint() - Starting paint operation";
    if (m_drawSelectRect) {
        // qCDebug(appLog) << "BrowserAnnotation::paint() - Drawing selection rectangle";
        QPen pen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
        painter->setPen(pen);
        painter->drawRect(option->rect);
    }
    // qCDebug(appLog) << "BrowserAnnotation::paint() - Paint operation completed";
}

deepin_reader::Annotation *BrowserAnnotation::annotation()
{
    // qCDebug(appLog) << "BrowserAnnotation::annotation() - Returning annotation pointer";
    return m_annotation;
}

bool BrowserAnnotation::isSame(Annotation *annotation)
{
    // qCDebug(appLog) << "BrowserAnnotation::isSame() - Comparing annotations";
    bool isSame = (annotation == m_annotation);
    // qCDebug(appLog) << "BrowserAnnotation::isSame() - Annotations are same:" << isSame;
    return isSame;
}

void BrowserAnnotation::setDrawSelectRect(const bool draw)
{
    // qCDebug(appLog) << "BrowserAnnotation::setDrawSelectRect:" << draw;
    if (nullptr != m_annotation && m_annotation->type() != 1) {
        // qCDebug(appLog) << "BrowserAnnotation::setDrawSelectRect() - Annotation type is not 1, setting to false";
        m_drawSelectRect = false;
        update();
        return;
    }

    // qCDebug(appLog) << "BrowserAnnotation::setDrawSelectRect() - Setting draw select rect to:" << draw;
    m_drawSelectRect = draw;

    update();
    // qCDebug(appLog) << "BrowserAnnotation::setDrawSelectRect() - Update called";
}

void BrowserAnnotation::setScaleFactor(const double scale)
{
    // qCDebug(appLog) << "BrowserAnnotation::setScaleFactor() - Setting scale factor to:" << scale;
    if (nullptr == m_annotation) {
        // qCDebug(appLog) << "BrowserAnnotation::setScaleFactor() - Annotation is null, returning";
        return;
    }

    m_scaleFactor = scale;
    // qCDebug(appLog) << "BrowserAnnotation::setScaleFactor() - Scale factor set, calling update";

    update();
    // qCDebug(appLog) << "BrowserAnnotation::setScaleFactor() - Update called";
}

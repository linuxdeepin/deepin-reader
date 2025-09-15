// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserAnnotation.h"
#include "BrowserPage.h"

#include <DGuiApplicationHelper>
#include <QDebug>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

BrowserAnnotation::BrowserAnnotation(QGraphicsItem *parent, QRectF rect, deepin_reader::Annotation *annotation, qreal scalefactor) : QGraphicsItem(parent),
    m_annotation(annotation), m_rect(rect), m_parent(parent), m_scaleFactor(scalefactor)
{
    qDebug() << "BrowserAnnotation::BrowserAnnotation() - Starting constructor";
    if (nullptr == m_annotation) {
        qWarning() << "BrowserAnnotation: Invalid annotation pointer";
        return;
    }

    qDebug() << "BrowserAnnotation created, type:" << m_annotation->type();

    if (m_annotation->type() == deepin_reader::Annotation::AText) {
        qDebug() << "BrowserAnnotation::BrowserAnnotation() - Setting Z_ORDER_ICON for text annotation";
        setZValue(deepin_reader::Z_ORDER_ICON);
    } else {
        qDebug() << "BrowserAnnotation::BrowserAnnotation() - Setting Z_ORDER_HIGHLIGHT for other annotation";
        setZValue(deepin_reader::Z_ORDER_HIGHLIGHT);
    }
    qDebug() << "BrowserAnnotation::BrowserAnnotation() - Constructor completed";
}

BrowserAnnotation::~BrowserAnnotation()
{
    // qDebug() << "BrowserAnnotation destroyed";
    m_annotation = nullptr;
}

int BrowserAnnotation::annotationType()
{
    // qDebug() << "BrowserAnnotation::annotationType() - Getting annotation type";
    if (nullptr == m_annotation) {
        // qDebug() << "BrowserAnnotation::annotationType() - Annotation is null, returning -1";
        return -1;
    }

    int type = m_annotation->type();
    // qDebug() << "BrowserAnnotation::annotationType() - Annotation type:" << type;
    return type;
}

QString BrowserAnnotation::annotationText()
{
    // qDebug() << "BrowserAnnotation::annotationText() - Getting annotation text";
    if (nullptr == m_annotation) {
        // qDebug() << "BrowserAnnotation::annotationText() - Annotation is null, returning empty string";
        return QString();
    }

    QString text = m_annotation->contents();
    // qDebug() << "BrowserAnnotation::annotationText() - Annotation text length:" << text.length();
    return text;
}

QRectF BrowserAnnotation::boundingRect() const
{
    // qDebug() << "BrowserAnnotation::boundingRect() - Calculating bounding rectangle";
    QRectF rect = QRectF(m_rect.x() * m_scaleFactor,
                  m_rect.y() * m_scaleFactor,
                  m_rect.width() * m_scaleFactor,
                  m_rect.height() * m_scaleFactor);
    // qDebug() << "BrowserAnnotation::boundingRect() - Bounding rectangle:" << rect;
    return rect;
}

void BrowserAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // qDebug() << "BrowserAnnotation::paint() - Starting paint operation";
    if (m_drawSelectRect) {
        // qDebug() << "BrowserAnnotation::paint() - Drawing selection rectangle";
        QPen pen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
        painter->setPen(pen);
        painter->drawRect(option->rect);
    }
    // qDebug() << "BrowserAnnotation::paint() - Paint operation completed";
}

deepin_reader::Annotation *BrowserAnnotation::annotation()
{
    // qDebug() << "BrowserAnnotation::annotation() - Returning annotation pointer";
    return m_annotation;
}

bool BrowserAnnotation::isSame(Annotation *annotation)
{
    // qDebug() << "BrowserAnnotation::isSame() - Comparing annotations";
    bool isSame = (annotation == m_annotation);
    // qDebug() << "BrowserAnnotation::isSame() - Annotations are same:" << isSame;
    return isSame;
}

void BrowserAnnotation::setDrawSelectRect(const bool draw)
{
    // qDebug() << "BrowserAnnotation::setDrawSelectRect:" << draw;
    if (nullptr != m_annotation && m_annotation->type() != 1) {
        // qDebug() << "BrowserAnnotation::setDrawSelectRect() - Annotation type is not 1, setting to false";
        m_drawSelectRect = false;
        update();
        return;
    }

    // qDebug() << "BrowserAnnotation::setDrawSelectRect() - Setting draw select rect to:" << draw;
    m_drawSelectRect = draw;

    update();
    // qDebug() << "BrowserAnnotation::setDrawSelectRect() - Update called";
}

void BrowserAnnotation::setScaleFactor(const double scale)
{
    // qDebug() << "BrowserAnnotation::setScaleFactor() - Setting scale factor to:" << scale;
    if (nullptr == m_annotation) {
        // qDebug() << "BrowserAnnotation::setScaleFactor() - Annotation is null, returning";
        return;
    }

    m_scaleFactor = scale;
    // qDebug() << "BrowserAnnotation::setScaleFactor() - Scale factor set, calling update";

    update();
    // qDebug() << "BrowserAnnotation::setScaleFactor() - Update called";
}

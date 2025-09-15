// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SheetRenderer.h"

#include "PageRenderThread.h"

#include <QDebug>

SheetRenderer::SheetRenderer(DocSheet *parent) : QObject(parent), m_sheet(parent)
{
    qDebug() << "Creating SheetRenderer for sheet:" << (parent ? parent->filePath() : "null");
}

SheetRenderer::~SheetRenderer()
{
    qDebug() << "正在添加关闭文档任务...";
    DocCloseTask task;

    task.document = m_document;

    task.pages = m_pages;

    PageRenderThread::appendTask(task);
    qDebug() << "关闭文档任务已添加";
}

bool SheetRenderer::openFileExec(const QString &password)
{
    qDebug() << "Executing synchronous file open";
    QEventLoop loop;

    connect(this, &SheetRenderer::sigOpened, &loop, &QEventLoop::quit);

    openFileAsync(password);

    loop.exec();

    bool success = deepin_reader::Document::NoError == m_error;
    if (!success) {
        qWarning() << "Failed to open file synchronously, error:" << m_error;
    }
    return success;
}

void SheetRenderer::openFileAsync(const QString &password)
{
    qDebug() << "Starting asynchronous file open";
    DocOpenTask task;

    task.sheet = m_sheet;

    task.password = password;

    task.renderer = this;

    PageRenderThread::appendTask(task);
    qDebug() << "SheetRenderer::openFileAsync end";
}

bool SheetRenderer::opened()
{
    qDebug() << "SheetRenderer::opened start";
    return m_document != nullptr;
}

int SheetRenderer::getPageCount()
{
    qDebug() << "SheetRenderer::getPageCount start";
    return m_pages.count();
}

QImage SheetRenderer::getImage(int index, int width, int height, const QRect &slice)
{
    qDebug() << "SheetRenderer::getImage start - index:" << index;
    if (m_pages.count() <= index) {
        qWarning() << "Invalid page index:" << index << "max pages:" << m_pages.count();
        return QImage();
    }

    QImage image = m_pages.value(index)->render(width, height, slice);
    qDebug() << "SheetRenderer::getImage end";
    return image;
}

deepin_reader::Link SheetRenderer::getLinkAtPoint(int index, const QPointF &point)
{
    // qDebug() << "SheetRenderer::getLinkAtPoint start - index:" << index << "point:" << point;
    if (m_pages.count() <= index)
        return deepin_reader::Link();

    // qDebug() << "SheetRenderer::getLinkAtPoint end";
    return  m_pages.value(index)->getLinkAtPoint(point);
}

QList<deepin_reader::Word> SheetRenderer::getWords(int index)
{
    // qDebug() << "SheetRenderer::getWords start - index:" << index;
    if (m_pages.count() <= index)
        return QList<deepin_reader::Word>();

    // qDebug() << "SheetRenderer::getWords end";
    return m_pages.value(index)->words();
}

QList<deepin_reader::Annotation *> SheetRenderer::getAnnotations(int index)
{
    // qDebug() << "SheetRenderer::getAnnotations start - index:" << index;
    if (m_pages.count() <= index)
        return QList<deepin_reader::Annotation *>();

    // qDebug() << "SheetRenderer::getAnnotations end";
    return m_pages.value(index)->annotations();
}

QSizeF SheetRenderer::getPageSize(int index) const
{
    // qDebug() << "SheetRenderer::getPageSize start - index:" << index;
    if (m_pages.count() <= index)
        return QSizeF();

    // qDebug() << "SheetRenderer::getPageSize end";
    return m_pages.value(index)->sizeF();
}

deepin_reader::Annotation *SheetRenderer::addHighlightAnnotation(int index, const QList<QRectF> &boundaries, const QString &text, const QColor &color)
{
    // qDebug() << "SheetRenderer::addHighlightAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    // qDebug() << "SheetRenderer::addHighlightAnnotation end";
    return m_pages.value(index)->addHighlightAnnotation(boundaries, text, color);
}

bool SheetRenderer::removeAnnotation(int index, deepin_reader::Annotation *annotation)
{
    // qDebug() << "SheetRenderer::removeAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    // qDebug() << "SheetRenderer::removeAnnotation end";
    return m_pages.value(index)->removeAnnotation(annotation);
}

bool SheetRenderer::updateAnnotation(int index, deepin_reader::Annotation *annotation, const QString &text, const QColor &color)
{
    qDebug() << "SheetRenderer::updateAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    qDebug() << "SheetRenderer::updateAnnotation end";
    return m_pages.value(index)->updateAnnotation(annotation, text, color);
}

deepin_reader::Annotation *SheetRenderer::addIconAnnotation(int index, const QRectF &rect, const QString &text)
{
    qDebug() << "SheetRenderer::addIconAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    qDebug() << "SheetRenderer::addIconAnnotation end";
    return m_pages.value(index)->addIconAnnotation(rect, text);
}

deepin_reader::Annotation *SheetRenderer::moveIconAnnotation(int index, deepin_reader::Annotation *annot, const QRectF &rect)
{
    qDebug() << "SheetRenderer::moveIconAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    qDebug() << "SheetRenderer::moveIconAnnotation end";
    return m_pages.value(index)->moveIconAnnotation(annot, rect);
}

QString SheetRenderer::getText(int index, const QRectF &rect)
{
    qDebug() << "SheetRenderer::getText start - index:" << index;
    if (m_pages.count() <= index)
        return QString();

    qDebug() << "SheetRenderer::getText end";
    return m_pages.value(index)->text(rect);
}

QVector<deepin_reader::PageSection> SheetRenderer::search(int index, const QString &text, bool matchCase, bool wholeWords)
{
    qDebug() << "SheetRenderer::search start - index:" << index;
    if (m_pages.count() <= index)
        return QVector<deepin_reader::PageSection>();

    qDebug() << "SheetRenderer::search end";
    return m_pages.value(index)->search(text, matchCase, wholeWords);
}

bool SheetRenderer::inLink(int index, const QPointF &pos)
{
    qDebug() << "SheetRenderer::inLink start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    deepin_reader::Link link = m_pages.value(index)->getLinkAtPoint(pos);

    qDebug() << "SheetRenderer::inLink end";
    return link.isValid();
}

bool SheetRenderer::hasWidgetAnnots(int index)
{
    qDebug() << "SheetRenderer::hasWidgetAnnots start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    qDebug() << "SheetRenderer::hasWidgetAnnots end";
    return m_pages.value(index)->hasWidgetAnnots();
}

deepin_reader::Outline SheetRenderer::outline()
{
    qDebug() << "SheetRenderer::outline start";
    if (m_document == nullptr)
        return deepin_reader::Outline();

    qDebug() << "SheetRenderer::outline end";
    return m_document->outline();
}

deepin_reader::Properties SheetRenderer::properties() const
{
    qDebug() << "SheetRenderer::properties start";
    if (m_document == nullptr)
        return deepin_reader::Properties();

    qDebug() << "SheetRenderer::properties end";
    return m_document->properties();
}

bool SheetRenderer::save()
{
    qDebug() << "Attempting to save document";
    if (m_document == nullptr) {
        qWarning() << "Cannot save - no document loaded";
        return false;
    }

    bool success = m_document->save();
    if (!success) {
        qWarning() << "Failed to save document";
    }
    qDebug() << "SheetRenderer::save end, success:" << success;
    return success;
}

void SheetRenderer::loadPageLable()
{
    qDebug() << "SheetRenderer::loadPageLable start";
    if (m_pageLabelLoaded || m_document == nullptr)
        return;

    m_pageLabelLoaded = true;

    m_lable2Page.clear();

    int pageCount = m_document->pageCount();

    for (int i = 0; i < pageCount; i++) {
        const QString &labelPage = m_document->label(i);

        if (!labelPage.isEmpty() && labelPage.toInt() != i + 1) {
            m_lable2Page.insert(labelPage, i);
        }
    }
    qDebug() << "SheetRenderer::loadPageLable end";
}

int SheetRenderer::pageLableIndex(const QString pageLable)
{
    qDebug() << "SheetRenderer::pageLableIndex start - pageLable:" << pageLable;
    if (m_lable2Page.count() <= 0 || !m_lable2Page.contains(pageLable))
        return -1;

    qDebug() << "SheetRenderer::pageLableIndex end";
    return m_lable2Page.value(pageLable);
}

bool SheetRenderer::pageHasLable()
{
    qDebug() << "SheetRenderer::pageHasLable start";
    loadPageLable();

    if (m_lable2Page.count() > 0) {
        qDebug() << "SheetRenderer::pageHasLable end - true";
        return true;
    }

    qDebug() << "SheetRenderer::pageHasLable end - false";
    return false;
}

QString SheetRenderer::pageNum2Lable(const int index)
{
    qDebug() << "SheetRenderer::pageNum2Lable start - index:" << index;
    QMap<QString, int>::const_iterator iter;
    for (iter = m_lable2Page.constBegin(); iter != m_lable2Page.constEnd(); ++iter) {
        if (iter.value() == index)
            return iter.key();
    }

    qDebug() << "SheetRenderer::pageNum2Lable end";
    return  QString::number(index + 1);
}

bool SheetRenderer::saveAs(const QString &filePath)
{
    qDebug() << "Attempting to save document as:" << filePath;
    if (filePath.isEmpty()) {
        qWarning() << "Cannot save - empty file path";
        return false;
    }
    if (m_document == nullptr) {
        qWarning() << "Cannot save - no document loaded";
        return false;
    }

    bool success = m_document->saveAs(filePath);
    if (!success) {
        qWarning() << "Failed to save document as:" << filePath;
    }
    qDebug() << "SheetRenderer::saveAs end, success:" << success;
    return success;
}

void SheetRenderer::handleOpened(deepin_reader::Document::Error error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    qDebug() << "Document opened with error:" << error;
    m_error = error;

    m_document = document;

    m_pages = pages;

    emit sigOpened(error);
    qDebug() << "SheetRenderer::handleOpened end";
}


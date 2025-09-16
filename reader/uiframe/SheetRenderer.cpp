// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SheetRenderer.h"

#include "PageRenderThread.h"
#include "ddlog.h"

#include <QDebug>

SheetRenderer::SheetRenderer(DocSheet *parent) : QObject(parent), m_sheet(parent)
{
    qCDebug(appLog) << "Creating SheetRenderer for sheet:" << (parent ? parent->filePath() : "null");
}

SheetRenderer::~SheetRenderer()
{
    qCDebug(appLog) << "正在添加关闭文档任务...";
    DocCloseTask task;

    task.document = m_document;

    task.pages = m_pages;

    PageRenderThread::appendTask(task);
    qCDebug(appLog) << "关闭文档任务已添加";
}

bool SheetRenderer::openFileExec(const QString &password)
{
    qCDebug(appLog) << "Executing synchronous file open";
    QEventLoop loop;

    connect(this, &SheetRenderer::sigOpened, &loop, &QEventLoop::quit);

    openFileAsync(password);

    loop.exec();

    bool success = deepin_reader::Document::NoError == m_error;
    if (!success) {
        qCWarning(appLog) << "Failed to open file synchronously, error:" << m_error;
    }
    return success;
}

void SheetRenderer::openFileAsync(const QString &password)
{
    qCDebug(appLog) << "Starting asynchronous file open";
    DocOpenTask task;

    task.sheet = m_sheet;

    task.password = password;

    task.renderer = this;

    PageRenderThread::appendTask(task);
    qCDebug(appLog) << "SheetRenderer::openFileAsync end";
}

bool SheetRenderer::opened()
{
    qCDebug(appLog) << "SheetRenderer::opened start";
    return m_document != nullptr;
}

int SheetRenderer::getPageCount()
{
    qCDebug(appLog) << "SheetRenderer::getPageCount start";
    return m_pages.count();
}

QImage SheetRenderer::getImage(int index, int width, int height, const QRect &slice)
{
    qCDebug(appLog) << "SheetRenderer::getImage start - index:" << index;
    if (m_pages.count() <= index) {
        qCWarning(appLog) << "Invalid page index:" << index << "max pages:" << m_pages.count();
        return QImage();
    }

    QImage image = m_pages.value(index)->render(width, height, slice);
    qCDebug(appLog) << "SheetRenderer::getImage end";
    return image;
}

deepin_reader::Link SheetRenderer::getLinkAtPoint(int index, const QPointF &point)
{
    // qCDebug(appLog) << "SheetRenderer::getLinkAtPoint start - index:" << index << "point:" << point;
    if (m_pages.count() <= index)
        return deepin_reader::Link();

    // qCDebug(appLog) << "SheetRenderer::getLinkAtPoint end";
    return  m_pages.value(index)->getLinkAtPoint(point);
}

QList<deepin_reader::Word> SheetRenderer::getWords(int index)
{
    // qCDebug(appLog) << "SheetRenderer::getWords start - index:" << index;
    if (m_pages.count() <= index)
        return QList<deepin_reader::Word>();

    // qCDebug(appLog) << "SheetRenderer::getWords end";
    return m_pages.value(index)->words();
}

QList<deepin_reader::Annotation *> SheetRenderer::getAnnotations(int index)
{
    // qCDebug(appLog) << "SheetRenderer::getAnnotations start - index:" << index;
    if (m_pages.count() <= index)
        return QList<deepin_reader::Annotation *>();

    // qCDebug(appLog) << "SheetRenderer::getAnnotations end";
    return m_pages.value(index)->annotations();
}

QSizeF SheetRenderer::getPageSize(int index) const
{
    // qCDebug(appLog) << "SheetRenderer::getPageSize start - index:" << index;
    if (m_pages.count() <= index)
        return QSizeF();

    // qCDebug(appLog) << "SheetRenderer::getPageSize end";
    return m_pages.value(index)->sizeF();
}

deepin_reader::Annotation *SheetRenderer::addHighlightAnnotation(int index, const QList<QRectF> &boundaries, const QString &text, const QColor &color)
{
    // qCDebug(appLog) << "SheetRenderer::addHighlightAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    // qCDebug(appLog) << "SheetRenderer::addHighlightAnnotation end";
    return m_pages.value(index)->addHighlightAnnotation(boundaries, text, color);
}

bool SheetRenderer::removeAnnotation(int index, deepin_reader::Annotation *annotation)
{
    // qCDebug(appLog) << "SheetRenderer::removeAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    // qCDebug(appLog) << "SheetRenderer::removeAnnotation end";
    return m_pages.value(index)->removeAnnotation(annotation);
}

bool SheetRenderer::updateAnnotation(int index, deepin_reader::Annotation *annotation, const QString &text, const QColor &color)
{
    qCDebug(appLog) << "SheetRenderer::updateAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    qCDebug(appLog) << "SheetRenderer::updateAnnotation end";
    return m_pages.value(index)->updateAnnotation(annotation, text, color);
}

deepin_reader::Annotation *SheetRenderer::addIconAnnotation(int index, const QRectF &rect, const QString &text)
{
    qCDebug(appLog) << "SheetRenderer::addIconAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    qCDebug(appLog) << "SheetRenderer::addIconAnnotation end";
    return m_pages.value(index)->addIconAnnotation(rect, text);
}

deepin_reader::Annotation *SheetRenderer::moveIconAnnotation(int index, deepin_reader::Annotation *annot, const QRectF &rect)
{
    qCDebug(appLog) << "SheetRenderer::moveIconAnnotation start - index:" << index;
    if (m_pages.count() <= index)
        return nullptr;

    qCDebug(appLog) << "SheetRenderer::moveIconAnnotation end";
    return m_pages.value(index)->moveIconAnnotation(annot, rect);
}

QString SheetRenderer::getText(int index, const QRectF &rect)
{
    qCDebug(appLog) << "SheetRenderer::getText start - index:" << index;
    if (m_pages.count() <= index)
        return QString();

    qCDebug(appLog) << "SheetRenderer::getText end";
    return m_pages.value(index)->text(rect);
}

QVector<deepin_reader::PageSection> SheetRenderer::search(int index, const QString &text, bool matchCase, bool wholeWords)
{
    qCDebug(appLog) << "SheetRenderer::search start - index:" << index;
    if (m_pages.count() <= index)
        return QVector<deepin_reader::PageSection>();

    qCDebug(appLog) << "SheetRenderer::search end";
    return m_pages.value(index)->search(text, matchCase, wholeWords);
}

bool SheetRenderer::inLink(int index, const QPointF &pos)
{
    qCDebug(appLog) << "SheetRenderer::inLink start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    deepin_reader::Link link = m_pages.value(index)->getLinkAtPoint(pos);

    qCDebug(appLog) << "SheetRenderer::inLink end";
    return link.isValid();
}

bool SheetRenderer::hasWidgetAnnots(int index)
{
    qCDebug(appLog) << "SheetRenderer::hasWidgetAnnots start - index:" << index;
    if (m_pages.count() <= index)
        return false;

    qCDebug(appLog) << "SheetRenderer::hasWidgetAnnots end";
    return m_pages.value(index)->hasWidgetAnnots();
}

deepin_reader::Outline SheetRenderer::outline()
{
    qCDebug(appLog) << "SheetRenderer::outline start";
    if (m_document == nullptr)
        return deepin_reader::Outline();

    qCDebug(appLog) << "SheetRenderer::outline end";
    return m_document->outline();
}

deepin_reader::Properties SheetRenderer::properties() const
{
    qCDebug(appLog) << "SheetRenderer::properties start";
    if (m_document == nullptr)
        return deepin_reader::Properties();

    qCDebug(appLog) << "SheetRenderer::properties end";
    return m_document->properties();
}

bool SheetRenderer::save()
{
    qCDebug(appLog) << "Attempting to save document";
    if (m_document == nullptr) {
        qCWarning(appLog) << "Cannot save - no document loaded";
        return false;
    }

    bool success = m_document->save();
    if (!success) {
        qCWarning(appLog) << "Failed to save document";
    }
    qCDebug(appLog) << "SheetRenderer::save end, success:" << success;
    return success;
}

void SheetRenderer::loadPageLable()
{
    qCDebug(appLog) << "SheetRenderer::loadPageLable start";
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
    qCDebug(appLog) << "SheetRenderer::loadPageLable end";
}

int SheetRenderer::pageLableIndex(const QString pageLable)
{
    qCDebug(appLog) << "SheetRenderer::pageLableIndex start - pageLable:" << pageLable;
    if (m_lable2Page.count() <= 0 || !m_lable2Page.contains(pageLable))
        return -1;

    qCDebug(appLog) << "SheetRenderer::pageLableIndex end";
    return m_lable2Page.value(pageLable);
}

bool SheetRenderer::pageHasLable()
{
    qCDebug(appLog) << "SheetRenderer::pageHasLable start";
    loadPageLable();

    if (m_lable2Page.count() > 0) {
        qCDebug(appLog) << "SheetRenderer::pageHasLable end - true";
        return true;
    }

    qCDebug(appLog) << "SheetRenderer::pageHasLable end - false";
    return false;
}

QString SheetRenderer::pageNum2Lable(const int index)
{
    qCDebug(appLog) << "SheetRenderer::pageNum2Lable start - index:" << index;
    QMap<QString, int>::const_iterator iter;
    for (iter = m_lable2Page.constBegin(); iter != m_lable2Page.constEnd(); ++iter) {
        if (iter.value() == index)
            return iter.key();
    }

    qCDebug(appLog) << "SheetRenderer::pageNum2Lable end";
    return  QString::number(index + 1);
}

bool SheetRenderer::saveAs(const QString &filePath)
{
    qCDebug(appLog) << "Attempting to save document as:" << filePath;
    if (filePath.isEmpty()) {
        qCWarning(appLog) << "Cannot save - empty file path";
        return false;
    }
    if (m_document == nullptr) {
        qCWarning(appLog) << "Cannot save - no document loaded";
        return false;
    }

    bool success = m_document->saveAs(filePath);
    if (!success) {
        qCWarning(appLog) << "Failed to save document as:" << filePath;
    }
    qCDebug(appLog) << "SheetRenderer::saveAs end, success:" << success;
    return success;
}

void SheetRenderer::handleOpened(deepin_reader::Document::Error error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    qCDebug(appLog) << "Document opened with error:" << error;
    m_error = error;

    m_document = document;

    m_pages = pages;

    emit sigOpened(error);
    qCDebug(appLog) << "SheetRenderer::handleOpened end";
}


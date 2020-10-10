/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar Sheets)
*
* Sheet(SheetSidebar SheetBrowser document)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BrowserPage.h"
#include "document/Model.h"
#include "PageRenderThread.h"
#include "SheetBrowser.h"
#include "BrowserWord.h"
#include "BrowserAnnotation.h"
#include "Application.h"
#include "PageViewportThread.h"
#include "Utils.h"

#include <DApplicationHelper>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>
#include <QMutexLocker>
#include <QTimer>
#include <QUuid>
#include <QPainterPath>
#include <QDesktopServices>
#include <QDebug>

QSet<BrowserPage *> BrowserPage::items;
BrowserPage::BrowserPage(SheetBrowser *parent, deepin_reader::Page *page) : QGraphicsItem(), m_parent(parent), m_page(page)
{
    items.insert(this);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsPanel);
}

BrowserPage::~BrowserPage()
{
    items.remove(this);

    qDeleteAll(m_annotations);

    qDeleteAll(m_annotationItems);

    qDeleteAll(m_words);

    if (nullptr != m_page)
        delete m_page;
}

void BrowserPage::reOpen(Page *page)
{
    Page *tmpPage = m_page;

    m_page = page;

    qDeleteAll(m_annotations);
    m_annotations.clear();

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_hasLoadedAnnotation = false;

    if (nullptr != tmpPage)
        delete tmpPage;
}

QRectF BrowserPage::boundingRect() const
{
    return QRectF(0, 0, static_cast<double>(m_pageSizeF.width() * m_scaleFactor), static_cast<double>(m_pageSizeF.height() * m_scaleFactor));
}

QRectF BrowserPage::rect()
{
    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, static_cast<double>(m_pageSizeF.height() * m_scaleFactor), static_cast<double>(m_pageSizeF.width() * m_scaleFactor));
    default: break;
    }

    return QRectF(0, 0, static_cast<double>(m_pageSizeF.width() * m_scaleFactor), static_cast<double>(m_pageSizeF.height() * m_scaleFactor));
}

QRectF BrowserPage::bookmarkRect()
{
    return QRectF(boundingRect().width() - 40, 1, 39, 39);
}

QRectF BrowserPage::bookmarkMouseRect()
{
    return QRectF(boundingRect().width() - 27, 10, 14, 20);
}

void BrowserPage::setBookmark(const bool &hasBookmark)
{
    m_bookmark = hasBookmark;

    if (hasBookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;

    update();
}

void BrowserPage::updateBookmarkState()
{
    if (m_bookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;

    update();
}

void BrowserPage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (m_page == nullptr)
        m_page = m_parent->page(itemIndex());

    if (!m_viewportTryRender)
        renderViewPort(false);

    if (!m_pixmapHasRendered) {
        render(m_scaleFactor, m_rotation);
    }

    painter->drawPixmap(option->rect, m_pixmap);

    if (m_viewportRenderedRect.isValid() && qFuzzyCompare(m_viewportScaleFactor, m_scaleFactor))
        painter->drawImage(m_viewportRenderedRect, m_viewportImage);

    if (1 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39)));
    if (2 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39)));
    if (3 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39)));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(238, 220, 0, 100));
    int lightsize = m_searchLightrectLst.size();

    for (int i = 0; i < lightsize; i++) {
        painter->drawRect(getNorotateRect(m_searchLightrectLst[i]));
    }

    painter->setBrush(QColor(59, 148, 1, 100));
    if (m_searchSelectLighRectf.width() > 0 || m_searchSelectLighRectf.height() > 0)
        painter->drawRect(getNorotateRect(m_searchSelectLighRectf));

    if (m_drawMoveIconRect) {
        QPen pen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
        painter->setPen(pen);

        qreal iconWidth = (nullptr != m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) ? (m_lastClickIconAnnotationItem->boundingRect().width()) : ICON_SIZE;
        qreal iconHeight = (nullptr != m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) ? (m_lastClickIconAnnotationItem->boundingRect().height()) : ICON_SIZE;

        int x = static_cast<int>(m_drawMoveIconPoint.x() - iconWidth / 2.0);
        int y = static_cast<int>(m_drawMoveIconPoint.y() - iconHeight / 2.0);

        if (x < 0 || y < 0 || x > this->boundingRect().width() || y > this->boundingRect().height())
            return;

        QRect rect = QRect(x, y, static_cast<int>(iconHeight), static_cast<int>(iconHeight));

        painter->drawRect(rect);
    }
}

void BrowserPage::render(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater, const bool &force)
{
    if (!force && renderLater && qFuzzyCompare(scaleFactor, m_scaleFactor) && rotation == m_rotation)
        return;

    m_pixmapHasRendered = false;

    if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem))
        m_lastClickIconAnnotationItem->setScaleFactor(scaleFactor);

    if (m_viewportRenderedRect.isValid()) {
        m_viewportRenderedRect = QRect();
        m_viewportImage = QImage();
    }

    m_scaleFactor = scaleFactor;

    if (m_rotation != rotation) {
        m_rotation = rotation;
        if (Dr::RotateBy0 == m_rotation)
            this->setRotation(0);
        else if (Dr::RotateBy90 == m_rotation)
            this->setRotation(90);
        else if (Dr::RotateBy180 == m_rotation)
            this->setRotation(180);
        else if (Dr::RotateBy270 == m_rotation)
            this->setRotation(270);
    }

//    if (m_wordHasRendered && !m_wordIsHide)
//        loadWords(false);

    if (!renderLater && !qFuzzyCompare(m_pixmapScaleFactor, m_scaleFactor)) {
        m_pixmapScaleFactor = m_scaleFactor;

        m_pixmapRenderedRect = QRect(0, 0, static_cast<int>(boundingRect().width()), 0);

        m_pixmapHasRendered = true;

        if (m_pixmap.isNull()) {
            m_pixmap = QPixmap(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
            m_pixmap.fill(Qt::white);
        } else
            m_pixmap = m_pixmap.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), Qt::IgnoreAspectRatio);

        PageRenderThread::clearTask(this);

        RenderPageTask task;
        task.item = this;
        task.scaleFactor = m_scaleFactor;
        task.rotation = Dr::RotateBy0;
        task.renderRect = boundingRect().toRect();
        PageRenderThread::appendTask(task);

        loadAnnotations();
    }
}

void BrowserPage::handleRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect)
{
    if (!qFuzzyCompare(scaleFactor, m_pixmapScaleFactor))
        return;

    if (rect.height() == static_cast<int>(boundingRect().height())) {
        m_pixmap = QPixmap::fromImage(image);
    } else {
        QPainter painter(&m_pixmap);
        painter.drawImage(rect, image);
    }

    m_pixmapRenderedRect.setHeight(rect.y() + rect.height());

    emit m_parent->sigPartThumbnailUpdated(m_index);

    update();
}

void BrowserPage::renderViewPort(bool force)
{
    m_viewportTryRender = true;

    if (nullptr == m_parent)
        return;

    if (!force && boundingRect().width() < 2000 && boundingRect().height() < 2000)
        return;

    QRect viewPortRect = QRect(0, 0, m_parent->size().width(), m_parent->size().height());

    QRectF visibleSceneRectF = m_parent->mapToScene(viewPortRect).boundingRect();

    QRectF intersectedRectF = this->mapToScene(this->boundingRect()).boundingRect().intersected(visibleSceneRectF);

    //如果不在当前可视范围则不加载 强制也没用
    if (intersectedRectF.height() <= 0 && intersectedRectF.width() <= 0)
        return;

    QRectF viewRenderRectF = mapFromScene(intersectedRectF).boundingRect();

    QRect viewRenderRect = QRect(static_cast<int>(viewRenderRectF.x()), static_cast<int>(viewRenderRectF.y()),
                                 static_cast<int>(viewRenderRectF.width()), static_cast<int>(viewRenderRectF.height()));

    //如果现在已经加载的rect包含viewRender 就不加入任务
    if (!force && m_pixmapRenderedRect.contains(viewRenderRect))
        return;

    //如果只是当前视图区域变小则不加载
//    if (!force && m_viewportRenderedRect.contains(viewRenderRect))
//        return;

    RenderViewportTask task;

    task.page = this;

    task.scaleFactor = m_scaleFactor;

    task.rotation = Dr::RotateBy0;

    task.renderRect = viewRenderRect;

    PageViewportThread::appendTask(task);
}

void BrowserPage::handleViewportRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect)
{
    if (!qFuzzyCompare(scaleFactor, m_pixmapScaleFactor))
        return;

    m_viewportScaleFactor = scaleFactor;

    m_viewportImage = image;

    m_viewportRenderedRect = rect;

    QPainter painter(&m_pixmap);

    painter.drawImage(rect, image);

    update();
}

void BrowserPage::handleWordLoaded(const QList<Word> &words)
{
    m_wordIsRendering = false;

    if (m_wordHasRendered)
        return;

    for (int i = 0; i < words.count(); ++i) {
        BrowserWord *word = new BrowserWord(this, words[i]);
        word->setSelectable(m_wordSelectable);
        m_words.append(word);
    }

    m_wordHasRendered = true;

    scaleWords(true);

}

QImage BrowserPage::getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect)
{
    if (nullptr == m_page)
        return QImage();

    return m_page->render(rotation, scaleFactor, boundingRect);
}

QImage BrowserPage::getImage(int width, int height, Qt::AspectRatioMode mode, bool bSrc)
{
    if (nullptr == m_page)
        return QImage();

    if (bSrc) {
        if (m_pixmap.isNull())
            return QImage();

        QImage image = m_pixmap.toImage().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode, Qt::SmoothTransformation);
        image.setDevicePixelRatio(dApp->devicePixelRatio());
        return image;
    }

    QSizeF size = m_pageSizeF.scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode);

    QImage image = m_page->render(static_cast<int>(size.width()), static_cast<int>(size.height()), mode);

    image.setDevicePixelRatio(dApp->devicePixelRatio());

    return image;
}

QImage BrowserPage::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage BrowserPage::getImagePoint(double scaleFactor, QPoint point)
{
    const QPoint &transformPoint = translatePoint(point);
    int ss = static_cast<int>(122 * scaleFactor / m_scaleFactor);
    QRect rect = QRect(qRound(transformPoint.x() * scaleFactor / m_scaleFactor - ss / 2.0), qRound(transformPoint.y() * scaleFactor / m_scaleFactor - ss / 2.0), ss, ss);
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage BrowserPage::getCurImagePoint(QPoint point)
{
    int ds = 122;
    QTransform transform;
    transform.rotate(m_rotation * 90);
    QImage image = Utils::copyImage(m_pixmap.toImage(), qRound(point.x() - ds / 2.0), qRound(point.y() - ds / 2.0), ds, ds).transformed(transform, Qt::SmoothTransformation);
    image.setDevicePixelRatio(dApp->devicePixelRatio());
    return image;
}

QList<Word> BrowserPage::getWords()
{
    if (nullptr == m_page)
        return QList<Word>();

    return m_page->words(Dr::RotateBy0);
}

bool BrowserPage::existInstance(BrowserPage *item)
{
    return items.contains(item);
}

void BrowserPage::setItemIndex(int itemIndex)
{
    m_index = itemIndex;
}

void BrowserPage::setPageSize(const QSizeF &pagesize)
{
    m_pageSizeF = pagesize;
}

int BrowserPage::itemIndex()
{
    return m_index;
}

QString BrowserPage::selectedWords()
{
    QString text;
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected()) {
            text += word->text();
        }
    }

    return text;
}

void BrowserPage::setWordSelectable(bool selectable)
{
    m_wordSelectable = selectable;
    foreach (BrowserWord *word, m_words) {
        word->setSelectable(selectable);
    }
}

void BrowserPage::loadAnnotations()
{
    if (!m_hasLoadedAnnotation)
        reloadAnnotations();
}

void BrowserPage::loadWords()
{
    m_wordNeeded = true;

    if (m_wordIsRendering)
        return;

    if (m_wordHasRendered) {
        //如果已经加载则取消隐藏和改变大小
        if (m_words.count() <= 0)
            return;

        prepareGeometryChange();

        if (!qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
            m_wordScaleFactor = m_scaleFactor;
            foreach (BrowserWord *word, m_words) {
                word->setScaleFactor(m_scaleFactor);
            }
        }
        return;
    }

    RenderPageTask task;
    task.type = RenderPageTask::word;
    task.item = this;
    PageRenderThread::appendTask(task);

    m_wordIsRendering = true;
}

void BrowserPage::clearPixmap()
{
    m_pixmap = QPixmap();
    m_pixmapHasRendered = false;
    m_pixmapScaleFactor = -1;
    m_pixmapRenderedRect = QRect();

    m_viewportImage = QImage();
    m_viewportRenderedRect = QRect();
    m_viewportScaleFactor = -1;

    PageRenderThread::clearTask(this);
}

void BrowserPage::clearWords()
{
    if (!m_wordHasRendered)
        return;

    foreach (BrowserWord *word, m_words) {
        if (word->isSelected())
            return;
    }

    prepareGeometryChange();

    m_wordHasRendered = false;
    m_wordNeeded = false;

    QList<BrowserWord *> t_word = m_words;
    m_words.clear();

    foreach (BrowserWord *word, t_word) {
        word->setParentItem(nullptr);
        scene()->removeItem(word);
        delete word;
    }
}

void BrowserPage::scaleWords(bool force)
{
    if (!m_wordHasRendered || m_words.count() <= 0)
        return;

    prepareGeometryChange();

    if (force || !qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
        m_wordScaleFactor = m_scaleFactor;
        foreach (BrowserWord *word, m_words) {
            word->setScaleFactor(m_scaleFactor);
        }
    }
}

void BrowserPage::reloadAnnotations()
{
    if (m_page == nullptr)
        return;

    //在reload之前将上一次选中去掉,避免操作野指针
    if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) {
        m_lastClickIconAnnotationItem->setDrawSelectRect(false);
        m_lastClickIconAnnotationItem = nullptr;
    }

    qDeleteAll(m_annotations);

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_annotations = m_page->annotations();

    for (int i = 0; i < m_annotations.count(); ++i) {
        m_annotations[i]->page = m_index + 1;

        //图标注释,可能是其它系统上不一样的样式,需刷新下位置达到重新刷新成自己样式的图标的目的
        if (m_annotations[i]->type() == 1 /*Text*/) {
            const QList<QRectF> &annoBoundary = m_annotations[i]->boundary();
            if (annoBoundary.size() > 0) {
                m_page->moveIconAnnotation(m_annotations[i], annoBoundary.at(0));
            }
        }

        foreach (QRectF rect, m_annotations[i]->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, m_annotations[i]);
            m_annotationItems.append(annotationItem);
        }
    }

    m_hasLoadedAnnotation = true;
}

QList<deepin_reader::Annotation *> BrowserPage::annotations()
{
    return m_annotations;
}

bool BrowserPage::updateAnnotation(deepin_reader::Annotation *annotation, const QString text, const QColor color)
{
    if (nullptr == annotation)
        return false;

    if (!m_annotations.contains(annotation))
        return false;

    if (!m_page->updateAnnotation(annotation, text, color))
        return false;

    updatePageFull();

    return true;
}

Annotation *BrowserPage::addHighlightAnnotation(QString text, QColor color)
{
    if (nullptr == m_page)
        return nullptr;

    Annotation *highLightAnnot{nullptr};
    QList<QRectF> boundarys;
    QRectF rect;
    QRectF recboundary;
    int index{0};
    qreal curwidth = m_pageSizeF.width();
    qreal curheight = m_pageSizeF.height();

    //加载文档文字无旋转情况下的文字(即旋转0度时的所有文字)
    QList<deepin_reader::Word> twords = m_page->words(Dr::RotateBy0);

    for (index = 0; index < m_words.count(); index++) {
        if (m_words.at(index) && m_words.at(index)->isSelected()) {
            m_words.at(index)->setSelected(false);
            if (index >= 0 && index < twords.count()) {
                deepin_reader::Word tword = twords.at(index);

                rect = tword.wordBoundingRect();

                recboundary.setTopLeft(QPointF(rect.left() / curwidth,
                                               rect.top() / curheight));
                recboundary.setTopRight(QPointF(rect.right() / curwidth,
                                                rect.top() / curheight));
                recboundary.setBottomLeft(QPointF(rect.left() / curwidth,
                                                  rect.bottom() / curheight));
                recboundary.setBottomRight(QPointF(rect.right() / curwidth,
                                                   rect.bottom() / curheight));
                boundarys << recboundary;
            }
        }
    }

    if (boundarys.count()) {
        loadAnnotations();

        highLightAnnot = m_page->addHighlightAnnotation(boundarys, text, color);
        if (highLightAnnot == nullptr)
            return nullptr;
        highLightAnnot->page = m_index + 1;
        m_annotations.append(highLightAnnot);

        foreach (QRectF rect, highLightAnnot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, highLightAnnot);
            m_annotationItems.append(annotationItem);
        }
    }

    updatePageFull();

    return highLightAnnot;
}

bool BrowserPage::hasAnnotation(deepin_reader::Annotation *annotation)
{
    return m_annotations.contains(annotation);
}

void BrowserPage::setSelectIconRect(const bool draw, Annotation *iconAnnot)
{
    QList<QRectF> rectList;

    if (iconAnnot) {
        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(iconAnnot)) {
                m_lastClickIconAnnotationItem = annotation;
                if (iconAnnot->type() == 1)
                    m_lastClickIconAnnotationItem->setDrawSelectRect(draw);
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
            }
        }
    } else {
        if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem))
            m_lastClickIconAnnotationItem->setDrawSelectRect(draw);
    }
}

void BrowserPage::setDrawMoveIconRect(const bool draw)
{
    m_drawMoveIconRect = draw;

    update();
}

void BrowserPage::setIconMovePos(const QPointF movePoint)
{
    m_drawMoveIconPoint = movePoint;

    update();
}

QString BrowserPage::deleteNowSelectIconAnnotation()
{
    if (nullptr == m_lastClickIconAnnotationItem)
        return "";

    QString iconAnnotationContains{""};

    m_lastClickIconAnnotationItem->setDrawSelectRect(false);

    iconAnnotationContains = m_lastClickIconAnnotationItem->annotationText();

    removeAnnotation(m_lastClickIconAnnotationItem->annotation());

    m_lastClickIconAnnotationItem = nullptr;

    return iconAnnotationContains;
}

bool BrowserPage::moveIconAnnotation(const QRectF moveRect)
{
    if (nullptr == m_page || nullptr == m_lastClickIconAnnotationItem)
        return false;


    Annotation *annot{nullptr};
    QString containtStr = m_lastClickIconAnnotationItem->annotationText();

    m_annotationItems.removeAll(m_lastClickIconAnnotationItem);
    annot = m_page->moveIconAnnotation(m_lastClickIconAnnotationItem->annotation(), moveRect);

    if (annot && m_annotations.contains(annot)) {
        delete m_lastClickIconAnnotationItem;
        m_lastClickIconAnnotationItem = nullptr;
        annot->page = m_index + 1;
        foreach (QRectF rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotationItem = annotationItem;
        }

        if (annot->type() == 1) {
            if (m_lastClickIconAnnotationItem) {
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotationItem->setDrawSelectRect(true);
            }
        }
    }

    updatePageFull();

    return true;
}

bool BrowserPage::removeAllAnnotation()
{
    m_lastClickIconAnnotationItem = nullptr;

    if (m_annotations.isEmpty())
        return false;

    for (int index = 0; index < m_annotations.size(); index++) {
        deepin_reader::Annotation *annota = m_annotations.at(index);
        if (!m_annotations.contains(annota) || (annota && annota->contents().isEmpty()))
            continue;

        if (!m_page->removeAnnotation(annota))
            continue;

        m_annotations.removeAt(index);
        index--;

        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(annota)) {
                if (m_lastClickIconAnnotationItem == annotation)
                    m_lastClickIconAnnotationItem = nullptr;
                m_annotationItems.removeAll(annotation);
                delete annotation;
                annotation = nullptr;
            }
        }
    }

    m_hasLoadedAnnotation = false;

    updatePageFull();

    return true;
}

bool BrowserPage::jump2Link(const QPointF point)
{
    if ((m_rotation != Dr::RotateBy0) && (m_rotation < Dr::NumberOfRotations)) {
        Q_UNUSED(point)
        return false;
    }

    if (nullptr == m_page)
        return false;

    return false;
}

bool BrowserPage::inLink(const QPointF pos)
{
    if ((m_rotation != Dr::RotateBy0) && (m_rotation < Dr::NumberOfRotations)) {
        Q_UNUSED(pos)
        return false;
    }

    return false;
}

void BrowserPage::setPageBookMark(const QPointF clickPoint)
{
    if (bookmarkMouseRect().contains(clickPoint)) {
        m_bookmarkState = 2;
        if (nullptr != m_parent) {
            m_parent->needBookmark(m_index, !m_bookmark);
            if (!m_bookmark && bookmarkMouseRect().contains(clickPoint))
                m_bookmarkState = 1;
            else if (m_bookmark)
                m_bookmarkState = 3;
            else
                m_bookmarkState = 0;
        }

        update();
    }
}

bool BrowserPage::removeAnnotation(deepin_reader::Annotation *annota)
{
    if (nullptr == annota)
        return false;

    if (!m_annotations.contains(annota))
        return false;

    m_annotations.removeAll(annota);

    if (!m_page->removeAnnotation(annota))
        return false;

    foreach (BrowserAnnotation *annotation, m_annotationItems) {
        if (annotation && annotation->isSame(annota)) {
            if (m_lastClickIconAnnotationItem == annotation)
                m_lastClickIconAnnotationItem = nullptr;
            m_annotationItems.removeAll(annotation);
            delete annotation;
            annotation = nullptr;
        }
    }

    updatePageFull();

    return true;
}

Annotation *BrowserPage::addIconAnnotation(const QRectF rect, const QString text)
{
    if (nullptr == m_page)
        return nullptr;

    Annotation *annot{nullptr};

    annot = m_page->addIconAnnotation(rect, text);

    if (annot) {
        annot->page = m_index + 1;

        m_annotations.append(annot);

        foreach (QRectF rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotationItem = annotationItem;
        }

        if (annot->type() == 1) {
            if (m_lastClickIconAnnotationItem) {
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotationItem->setDrawSelectRect(true);
            }
        }
    }

    updatePageFull();

    return annot;
}

bool BrowserPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    if (nullptr == m_page)
        return false;

    return m_page->mouseClickIconAnnot(clickPoint);
}

/**
 * @brief BrowserPage::sceneEvent
 *  画布事件,目前只处理HoverMove事件
 * @param event
 * @return
 */
bool BrowserPage::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneHoverMove) {
        QGraphicsSceneHoverEvent *moveevent = dynamic_cast<QGraphicsSceneHoverEvent *>(event);
        if (!m_bookmark && bookmarkMouseRect().contains(moveevent->pos()))
            m_bookmarkState = 1;
        else if (m_bookmark)
            m_bookmarkState = 3;
        else
            m_bookmarkState = 0;
        update();
    }
    return QGraphicsItem::sceneEvent(event);
}

void BrowserPage::setSearchHighlightRectf(const QVector< QRectF > &rectflst)
{
    if (rectflst.size() > 0) {
        if (m_parent->currentPage() == this->itemIndex() + 1)
            m_searchSelectLighRectf = rectflst.first();
        m_searchLightrectLst = rectflst;
        update();
    }
}

void BrowserPage::clearSearchHighlightRects()
{
    m_searchSelectLighRectf = QRectF(0, 0, 0, 0);
    m_searchLightrectLst.clear();
    update();
}

void BrowserPage::clearSelectSearchHighlightRects()
{
    m_searchSelectLighRectf = QRectF(0, 0, 0, 0);
    update();
}

int BrowserPage::searchHighlightRectSize()
{
    return m_searchLightrectLst.size();
}

QRectF BrowserPage::findSearchforIndex(int index)
{
    if (index >= 0 && index < m_searchLightrectLst.size()) {
        m_searchSelectLighRectf = m_searchLightrectLst[index];
        update();
        return m_searchSelectLighRectf;
    }
    return QRectF(-1, -1, -1, -1);
}

QRectF BrowserPage::getNorotateRect(const QRectF &rect)
{
    QRectF newrect;
    newrect.setX(rect.x()*m_scaleFactor);
    newrect.setY(rect.y()*m_scaleFactor);
    newrect.setWidth(rect.width()*m_scaleFactor);
    newrect.setHeight(rect.height()*m_scaleFactor);
    return newrect;
}

QPoint BrowserPage::translatePoint(const QPoint &point)
{
    QPoint newpoint = point;
    switch (m_rotation) {
    case Dr::RotateBy90: {
        newpoint.setX(static_cast<int>(boundingRect().height() - point.y()));
        newpoint.setY(point.x());
        break;
    }
    case Dr::RotateBy180: {
        newpoint.setX(static_cast<int>(boundingRect().width() - point.x()));
        newpoint.setY(static_cast<int>(boundingRect().height() - point.y()));
        break;
    }
    case Dr::RotateBy270: {
        newpoint.setX(point.y());
        newpoint.setY(static_cast<int>(boundingRect().width() - point.x()));
        break;
    }
    default:
        break;
    }
    return  newpoint;
}

QRectF BrowserPage::translateRect(const QRectF &rect)
{
    //旋转角度逆时针增加
    QRectF newrect;
    switch (m_rotation) {
    case Dr::RotateBy0: {
        newrect.setX(rect.x()*m_scaleFactor);
        newrect.setY(rect.y()*m_scaleFactor);
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy90: {
        newrect.setX((m_pageSizeF.height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setY(rect.x()*m_scaleFactor);
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy180: {
        newrect.setX((m_pageSizeF.width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setY((m_pageSizeF.height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy270: {
        newrect.setX(rect.y()*m_scaleFactor);
        newrect.setY((m_pageSizeF.width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    default:
        break;
    }
    return  newrect;
}

BrowserAnnotation *BrowserPage::getBrowserAnnotation(const QPoint &point)
{
    BrowserAnnotation *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserAnnotation *>(itemIter);
        if (item != nullptr) {
            return item;
        }
    }

    return nullptr;
}

BrowserWord *BrowserPage::getBrowserWord(const QPoint &point)
{
    BrowserWord *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserWord *>(itemIter);
        if (item != nullptr) {
            return item;
        }
    }

    return nullptr;
}

void BrowserPage::updatePageFull()
{
    m_pixmapScaleFactor = -1;

    render(m_scaleFactor, m_rotation, true, true);

    renderViewPort(false);
}

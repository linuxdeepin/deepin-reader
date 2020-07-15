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
#include "RenderPageThread.h"
#include "SheetBrowser.h"
#include "BrowserWord.h"
#include "BrowserAnnotation.h"
#include "Application.h"
#include "RenderViewportThread.h"
#include "Utils.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>
#include <QMutexLocker>

QSet<BrowserPage *> BrowserPage::items;
BrowserPage::BrowserPage(SheetBrowser *parent, deepin_reader::Page *page) : QGraphicsItem(), m_page(page), m_parent(parent)
{
    items.insert(this);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsPanel);
    //setCacheMode(CacheMode::ItemCoordinateCache);
}

BrowserPage::~BrowserPage()
{
    items.remove(this);
    if (nullptr != m_page)
        delete m_page;
}

QRectF BrowserPage::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, static_cast<double>(m_page->sizeF().height() * m_scaleFactor), static_cast<double>(m_page->sizeF().width() * m_scaleFactor));
    default: break;
    }

    return QRectF(0, 0, static_cast<double>(m_page->sizeF().width() * m_scaleFactor), static_cast<double>(m_page->sizeF().height() * m_scaleFactor));
}

QRectF BrowserPage::bookmarkRect()
{
    return QRectF(boundingRect().width() - 40, 1, 39, 39);
}

QRectF BrowserPage::bookmarkMouseRect()
{
    return QRectF(boundingRect().width() - 40, 10, 29, 29);
}

void BrowserPage::setBookmark(bool hasBookmark)
{
    m_bookmark = hasBookmark;
    if (hasBookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;
    update();
}

void BrowserPage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(option->rect);

    if (m_renderedRect.height() == 0) {
        render(m_scaleFactor, m_rotation);
    }

    if (m_hasRendered)
        painter->drawPixmap(option->rect, m_pixmap);

    if (m_viewportRenderedRect.height() > 0)
        painter->drawPixmap(m_viewportRenderedRect, m_viewportPixmap);

    if (1 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39)));
    if (2 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39)));
    if (3 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39)));
}

void BrowserPage::renderViewPort()
{
    if (nullptr == m_parent)
        return;

    QRect viewPortRect = QRect(0, 0, m_parent->size().width(), m_parent->size().height());

    QRectF visibleSceneRectF = m_parent->mapToScene(viewPortRect).boundingRect();

    QRectF intersectedRectF = this->mapToScene(this->boundingRect()).boundingRect().intersected(visibleSceneRectF);

    if (intersectedRectF.height() <= 0 && intersectedRectF.width() <= 0)
        return;

    QRectF viewRenderRectF = mapFromScene(intersectedRectF).boundingRect();

    QRect viewRenderRect = QRect(static_cast<int>(viewRenderRectF.x()), static_cast<int>(viewRenderRectF.y()),
                                 static_cast<int>(viewRenderRectF.width()), static_cast<int>(viewRenderRectF.height()));

    //如果现在已经加载的rect包含viewRender 就不加入任务
    if (m_renderedRect.contains(viewRenderRect))
        return;

    if (m_viewportRenderedRect.contains(viewRenderRect))
        return;

    RenderViewportTask task;

    task.page = this;

    task.scaleFactor = m_scaleFactor;

    task.rotation = m_rotation;

    task.renderRect = viewRenderRect;

    RenderViewportThread::appendTask(task);
}

void BrowserPage::render(double scaleFactor, Dr::Rotation rotation, bool renderLater)
{
    if (nullptr == m_page)
        return;

    if (qFuzzyCompare(scaleFactor, m_imageScaleFactor) && rotation == m_rotation)
        return;

    prepareGeometryChange();

    m_scaleFactor = scaleFactor;

    m_viewportRenderedRect = QRect();
    m_viewportPixmap = QPixmap();

    if (m_rotation != rotation) {//旋转变化则强制移除
        m_rotation  = rotation;
        m_pixmap = QPixmap();
    }

    m_renderedRect = QRect(0, 0, static_cast<int>(boundingRect().width()), 0);

    if (!renderLater) {
        if (m_pixmap.isNull()) {
            m_pixmap = QPixmap(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
            m_pixmap.fill(Qt::white);
        } else
            m_pixmap = m_pixmap.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), Qt::IgnoreAspectRatio);

        m_renderedRect = QRect(0, 0, static_cast<int>(boundingRect().width()), 1);
        //之后替换为线程reader
        RenderPageThread::clearTask(this);

        QList<RenderPageTask> tasks;

        QRectF rect = boundingRect();

        //加载模糊的图
//        RenderPageTask task;
//        task.item = this;
//        task.scaleFactor = 0.05;
//        task.rotation = m_rotation;
//        task.renderRect = QRect();
//        task.preRender = true;
//        tasks.append(task);

        int pieceWidth = 1000;
        int pieceHeight = 1000;

        for (int i = 0 ; i * pieceHeight < rect.height(); ++i) {
            int height = pieceHeight;

            if (rect.height() < (i + 1) * pieceHeight)
                height = static_cast<int>(rect.height() - pieceHeight * i);

            QRect renderRect = QRect(0, pieceHeight * i, static_cast<int>(boundingRect().width()), height);

            for (int j = 0; j * pieceWidth < renderRect.width(); ++j) {
                int width = pieceWidth;

                if (renderRect.width() < (j + 1) * pieceWidth)
                    width = static_cast<int>(renderRect.width() - pieceWidth * j);

                QRect finalRenderRect = QRect(pieceWidth * j, pieceHeight * i, width, height);

                RenderPageTask task;
                task.item = this;
                task.scaleFactor = m_scaleFactor;
                task.rotation = m_rotation;
                task.renderRect = finalRenderRect;
                task.preRender = false;
                tasks.append(task);
            }

        }

        RenderPageThread::appendTasks(tasks);
        m_imageScaleFactor = m_scaleFactor;

        loadAnnotations();

        foreach (BrowserAnnotation *annotationItem, m_annotationItems)
            annotationItem->setScaleFactorAndRotation(m_rotation);
    }
}

QImage BrowserPage::getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect)
{
    return m_page->render(rotation, scaleFactor, boundingRect);
}

QImage BrowserPage::getImage(int width, int height, Qt::AspectRatioMode mode)
{
    if (!m_pixmap.isNull()) {
        QImage image = m_pixmap.toImage().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode, Qt::SmoothTransformation);
        image.setDevicePixelRatio(dApp->devicePixelRatio());
        return image;
    }

    QSizeF size = m_page->sizeF().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode);
    QImage image = m_page->render(static_cast<int>(size.width()), static_cast<int>(size.height()), mode);
    image.setDevicePixelRatio(dApp->devicePixelRatio());
    return image;
}

QImage BrowserPage::thumbnail()
{
    return m_page->thumbnail();
}

QImage BrowserPage::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage BrowserPage::getImagePoint(double scaleFactor, QPoint point)
{
    QMutexLocker locker(&m_imageMutex);
    int ss = static_cast<int>(122 * scaleFactor / m_scaleFactor);

    QRect rect = QRect(qRound(point.x() * scaleFactor / m_scaleFactor - ss / 2.0), qRound(point.y() * scaleFactor / m_scaleFactor - ss / 2.0), ss, ss);
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage BrowserPage::getCurImagePoint(QPoint point)
{
    int ds = 122;
    return Utils::copyImage(m_pixmap.toImage(), qRound(point.x() - ds / 2.0), qRound(point.y() - ds / 2.0), ds, ds);
}

void BrowserPage::handlePreRenderFinished(Dr::Rotation rotation, QImage image)
{
    if (rotation != m_rotation)
        return;

    if (!m_pixmap.isNull())
        return;

    m_hasRendered = true;

    image = image.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), Qt::KeepAspectRatio);

    m_pixmap = QPixmap::fromImage(image.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), Qt::KeepAspectRatio));

    m_renderedRect = QRect(0, 0, static_cast<int>(boundingRect().width()), 2);

    update();
}

void BrowserPage::handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect)
{
    if (!qFuzzyCompare(scaleFactor, m_imageScaleFactor) || rotation != m_rotation)
        return;

    m_hasRendered = true;

    if (rect.height() == static_cast<int>(boundingRect().height())) {
        m_pixmap = QPixmap::fromImage(image);
    } else {
        QPainter painter(&m_pixmap);
        painter.drawImage(rect, image);
    }

    m_renderedRect.setHeight(rect.y() + rect.height());

    emit m_parent->sigThumbnailUpdated(m_index);

    update();
}

void BrowserPage::handleViewportRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect)
{
    if (!qFuzzyCompare(scaleFactor, m_imageScaleFactor) || rotation != m_rotation)
        return;

    m_viewportPixmap = QPixmap::fromImage(image);
    m_viewportRenderedRect = rect;
    update();
}

bool BrowserPage::existInstance(BrowserPage *item)
{
    return items.contains(item);
}

void BrowserPage::setItemIndex(int itemIndex)
{
    m_index = itemIndex;
}

int BrowserPage::itemIndex()
{
    return m_index;
}

QString BrowserPage::selectedWords()
{
    QString text;
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected())
            text += word->text();
    }

    return text;
}

void BrowserPage::setWordSelectable(bool selectable)
{
    m_wordSelectable = selectable;
    foreach (BrowserWord *word, m_words) {
        word->setFlag(QGraphicsItem::ItemIsSelectable, selectable);
    }
}

void BrowserPage::clearWords()
{
    if (m_words.isEmpty())
        return;

    foreach (BrowserWord *word, m_words) {
        scene()->removeItem(word);
        delete word;
    }

    m_words.clear();
    m_wordRotation = Dr::NumberOfRotations;
}

void BrowserPage::loadWords()
{
    if (m_wordRotation != m_rotation) {
        clearWords();

        m_wordRotation = m_rotation;
        QList<deepin_reader::Word> words = m_page->words(m_rotation);
        for (int i = 0; i < words.count(); ++i) {
            BrowserWord *word = new BrowserWord(this, words[i]);
            word->setFlag(QGraphicsItem::ItemIsSelectable, m_wordSelectable);
            m_words.append(word);
        }
    }

    if (!qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
        m_wordScaleFactor = m_scaleFactor;
        foreach (BrowserWord *word, m_words)
            word->setScaleFactor(m_scaleFactor);
    }
}

void BrowserPage::loadAnnotations()
{
    if (!m_hasLoadedAnnotation)
        reloadAnnotations();
}

void BrowserPage::reloadAnnotations()
{
    qDeleteAll(m_annotations);
    m_annotations.clear();

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_annotations = m_page->annotations();
    for (int i = 0; i < m_annotations.count(); ++i) {
        m_annotations[i]->page = m_index + 1;
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

    return  annotation->updateAnnotation(text, color);
}

Annotation *BrowserPage::addHighlightAnnotation(QString text, QColor color)
{
    QList<QRectF> boundarys;
    QRectF rect, recboundary;
    qreal curwidth = this->boundingRect().width();
    qreal curheight = this->boundingRect().height();
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected()) {
            rect = word->boundingBox();
            recboundary.setTopLeft(QPointF(rect.left() / curwidth,
                                           rect.top() / curheight));
            recboundary.setTopRight(QPointF(rect.right() / curwidth,
                                            rect.top() / curheight));
            recboundary.setBottomLeft(QPointF(rect.left() / curwidth,
                                              rect.bottom() / curheight));
            recboundary.setBottomRight(QPointF(rect.right() / curwidth,
                                               rect.bottom() / curheight));
            boundarys.append(word->boundingBox());
        }
    }

    if (boundarys.isEmpty())
        return nullptr;

    Annotation *anno = m_page->addHighlightAnnotation(boundarys, text, color);

    reloadAnnotations();

    renderViewPort();

    return anno;
}

/**
 * @brief BrowserPage::addHighlightAnnotation
 * 根据起始点添加高亮(目前测试用)
 * @param start
 * @param end
 * @param text
 * @param color
 * @return
 */
Annotation *BrowserPage::addHighlightAnnotation(const QPointF start, const QPointF end, QString text, QColor color)
{
    Annotation *highLightAnnot{nullptr};
    QList<QRectF> boundarys;
    QRectF rect, recboundary;
    qreal curwidth = this->boundingRect().width();
    qreal curheight = this->boundingRect().height();

    //calc highlit annot rect list
    if (!start.isNull() && end.isNull()) {
        bool startAddRect{false};
        foreach (BrowserWord *word, m_words) {
            if (word) {
                if (word->boundingRect().contains(start)) {
                    startAddRect = true;
                }
                if (startAddRect) {
                    rect = word->boundingRect();
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
    } else if (start.isNull() && !end.isNull()) {
        bool endAddRect{true};
        foreach (BrowserWord *word, m_words) {
            if (word) {
                if (endAddRect) {
                    rect = word->boundingRect();
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
                if (word->boundingRect().contains(end)) {
                    endAddRect = false;
                    boundarys << word->boundingRect();
                }
            }
        }
    } else {
        bool startAddRect{false};
        foreach (BrowserWord *word, m_words) {
            if (word) {
                if (word->boundingRect().contains(start)) {
                    startAddRect = true;
                }
                if (startAddRect) {
                    rect = word->boundingRect();
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
                if (word->boundingRect().contains(end)) {
                    startAddRect = false;
                    rect = word->boundingRect();
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
    }

    if (boundarys.count()) {
        //add highlight annot
        highLightAnnot = m_page->addHighlightAnnotation(boundarys, text, color);
    }

    reloadAnnotations();

    renderViewPort();

    return highLightAnnot;
}

bool BrowserPage::hasAnnotation(deepin_reader::Annotation *annotation)
{
    return m_annotations.contains(annotation);
}

bool BrowserPage::removeAnnotation(deepin_reader::Annotation *annotation)
{
    if (!m_annotations.contains(annotation))
        return false;

    if (!m_page->removeAnnotation(annotation))
        return false;

    reloadAnnotations();        // 必须要reload 因为可能多个item共享同一个annotation

    renderViewPort();

    return true;
}

Annotation *BrowserPage::addIconAnnotation(const QRectF rect, const QString text)
{
    if (nullptr == m_page)
        return nullptr;

    Annotation *annot{nullptr};

    annot = m_page->addIconAnnotation(rect, text);

    if (annot) {
        m_annotations.append(annot);
        annot->page = m_index + 1;
        foreach (QRectF rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot);
            m_annotationItems.append(annotationItem);
        }
    }

    renderViewPort();

    return annot;
}

bool BrowserPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    if (nullptr == m_page)
        return false;

    return m_page->mouseClickIconAnnot(clickPoint);
}

bool BrowserPage::sceneEvent(QEvent *event)
{
    return QGraphicsItem::sceneEvent(event);

    if (event->type() == QEvent::GraphicsSceneHoverMove) {
        QGraphicsSceneHoverEvent *moveevent = dynamic_cast<QGraphicsSceneHoverEvent *>(event);
        if (!m_bookmark && bookmarkMouseRect().contains(moveevent->pos()))
            m_bookmarkState = 1;
        else if (m_bookmark)
            m_bookmarkState = 3;
        else
            m_bookmarkState = 0;
        update();
    } else if (event->type() == QEvent::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent *t_event = dynamic_cast<QGraphicsSceneMouseEvent *>(event);
        m_posPressed = QPoint();
        if (bookmarkMouseRect().contains(t_event->pos())) {
            m_bookmarkState = 2;
            if (nullptr != m_parent) {
                m_parent->needBookmark(m_index, !m_bookmark);
                if (!m_bookmark && bookmarkMouseRect().contains(t_event->pos()))
                    m_bookmarkState = 1;
                else if (m_bookmark)
                    m_bookmarkState = 3;
                else
                    m_bookmarkState = 0;
            }
            update();
        } else {
            m_posPressed = t_event->pos();
        }
    } else if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        m_posPressed = QPoint();
    }

    return QGraphicsItem::sceneEvent(event);
}

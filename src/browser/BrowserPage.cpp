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
#include "BrowserRenderThread.h"
#include "SheetBrowser.h"
#include "BrowserWord.h"
#include "BrowserAnnotation.h"
#include "Application.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>

QSet<BrowserPage *> BrowserPage::items;
BrowserPage::BrowserPage(SheetBrowser *parent, deepin_reader::Page *page) : QGraphicsItem(), m_page(page), m_parent(parent)
{
    items.insert(this);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsPanel);
    setCacheMode(CacheMode::ItemCoordinateCache);
    setData(0, "BrowserPage");
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
    if (m_image.isNull() || !qFuzzyCompare(m_imageScaleFactor, m_scaleFactor)) {
        render(m_scaleFactor, m_rotation, false);
        renderViewPort();
    }

    if (m_image.isNull())
        painter->drawImage(option->rect, m_leftImage);
    else
        painter->drawImage(option->rect.x(), option->rect.y(), m_image);

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

    QRectF visibleSceneRect = m_parent->mapToScene(viewPortRect).boundingRect();

    QRectF viewRenderRectF = mapFromScene(visibleSceneRect).boundingRect();

    QRect viewRenderRect = QRect(static_cast<int>(viewRenderRectF.x()), static_cast<int>(viewRenderRectF.y()),
                                 static_cast<int>(viewRenderRectF.width()), static_cast<int>(viewRenderRectF.height()));

    RenderTaskPDFL task;

    task.item = this;

    task.scaleFactor = m_scaleFactor;

    task.rotation = m_rotation;

    task.renderRect = viewRenderRect;

    BrowserRenderThread::clearVipTask(this);

    BrowserRenderThread::appendVipTask(task);
}

void BrowserPage::render(double scaleFactor, Dr::Rotation rotation, bool renderLater)
{
    if (nullptr == m_page)
        return;

    if (!m_image.isNull() && qFuzzyCompare(scaleFactor, m_imageScaleFactor) && rotation == m_rotation)
        return;

    m_scaleFactor = scaleFactor;

    if (!m_image.isNull())
        m_leftImage = m_image;

    if (m_rotation != rotation) {//旋转变化则强制移除
        m_leftImage = QImage();
        m_rotation  = rotation;
    }

    if (!renderLater) {
        //之后替换为线程reader
        BrowserRenderThread::clearTask(this);

        QList<RenderTaskPDFL> tasks;

        QRectF rect = boundingRect();

        if (rect.height() > 2000) {
            for (int i = 0 ; i * 400 < rect.height(); ++i) {
                int height = 400;

                if (rect.height() < i * 400)
                    height = static_cast<int>(rect.height() - 400 * i);

                QRect renderRect = QRect(0, 400 * i, static_cast<int>(boundingRect().width()), height);

                RenderTaskPDFL task;
                task.item = this;
                task.scaleFactor = m_scaleFactor;
                task.rotation = m_rotation;
                task.renderRect = renderRect;
                tasks.append(task);
            }
        } else {
            QRect renderRect = QRect(static_cast<int>(boundingRect().x()), static_cast<int>(boundingRect().y()),
                                     static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
            RenderTaskPDFL task;
            task.item = this;
            task.scaleFactor = m_scaleFactor;
            task.rotation = m_rotation;
            task.renderRect = renderRect;
            tasks.append(task);
        }

        BrowserRenderThread::appendTasks(tasks);
        m_imageScaleFactor = m_scaleFactor;

        loadAnnotations();

        foreach (BrowserAnnotation *annotationItem, m_annotationItems)
            annotationItem->setScaleFactorAndRotation(m_rotation);
    }

    m_image = QImage();

    update();
}

QImage BrowserPage::getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect)
{
    return m_page->render(rotation, scaleFactor, boundingRect);
}

QImage BrowserPage::getImage(int width, int height, Qt::AspectRatioMode mode)
{

    QSizeF size = m_page->sizeF().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode);
    QImage image = m_page->render(size.width(), size.height(), mode);
    image.setDevicePixelRatio(dApp->devicePixelRatio());
    return image;
}

QImage BrowserPage::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage BrowserPage::getImagePoint(double scaleFactor, QPoint point)
{
    QRect rect = QRect(static_cast<int>(point.x() * scaleFactor / m_scaleFactor - 122),
                       static_cast<int>(point .y() * scaleFactor / m_scaleFactor  - 122), 244, 244);

    return m_page->render(m_rotation, scaleFactor, rect);
}

void BrowserPage::handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect)
{
    if (!qFuzzyCompare(scaleFactor, m_scaleFactor) || rotation != m_rotation)
        return;

    if (rect.isNull()) {
        m_image = image;
    } else {
        if (m_image.isNull()) {
            m_image = QImage(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), QImage::Format_RGB32);
            m_image.fill(QColor(Qt::white));
        }
        QPainter painter(&m_image);
        painter.drawImage(rect, image);
    }
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

    m_wordRotation = Dr::NumberOfRotations;
    m_wordScaleFactor = -1;
    m_words.clear();
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

QPoint BrowserPage::point2Local(QPointF &point)
{
    QPoint tpoint(0, 0);

    qInfo() << this->boundingRect() << "   3333  "  << point << "        "  << this->boundingRect().contains(point);

    if (this->boundingRect().contains(point))
        qInfo() << __LINE__ << "  22222222222     in  page   "  << __FUNCTION__;

    return tpoint;
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
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected()) {
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
    if (nullptr == m_page /*|| text == "" || text.isEmpty()*/)
        return nullptr;

    Annotation *annot{nullptr};
//    QRectF trect = rect;
//    QPointF tpoint;

//    point2Local(tpoint);

    annot = m_page->addIconAnnotation(rect, text);

    reloadAnnotations();        // 必须要reload 因为可能多个item共享同一个annotation

    renderViewPort();

    return annot;
}

bool BrowserPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    if (nullptr == m_page)
        return false;

    return m_page->mouseClickIconAnnot(clickPoint);
}

//void BrowserPage::reload()
//{
//    double scaleFactor = m_scaleFactor;
//    Dr::Rotation rotation = m_rotation;

//    m_scaleFactor = -1;
//    m_rotation = Dr::NumberOfRotations;
//    m_wordRotation = Dr::NumberOfRotations;
//    m_annotationRotation = Dr::NumberOfRotations;
//    m_annotationScaleFactor = -1;

//    render(scaleFactor, rotation);
//}

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

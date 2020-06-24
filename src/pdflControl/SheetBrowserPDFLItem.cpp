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
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#include "SheetBrowserPDFLItem.h"
#include "document/Model.h"
#include "RenderThreadPDFL.h"
#include "SheetBrowserPDFL.h"
#include "SheetBrowserPDFLWord.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>

QSet<SheetBrowserPDFLItem *> SheetBrowserPDFLItem::items;
SheetBrowserPDFLItem::SheetBrowserPDFLItem(SheetBrowserPDFL *parent, deepin_reader::Page *page) : QGraphicsItem(), m_page(page), m_parent(parent)
{
    items.insert(this);
    setAcceptHoverEvents(true);
}

SheetBrowserPDFLItem::~SheetBrowserPDFLItem()
{
    items.remove(this);
    if (nullptr != m_page)
        delete m_page;
}

QRectF SheetBrowserPDFLItem::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, static_cast<double>(m_page->size().height() * m_scaleFactor), static_cast<double>(m_page->size().width() * m_scaleFactor));
    default: break;
    }

    return QRectF(0, 0, static_cast<double>(m_page->size().width() * m_scaleFactor), static_cast<double>(m_page->size().height() * m_scaleFactor));
}

QRectF SheetBrowserPDFLItem::bookmarkRect()
{
    return QRectF(boundingRect().width() - 40, 1, 39, 39);
}

QRectF SheetBrowserPDFLItem::bookmarkMouseRect()
{
    return QRectF(boundingRect().width() - 40, 10, 29, 29);
}

void SheetBrowserPDFLItem::setBookmark(bool hasBookmark)
{
    m_bookmark = hasBookmark;
    if (hasBookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;
    update();
}

void SheetBrowserPDFLItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (m_image.isNull() || !qFuzzyCompare(m_imageScaleFactor, m_scaleFactor)) {
        render(m_scaleFactor, m_rotation, false);
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

void SheetBrowserPDFLItem::render(double scaleFactor, Dr::Rotation rotation, bool renderLater, int beginRenderY)
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
        RenderThreadPDFL::clearTask(this);

        QList<RenderTaskPDFL> tasks;

        QRectF rect = boundingRect();

        if (rect.height() > 2000) {

            for (int i = 0 ; i * 400 < rect.height(); ++i) {
                int height = 400;

                if (rect.height() < i * 400)
                    height = rect.height() - 400 * i;

                QRect renderRect = QRect(0, 400 * i, boundingRect().width(), height);

                RenderTaskPDFL task;
                task.item = this;
                task.scaleFactor = m_scaleFactor;
                task.rotation = m_rotation;
                task.renderRect = renderRect;
                tasks.append(task);
            }
        } else {
            QRect renderRect = QRect(boundingRect().x(), boundingRect().y(), boundingRect().width(), boundingRect().height());
            RenderTaskPDFL task;
            task.item = this;
            task.scaleFactor = m_scaleFactor;
            task.rotation = m_rotation;
            task.renderRect = renderRect;
            tasks.append(task);
        }

        RenderThreadPDFL::appendTasks(tasks);
        m_imageScaleFactor = m_scaleFactor;

        //loadword
        if (m_wordRotation != m_rotation) {
            qDeleteAll(m_words);
            m_words.clear();

            QList<deepin_reader::Word> words = m_page->words(m_rotation);

            for (int i = 0; i < words.count(); ++i) {
                SheetBrowserPDFLWord *word = new SheetBrowserPDFLWord(this, words[i]);
                m_words.append(word);
            }
            m_wordRotation = m_rotation;
        }

        foreach (SheetBrowserPDFLWord *word, m_words)
            word->setScaleFactor(m_scaleFactor);
    }

    m_image = QImage();

    update();
}

QImage SheetBrowserPDFLItem::getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect)
{
    return m_page->render(rotation, scaleFactor, boundingRect);
}

QImage SheetBrowserPDFLItem::getImage(int width, int height, Qt::AspectRatioMode mode)
{
    return m_page->render(width, height, mode);
}

QImage SheetBrowserPDFLItem::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage SheetBrowserPDFLItem::getImagePoint(double scaleFactor, QPoint point)
{
    QRect rect = QRect(static_cast<int>(point.x() * scaleFactor / m_scaleFactor - 122),
                       static_cast<int>(point .y() * scaleFactor / m_scaleFactor  - 122), 244, 244);

    return m_page->render(m_rotation, scaleFactor, rect);
}

void SheetBrowserPDFLItem::handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect)
{
    if (!qFuzzyCompare(scaleFactor, m_scaleFactor) || rotation != m_rotation)
        return;

    if (rect.isNull()) {
        m_image = image;
    } else {
        if (m_image.isNull())
            m_image = QImage(boundingRect().width(), boundingRect().height(), QImage::Format_RGB32);
        QPainter painter(&m_image);
        painter.drawImage(rect, image);
    }
    update();
}

bool SheetBrowserPDFLItem::existInstance(SheetBrowserPDFLItem *item)
{
    return items.contains(item);
}

void SheetBrowserPDFLItem::setItemIndex(int itemIndex)
{
    m_index = itemIndex;
}

int SheetBrowserPDFLItem::itemIndex()
{
    return m_index;
}

void SheetBrowserPDFLItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (bookmarkMouseRect().contains(event->pos())) {
        m_bookmarkState = 2;
        if (nullptr != m_parent) {
            m_parent->needBookmark(m_index, !m_bookmark);
            if (!m_bookmark && bookmarkMouseRect().contains(event->pos()))
                m_bookmarkState = 1;
            else if (m_bookmark)
                m_bookmarkState = 3;
            else
                m_bookmarkState = 0;
        }
        update();
    }

    QGraphicsItem::mousePressEvent(event);
}

bool SheetBrowserPDFLItem::sceneEvent(QEvent *event)
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

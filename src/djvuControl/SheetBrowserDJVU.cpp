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
#include "SheetBrowserDJVU.h"
#include "document/DjVuModel.h"
#include "SheetBrowserDJVUItem.h"
#include "DocSheetDJVU.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QBitmap>
#include <DMenu>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

SheetBrowserDJVU::SheetBrowserDJVU(DocSheetDJVU *parent) : DGraphicsView(parent), m_sheet(parent)
{
    setScene(new QGraphicsScene());

    setFrameShape(QFrame::NoFrame);

    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QGraphicsView{background-color:white}");     //由于DTK bug不响应WA_TranslucentBackground参数写死了颜色，这里加样式表让dtk style失效

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScrollBarValueChanged(int)));

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));
}

SheetBrowserDJVU::~SheetBrowserDJVU()
{
    qDeleteAll(m_items);

    if (nullptr != m_document)
        delete m_document;
}

bool SheetBrowserDJVU::openFilePath(const QString &filePath)
{
    m_document = deepin_reader::DjVuDocument::loadDocument(filePath);

    if (nullptr == m_document)
        return false;

    return true;
}

bool SheetBrowserDJVU::loadPages(DocOperation &operation, const QSet<int> &bookmarks)
{
    if (nullptr == m_document)
        return false;

    for (int i = 0; i < m_document->numberOfPages(); ++i) {
        deepin_reader::Page *page = m_document->page(i);

        if (page == nullptr)
            return false;

        if (page->size().width() > m_maxWidth)
            m_maxWidth = page->size().width();

        if (page->size().height() > m_maxHeight)
            m_maxHeight = page->size().height();

        SheetBrowserDJVUItem *item = new SheetBrowserDJVUItem(this, page);
        item->setItemIndex(i);

        if (bookmarks.contains(i))
            item->setBookmark(true);

        m_items.append(item);
        scene()->addItem(item);
    }

    m_initPage = operation.currentPage;

    if (m_initPage <= 1)
        m_initPage = 1;

    if (m_document->numberOfPages() < 1)
        m_initPage = 0;

    if (m_initPage > m_document->numberOfPages())
        m_initPage = m_document->numberOfPages();

    loadMouseShape(operation);

    deform(operation);

    m_hasLoaded = true;

    return true;
}

QImage SheetBrowserDJVU::firstThumbnail(const QString &filePath)
{
    deepin_reader::Document *document = deepin_reader::DjVuDocument::loadDocument(filePath);

    if (nullptr == document)
        return QImage();

    if (document->numberOfPages() <= 0)
        return QImage();

    deepin_reader::Page *page = document->page(0);

    if (page == nullptr)
        return QImage();

    QImage image = page->render(Dr::RotateBy0);

    delete page;

    delete document;

    return image;
}

void SheetBrowserDJVU::loadMouseShape(DocOperation &operation)
{
    if (Dr::MouseShapeHand == operation.mouseShape) {
        operation.mouseShape = Dr::MouseShapeHand;
        setDragMode(QGraphicsView::ScrollHandDrag);
    } else if (Dr::MouseShapeNormal == operation.mouseShape) {
        operation.mouseShape = Dr::MouseShapeNormal;
        setDragMode(QGraphicsView::NoDrag);
    }
}

void SheetBrowserDJVU::setBookMark(int index, int state)
{
    if (m_items.count() > index) {
        m_items.at(index)->setBookmark(state);
    }
}

void SheetBrowserDJVU::onVerticalScrollBarValueChanged(int)
{
    pageChanged(visibleCurrentPage());
}

void SheetBrowserDJVU::onCustomContextMenuRequested(const QPoint &point)
{
    closeMagnifier();

    int index = currentPage() - 1;

    if (index < 0 || index > allPages() - 1)
        return;

    QMenu menu(this);
    if (m_sheet->hasBookMark(index))
        menu.addAction(tr("Remove bookmark"), [this, index]() {
        m_sheet->setBookMark(index, false);
    });
    else
        menu.addAction(tr("Add bookmark"), [this, index]() {
        m_sheet->setBookMark(index, true);
    });

    menu.addSeparator();

    QAction *firstPageAction = new QAction(tr("First Page"), &menu);
    connect(firstPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageFirst();
    });
    menu.addAction(firstPageAction);
    if (index == 0) {
        firstPageAction->setDisabled(true);
    }

    QAction *previousPageAction = new QAction(tr("Previous Page"), &menu);
    connect(previousPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPagePrev();
    });
    menu.addAction(previousPageAction);
    if (index == 0) {
        previousPageAction->setDisabled(true);
    }

    QAction *nextPageAction = new QAction(tr("Next Page"), &menu);
    connect(nextPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageNext();
    });
    menu.addAction(nextPageAction);
    if (index == m_items.count() - 1) {
        nextPageAction->setDisabled(true);
    }

    QAction *lastPageAction = new QAction(tr("Last Page"), &menu);
    connect(lastPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageLast();
    });
    menu.addAction(lastPageAction);
    if (index == m_items.count() - 1) {
        lastPageAction->setDisabled(true);
    }

    menu.move(mapToGlobal(point));
    menu.exec();
}

void SheetBrowserDJVU::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {

        DocSheet *sheet = static_cast<DocSheet *>(parentWidget());
        if (nullptr == sheet)
            return;

        if (event->delta() > 0) {
            emit sigWheelUp();
        } else {
            emit sigWheelDown();
        }
        return;
    }

    QGraphicsView::wheelEvent(event);
}

void SheetBrowserDJVU::deform(DocOperation &operation)
{
    m_lastScaleFactor = operation.scaleFactor;

    int page = currentPage();

    qreal pageDiffFactor = 0;

    if (page > 0 && page <= m_items.count()) {
        qreal diff = verticalScrollBar()->value() - m_items.at(page - 1)->pos().y();
        pageDiffFactor = diff / m_items.at(page - 1)->boundingRect().height();
    }

    int width = 0;
    int height = 0;

    switch (operation.rotation) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxWidth) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxHeight);
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxWidth) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxHeight > this->height())
                scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxHeight);
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxHeight) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxWidth);
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxHeight) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxWidth > this->height())
                scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxWidth);
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    }

    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->render(operation.scaleFactor, operation.rotation, true);
    }

    if (Dr::SinglePageMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            m_items.at(i)->setPos(0, height);
            height += m_items.at(i)->boundingRect().height() + 5;
            if (m_items.at(i)->boundingRect().width() > width)
                width = static_cast<int>(m_items.at(i)->boundingRect().width());
        }
    } else if (Dr::TwoPagesMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            if (i % 2 == 1)
                continue;

            if (m_items.count() > i + 1) {
                m_items.at(i)->setPos(0, height);
                m_items.at(i + 1)->setPos(m_items.at(i)->boundingRect().width() + 5, height);
            } else {
                m_items.at(i)->setPos(0, height);
            }

            if (m_items.count() > i + 1) {
                height +=  qMax(m_items.at(i)->boundingRect().height(), m_items.at(i + 1)->boundingRect().height()) + 5;
            } else
                height += m_items.at(i)->boundingRect().height() + 5;

            if (m_items.count() > i + 1) {
                if (m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width() > width)
                    width = static_cast<int>(m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width());
            } else {
                if (m_items.at(i)->boundingRect().width() > width)
                    width = static_cast<int>(m_items.at(i)->boundingRect().width());
            }
        }
    }

    setSceneRect(0, 0, width, height);

    if (page > 0 && page <= m_items.count()) {
        verticalScrollBar()->setValue(static_cast<int>(m_items[page - 1]->pos().y() + m_items[page - 1]->boundingRect().height()*pageDiffFactor));
    }
}

bool SheetBrowserDJVU::hasLoaded()
{
    return m_hasLoaded;
}

void SheetBrowserDJVU::resizeEvent(QResizeEvent *event)
{
    if (hasLoaded()) {
        sigSizeChanged();
    }

    QGraphicsView::resizeEvent(event);
}

void SheetBrowserDJVU::mouseMoveEvent(QMouseEvent *event)
{
    if (m_magnifierLabel) {
        QPoint mousePos = event->pos();

        if (mousePos.y() < 122) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (122 - mousePos.y()));
            mousePos.setY(122);
        }

        if (mousePos.x() < 122) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (122 - mousePos.x()));
            mousePos.setX(122);
        }

        if (mousePos.y() > (this->size().height() - 122) && (this->size().height() - 122 > 0)) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() + (mousePos.y() - (this->size().height() - 122)));
            mousePos.setY(this->size().height() - 122);
        }

        if (mousePos.x() > (this->size().width() - 122) && (this->size().width() - 122 > 0)) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + (mousePos.x() - (this->size().width() - 122)));
            mousePos.setX(this->size().width() - 122);
        }

        QImage image;
        QPixmap pix(244, 244);
        pix.fill(Qt::transparent);
        if (getImagePoint(mousePos, m_lastScaleFactor + 2, image) && !image.isNull()) {
            QPainter painter(&pix);
            QPainterPath clippath;
            clippath.addRoundedRect(17, 17, 210, 210, 105, 105);
            painter.setClipPath(clippath);
            painter.drawImage(0, 0, image);
            painter.end();
        } else {
            QPainter painter(&pix);
            QPainterPath clippath;
            clippath.addRoundedRect(17, 17, 210, 210, 105, 105);
            painter.setClipPath(clippath);
            painter.fillRect(0, 0, 244, 244, Qt::white);
            painter.end();
        }

        QPainter painter(&pix);
        painter.drawPixmap(0, 0, 244, 244, QIcon::fromTheme(Pri::g_module + "maganifier").pixmap(QSize(244, 244)));
        m_magnifierLabel->setPixmap(pix);
        m_magnifierLabel->move(QPoint(mousePos.x() - 122, mousePos.y() - 122));

    }

    QGraphicsView::mouseMoveEvent(event);

}

int SheetBrowserDJVU::allPages()
{
    return m_items.count();
}

int SheetBrowserDJVU::visibleCurrentPage()
{
    int value = verticalScrollBar()->value();

    int index = 0;

    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items[i]->pos().y() + m_items[i]->boundingRect().height() >= value) {
            index = i;
            break;
        }
    }

    if (value == verticalScrollBar()->maximum())
        index = m_items.count() - 1;

    return index + 1;
}

int SheetBrowserDJVU::currentPage()
{
    if (m_curPage > 0)
        return m_curPage;

    return visibleCurrentPage();
}

int SheetBrowserDJVU::viewPointInIndex(QPoint viewPoint)
{
    SheetBrowserDJVUItem *item  = static_cast<SheetBrowserDJVUItem *>(itemAt(viewPoint));

    return m_items.indexOf(item);
}

void SheetBrowserDJVU::pageChanged(int page)
{
    if (page != m_curPage) {
        m_curPage = page;

        emit sigPageChanged(m_curPage);
    }
}

void SheetBrowserDJVU::setCurrentPage(int page)
{
    if (page < 1 && page > allPages())
        return;

    verticalScrollBar()->setValue(static_cast<int>(m_items.at(page - 1)->pos().y()));

    pageChanged(page);
}

bool SheetBrowserDJVU::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    if (m_items.count() <= index)
        return false;

    image = m_items.at(index)->getImage(static_cast<int>(width), static_cast<int>(height), mode);

    return true;
}

bool SheetBrowserDJVU::getImagePoint(QPoint viewPoint, double scaleFactor, QImage &image)
{
    SheetBrowserDJVUItem *item  = static_cast<SheetBrowserDJVUItem *>(itemAt(viewPoint));

    if (nullptr == item)
        return false;

    QPointF itemPoint = item->mapFromScene(mapToScene(viewPoint));

    if (!item->contains(itemPoint))
        return false;

    QPoint point = QPoint(static_cast<int>(itemPoint.x()), static_cast<int>(itemPoint.y()));

    image = item->getImagePoint(scaleFactor, point);

    return true;
}

void SheetBrowserDJVU::openMagnifier()
{
    if (nullptr == m_magnifierLabel) {
        m_magnifierLabel = new QLabel(this);
        m_magnifierLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_magnifierLabel->setWindowFlag(Qt::FramelessWindowHint);
        m_magnifierLabel->setAutoFillBackground(false);
        m_magnifierLabel->setAttribute(Qt::WA_TranslucentBackground);
        m_magnifierLabel->resize(244, 244);
        m_magnifierLabel->show();
        setDragMode(QGraphicsView::NoDrag);
        setMouseTracking(true);
        setCursor(QCursor(Qt::BlankCursor));
    }
}

void SheetBrowserDJVU::closeMagnifier()
{
    if (nullptr != m_magnifierLabel) {
        m_magnifierLabel->hide();
        m_magnifierLabel->deleteLater();
        m_magnifierLabel = nullptr;

        setMouseTracking(false);
        setCursor(QCursor(Qt::ArrowCursor));
        if (Dr::MouseShapeHand == m_sheet->operation().mouseShape) {
            setDragMode(QGraphicsView::ScrollHandDrag);
        } else if (Dr::MouseShapeNormal == m_sheet->operation().mouseShape) {
            setDragMode(QGraphicsView::NoDrag);
        }
    }
}

bool SheetBrowserDJVU::magnifierOpened()
{
    return (nullptr != m_magnifierLabel) ;
}

int SheetBrowserDJVU::maxWidth()
{
    return m_maxWidth;
}

int SheetBrowserDJVU::maxHeight()
{
    return m_maxHeight;
}

void SheetBrowserDJVU::needBookmark(int index, bool state)
{
    emit sigNeedBookMark(index, state);
}

void SheetBrowserDJVU::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void SheetBrowserDJVU::showEvent(QShowEvent *event)
{
    if (0 != m_initPage) {
        setCurrentPage(m_initPage);
        m_initPage = 0;
    }

    QGraphicsView::showEvent(event);
}


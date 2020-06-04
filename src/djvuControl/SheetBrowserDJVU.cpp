#include "SheetBrowserDJVU.h"
#include "document/djvumodel.h"
#include "SheetBrowserDJVUItem.h"
#include "DocSheetDJVU.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QBitmap>
#include <DMenu>

SheetBrowserDJVU::SheetBrowserDJVU(DocSheetDJVU *parent) : QGraphicsView(parent), m_sheet(parent)
{
    setScene(new QGraphicsScene());

    setFrameShape(QFrame::NoFrame);

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

void SheetBrowserDJVU::loadPages(DocOperation &operation, const QSet<int> &bookmarks)
{
    if (nullptr == m_document)
        return;

    for (int i = 0; i < m_document->numberOfPages(); ++i) {
        deepin_reader::Page *page = m_document->page(i);
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

    loadMouseShape(operation);
    deform(operation);
    m_initPage = operation.currentPage;
    m_hasLoaded = true;
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

void SheetBrowserDJVU::onVerticalScrollBarValueChanged(int value)
{
    emit sigPageChanged(currentPage());
}

void SheetBrowserDJVU::onCustomContextMenuRequested(const QPoint &point)
{
    closeMagnifier();

    SheetBrowserDJVUItem *item  = static_cast<SheetBrowserDJVUItem *>(itemAt(QPoint(point)));
    if (nullptr == item)
        return;

    int index = m_items.indexOf(item);

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

    menu.addAction(tr("First page"), [this]() {
        this->emit sigNeedPageFirst();
    });
    menu.addAction(tr("Previous page"), [this]() {
        this->emit sigNeedPagePrev();
    });
    menu.addAction(tr("Next page"), [this]() {
        this->emit sigNeedPageNext();
    });
    menu.addAction(tr("Last page"), [this]() {
        this->emit sigNeedPageLast();
    });
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
    }

    QGraphicsView::wheelEvent(event);
}

void SheetBrowserDJVU::deform(DocOperation &operation)
{
    m_lastScaleFactor = operation.scaleFactor;

    int page = currentPage();

    int diff = 0;

    if (page > 0 && page <= m_items.count())
        diff = verticalScrollBar()->value() - m_items.at(page - 1)->pos().y();

    int width = 0;
    int height = 0;

    switch (operation.rotation) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = ((double)this->width() - 25.0) / (double) m_maxWidth / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = (double)this->height() / (double) m_maxHeight ;
        else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = ((double)this->width() - 25.0) / (double) m_maxHeight / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = (double)this->height() / (double) m_maxWidth ;
        else
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
                width = m_items.at(i)->boundingRect().width();
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
                    width = m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width();
            } else {
                if (m_items.at(i)->boundingRect().width() > width)
                    width = m_items.at(i)->boundingRect().width();
            }
        }
    }

    setSceneRect(0, 0, width, height);

    if (page > 0 && page <= m_items.count())
        verticalScrollBar()->setValue(m_items[page - 1]->pos().y() + diff);
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

int SheetBrowserDJVU::currentPage()
{
    int value = verticalScrollBar()->value();

    int index = 0;

    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items[i]->pos().y() + m_items[i]->boundingRect().height() >= value) {
            index = i;
            break;
        }
    }

    return index + 1;
}

int SheetBrowserDJVU::viewPointInIndex(QPoint viewPoint)
{
    SheetBrowserDJVUItem *item  = static_cast<SheetBrowserDJVUItem *>(itemAt(viewPoint));

    return m_items.indexOf(item);
}

void SheetBrowserDJVU::setCurrentPage(int page)
{
    if (page < 1 && page > allPages())
        return;

    verticalScrollBar()->setValue(m_items.at(page - 1)->pos().y());
}

bool SheetBrowserDJVU::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    if (m_items.count() <= index)
        return false;

    image = m_items.at(index)->getImage(width, height, mode);

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

    QPoint point = QPoint(itemPoint.x(), itemPoint.y());

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
    if (1 != m_initPage) {
        setCurrentPage(m_initPage);
        m_initPage = 1;
    }

    QGraphicsView::showEvent(event);
}


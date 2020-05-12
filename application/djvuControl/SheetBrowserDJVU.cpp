#include "SheetBrowserDJVU.h"
#include "document/djvumodel.h"
#include "SheetBrowserDJVUItem.h"
#include "DocSheet.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>

SheetBrowserDJVU::SheetBrowserDJVU(QWidget *parent) : QGraphicsView(parent)
{
    setScene(new QGraphicsScene());

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScrollBarValueChanged(int)));
}

SheetBrowserDJVU::~SheetBrowserDJVU()
{
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

void SheetBrowserDJVU::loadPages(Dr::ScaleMode mode, double scaleFactor, Dr::Rotation rotation, Dr::MouseShape mouseShape, int currentPage)
{
    if (nullptr == m_document)
        return;

    for (int i = 0; i < m_document->numberOfPages(); ++i) {
        deepin_reader::Page *page = m_document->page(i);
        if (page->size().width() > m_maxWidth)
            m_maxWidth = page->size().width();
        if (page->size().height() > m_maxHeight)
            m_maxHeight = page->size().height();

        SheetBrowserDJVUItem *item = new SheetBrowserDJVUItem(page);
        m_items.append(item);
        scene()->addItem(item);
    }

    mode = Dr::FitToPageWidthMode;
    setScale(mode, scaleFactor, rotation);
    setMouseShape(mouseShape);
    m_initPage = 5;
    m_hasLoaded = true;
}

void SheetBrowserDJVU::setScale(Dr::ScaleMode mode, double scaleFactor, Dr::Rotation rotation)
{
    m_scaleFactor = scaleFactor;
    m_scaleMode   = mode;
    m_rotion      = rotation;
    deform();
}

Dr::MouseShape SheetBrowserDJVU::setMouseShape(Dr::MouseShape mouseShape)
{
    if (Dr::MouseShapeHand == mouseShape) {
        m_mouseShape = mouseShape;
        setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        m_mouseShape = Dr::MouseShapeNormal;
        setDragMode(QGraphicsView::NoDrag);
    }
    return m_mouseShape;
}

void SheetBrowserDJVU::onVerticalScrollBarValueChanged(int value)
{
    emit sigPageChanged(currentPage());
}

void SheetBrowserDJVU::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {

        DocSheet *sheet = static_cast<DocSheet *>(parentWidget());
        if (nullptr == sheet)
            return;

        if (event->delta() > 0) {
            sheet->zoomin();
        } else {
            sheet->zoomout();
        }
    }

    QGraphicsView::wheelEvent(event);
}

void SheetBrowserDJVU::deform()
{
    int page = currentPage();

    switch (m_rotion) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == m_scaleMode) {
            m_scaleFactor = ((double)this->width() - 25.0)  / (double) m_maxWidth ;
        } else if (Dr::FitToPageHeightMode == m_scaleMode) {
            m_scaleFactor = (double)this->height() / (double) m_maxHeight ;
        } else {
            m_scaleMode = Dr::ScaleFactorMode;
        }
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == m_scaleMode) {
            m_scaleFactor = ((double)this->width() - 25.0)   / (double) m_maxHeight ;
        } else if (Dr::FitToPageHeightMode == m_scaleMode) {
            m_scaleFactor = (double)this->height() / (double) m_maxWidth ;
        } else {
            m_scaleMode = Dr::ScaleFactorMode;
        }
        break;
    }

    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->render(m_scaleFactor, m_rotion, true);
    }

    int width = 0;
    int height = 0;
    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->setPos(0, height);

        height += m_items.at(i)->boundingRect().height() + 5;

        if (m_items.at(i)->boundingRect().width() > width)
            width = m_items.at(i)->boundingRect().width();

        setCurrentPage(page);
    }

    setSceneRect(0, 0, width, height);

    emit sigScaleChanged(m_scaleMode, m_scaleFactor);
}

bool SheetBrowserDJVU::hasLoaded()
{
    return m_hasLoaded;
}

void SheetBrowserDJVU::resizeEvent(QResizeEvent *event)
{
    if (hasLoaded() && Dr::ScaleFactorMode != m_scaleMode) {
        deform();
    }
    QGraphicsView::resizeEvent(event);
}

int SheetBrowserDJVU::allPages()
{
    return m_items.count();
}

int SheetBrowserDJVU::currentPage()
{
    SheetBrowserDJVUItem *item = static_cast<SheetBrowserDJVUItem *>(itemAt(QPoint(1, 1)));
    if (nullptr == item)
        return 1;

    return m_items.indexOf(item) + 1;
}

void SheetBrowserDJVU::setCurrentPage(int page)
{
    if (page < 1 && page > allPages())
        return;

    int height = 0;
    for (int i = 0; i < page - 1; ++i) {
        height += m_items.at(i)->boundingRect().height() + 5;
    }

    verticalScrollBar()->setValue(height);
}

void SheetBrowserDJVU::showEvent(QShowEvent *event)
{
    if (1 != m_initPage) {
        setCurrentPage(5);
        m_initPage = 1;
    }

    QGraphicsView::showEvent(event);
}

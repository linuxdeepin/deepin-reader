#include "SheetBrowserDJVU.h"
#include "document/djvumodel.h"
#include "SheetBrowserDJVUItem.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>

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

    setScale(mode, scaleFactor, rotation);
    setMouseShape(mouseShape);
    setCurrentPage(5);
}

void SheetBrowserDJVU::setScale(Dr::ScaleMode mode, double scaleFactor, Dr::Rotation rotation)
{
//保持中心不变的放大
//    QPointF center = my_qgv->viewPort().rect().center();
//    center = my_qgv->mapToScene(center);
//    ...
//    // user edit; reconstruct scene
//    //
//    my_qgv->centerOn(center);


    m_mode   = mode;
    m_rotion = rotation;

    switch (m_rotion) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == m_mode) {
            m_scaleFactor = (double)this->width()  / (double) m_maxWidth ;
        } else if (Dr::FitToPageHeightMode == m_mode) {
            m_scaleFactor = (double)this->height() / (double) m_maxHeight ;
        } else {
            m_mode = Dr::ScaleFactorMode;
            m_scaleFactor = scaleFactor;
        }
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == m_mode) {
            m_scaleFactor = (double)this->width()  / (double) m_maxHeight ;
        } else if (Dr::FitToPageHeightMode == m_mode) {
            m_scaleFactor = (double)this->height() / (double) m_maxWidth ;
        } else {
            m_mode = Dr::ScaleFactorMode;
            m_scaleFactor = scaleFactor;
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

    }

    scene()->setSceneRect(0, 0, width, height);
}

void SheetBrowserDJVU::setMouseShape(Dr::MouseShape mouseShape)
{
    m_mouseShape = mouseShape;
    if (Dr::MouseShapeHand == m_mouseShape)
        setDragMode(QGraphicsView::ScrollHandDrag);
    else
        setDragMode(QGraphicsView::NoDrag);
}

void SheetBrowserDJVU::setCurrentPage(int page)
{
    scrollValue = 0;
    for (int i = 0; i < page - 1; ++i) {
        if (m_items.count() <= i)
            break;
        scrollValue += m_items.at(i)->boundingRect().height() + 5;
    }

    QTimer::singleShot(1, this, SLOT(onScroll()));
}

void SheetBrowserDJVU::onVerticalScrollBarValueChanged(int value)
{
    int y = 0;
    for (int i = 0; i < m_items.count(); ++i) {
        y += m_items.at(i)->boundingRect().height() + 5;
        if (y > value) {
            emit sigScrollPage(i + 1);
            break;
        }
    }
}

void SheetBrowserDJVU::onScroll()
{
    verticalScrollBar()->setValue(scrollValue);
}

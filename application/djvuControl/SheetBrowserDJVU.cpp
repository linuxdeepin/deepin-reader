#include "SheetBrowserDJVU.h"
#include "document/djvumodel.h"
#include "SheetBrowserDJVUItem.h"

#include <QDebug>
#include <QGraphicsItem>
#
SheetBrowserDJVU::SheetBrowserDJVU(QWidget *parent) : QGraphicsView(parent)
{
    setScene(new QGraphicsScene());

    //setDragMode(QGraphicsView::ScrollHandDrag);
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

void SheetBrowserDJVU::loadPages()
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

    setScale(m_mode, m_scale, m_rotion);
}

void SheetBrowserDJVU::setScale(Dr::ScaleMode mode, double scale, Dr::Rotation rotation)
{
    m_mode   = mode;
    m_rotion = rotation;

    switch (m_rotion) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == m_mode) {
            m_scale = (double)this->width() * 100.00 / (double) m_maxWidth ;
        } else if (Dr::FitToPageHeightMode == m_mode) {
            m_scale = (double)this->height() * 100.00 / (double) m_maxHeight ;
        } else {
            m_mode = Dr::ScaleFactorMode;
            m_scale = scale;
        }
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == m_mode) {
            m_scale = (double)this->width() * 100.00 / (double) m_maxHeight ;
        } else if (Dr::FitToPageHeightMode == m_mode) {
            m_scale = (double)this->height() * 100.00 / (double) m_maxWidth ;
        } else {
            m_mode = Dr::ScaleFactorMode;
            m_scale = scale;
        }
        break;
    }

    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->render(m_scale, m_rotion, true);
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

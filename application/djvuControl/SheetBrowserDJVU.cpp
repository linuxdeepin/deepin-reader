#include "SheetBrowserDJVU.h"
#include "document/djvumodel.h"

SheetBrowserDJVU::SheetBrowserDJVU(QWidget *parent) : QGraphicsView(parent)
{
    setScene(new QGraphicsScene());
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

    }
}

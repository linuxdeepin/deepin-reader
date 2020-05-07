#include "DocSheetDJVU.h"
#include "SheetBrowserDJVU.h"

#include <QDebug>

DocSheetDJVU::DocSheetDJVU(QWidget *parent) : DocSheet(Dr::DjVu, parent)
{
    m_browser = new SheetBrowserDJVU(this);
}

DocSheetDJVU::~DocSheetDJVU()
{

}

void DocSheetDJVU::zoomin()
{
    QList<int> dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};

    m_browser->setScale(Dr::ScaleFactorMode, 150, Dr::RotateBy0);
}

void DocSheetDJVU::zoomout()
{
    m_browser->setScale(Dr::ScaleFactorMode, 100, Dr::RotateBy90);
}

bool DocSheetDJVU::openFileExec(const QString &filePath)
{
    if (m_browser->openFilePath(filePath)) {
        //readOperation();

        m_browser->loadPages(operation().scaleMode, operation().scale, operation().rotation, operation().mouseShape, operation().currentPage);
        return true;
    }

    return false;
}

#include "DocSheetDJVU.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowserDJVU.h"

#include <QDebug>

DocSheetDJVU::DocSheetDJVU(QString filePath, QWidget *parent) : DocSheet(Dr::DjVu, filePath, parent)
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_sidebar = new SheetSidebar(this);
    m_browser = new SheetBrowserDJVU(this);

    addWidget(m_sidebar);
    addWidget(m_browser);

    setAcceptDrops(true);
}

DocSheetDJVU::~DocSheetDJVU()
{

}

void DocSheetDJVU::zoomin()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > (operation().scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheetDJVU::zoomout()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = dataList.count() - 1; i >= 0; --i) {
        if (dataList[i] < (operation().scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheetDJVU::setScaleFactor(qreal scaleFactor)
{
    operation().scaleMode = Dr::ScaleFactorMode;
    operation().scaleFactor = scaleFactor;

    m_browser->setScale(Dr::ScaleFactorMode, scaleFactor, operation().rotation);
    emit sigFileChanged(this);
}

bool DocSheetDJVU::openFileExec()
{
    if (m_browser->openFilePath(filePath())) {
        //readOperation();

        m_browser->loadPages(operation().scaleMode, operation().scaleFactor, operation().rotation, operation().mouseShape, operation().currentPage);
        return true;
    }

    return false;
}

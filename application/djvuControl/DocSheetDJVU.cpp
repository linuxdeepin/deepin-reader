#include "DocSheetDJVU.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowserDJVU.h"

#include <QDebug>

DocSheetDJVU::DocSheetDJVU(QString filePath, QWidget *parent) : DocSheet(Dr::DjVu, filePath, parent)
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_sidebar = new SheetSidebar(this);
    m_sidebar->setVisible(true);

    m_browser = new SheetBrowserDJVU(this);
    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onBrowserPageChanged(int)));
    connect(m_browser, SIGNAL(sigScaleChanged(Dr::ScaleMode, qreal)), this, SLOT(onBrowserScaleChanged(Dr::ScaleMode, qreal)));

    addWidget(m_sidebar);
    addWidget(m_browser);

    setAcceptDrops(true);
}

DocSheetDJVU::~DocSheetDJVU()
{

}

void DocSheetDJVU::pageJump(int page)
{
    m_browser->setCurrentPage(page);
}

void DocSheetDJVU::jumpToIndex(int index)
{
    m_browser->setCurrentPage(index + 1);
}

void DocSheetDJVU::jumpToFirstPage()
{
    m_browser->setCurrentPage(1);
}

void DocSheetDJVU::jumpToLastPage()
{
    pageJump(m_browser->allPages());
}

void DocSheetDJVU::jumpToNextPage()
{
    int page = m_browser->currentPage() + (operation().layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page >= m_browser->allPages() ? m_browser->allPages() : page;

    pageJump(page);
}

void DocSheetDJVU::jumpToPrevPage()
{
    int page = m_browser->currentPage() - (operation().layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page < 1 ? 1 : page;

    pageJump(page);
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
        m_browser->loadPages(operation().scaleMode, operation().scaleFactor, operation().rotation, operation().mouseShape, operation().currentPage);
        return true;
    }

    return false;
}

void DocSheetDJVU::setMouseShape(Dr::MouseShape shape)
{
    operation().mouseShape = m_browser->setMouseShape(shape);
    emit sigFileChanged(this);
}

void DocSheetDJVU::setScaleMode(Dr::ScaleMode mode)
{
    m_browser->setScale(operation().scaleMode, 0, operation().rotation);
}

void DocSheetDJVU::onBrowserPageChanged(int page)
{

}

void DocSheetDJVU::onBrowserScaleChanged(Dr::ScaleMode mode, qreal scaleFactor)
{
    if (operation().scaleMode != mode || operation().scaleFactor != scaleFactor) {
        operation().scaleMode = mode;
        operation().scaleFactor = scaleFactor;
        emit sigFileChanged(this);
    }
}

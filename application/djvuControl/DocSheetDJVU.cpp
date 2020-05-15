#include "DocSheetDJVU.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowserDJVU.h"

#include <QDebug>

DocSheetDJVU::DocSheetDJVU(QString filePath, QWidget *parent) : DocSheet(Dr::DjVu, filePath, parent)
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL);

    m_browser = new SheetBrowserDJVU(this);
    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onBrowserPageChanged(int)));
    connect(m_browser, SIGNAL(sigScaleChanged(Dr::ScaleMode, qreal)), this, SLOT(onBrowserScaleChanged(Dr::ScaleMode, qreal)));

    setAcceptDrops(true);
}

DocSheetDJVU::~DocSheetDJVU()
{

}

void DocSheetDJVU::jumpToPage(int page)
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
    jumpToPage(m_browser->allPages());
}

void DocSheetDJVU::jumpToNextPage()
{
    int page = m_browser->currentPage() + (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page >= m_browser->allPages() ? m_browser->allPages() : page;

    jumpToPage(page);
}

void DocSheetDJVU::jumpToPrevPage()
{
    int page = m_browser->currentPage() - (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page < 1 ? 1 : page;

    jumpToPage(page);
}

void DocSheetDJVU::zoomin()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheetDJVU::zoomout()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = dataList.count() - 1; i >= 0; --i) {
        if (dataList[i] < (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheetDJVU::rotateLeft()
{
    if (Dr::RotateBy0 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy270;
    else if (Dr::RotateBy270 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy180;
    else if (Dr::RotateBy180 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy90;
    else if (Dr::RotateBy90 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy0;

    m_browser->setRotation(m_operation.rotation);
}

void DocSheetDJVU::rotateRight()
{
    if (Dr::RotateBy0 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy90;
    else if (Dr::RotateBy90 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy180;
    else if (Dr::RotateBy180 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy270;
    else if (Dr::RotateBy270 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy0;

    m_browser->setRotation(m_operation.rotation);
}

void DocSheetDJVU::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode >= 0 && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        m_browser->setLayoutMode(mode);
    }
}

void DocSheetDJVU::setScaleFactor(qreal scaleFactor)
{
    m_operation.scaleMode = Dr::ScaleFactorMode;
    m_operation.scaleFactor = scaleFactor;
    m_browser->setScale(Dr::ScaleFactorMode, scaleFactor, m_operation.rotation);
}

bool DocSheetDJVU::openFileExec()
{
    if (m_browser->openFilePath(filePath())) {
        m_browser->loadPages(m_operation.scaleMode, m_operation.scaleFactor, m_operation.rotation, m_operation.mouseShape, m_operation.currentPage);
        handleOpenSuccess();
        return true;
    }

    return false;
}

int DocSheetDJVU::pagesNumber()
{
    if (m_browser)
        return m_browser->allPages();

    return 0;
}

int DocSheetDJVU::currentPage()
{
    if (m_browser)
        return m_browser->currentPage();

    return 1;
}

int DocSheetDJVU::currentIndex()
{
    if (m_browser)
        return m_browser->currentPage() - 1;

    return 0;
}

void DocSheetDJVU::setMouseShape(Dr::MouseShape shape)
{
    if (shape > 0 && shape < Dr::NumberOfMouseShapes) {
        m_operation.mouseShape = shape;
        m_browser->setMouseShape(shape);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setScaleMode(Dr::ScaleMode mode)
{
    m_browser->setScale(mode, m_operation.scaleMode, m_operation.rotation);
}

bool DocSheetDJVU::getImageMax(int index, QImage &image, double max)
{
    if (m_browser)
        return m_browser->getImageMax(index, image, max);

    return false;
}

bool DocSheetDJVU::fileChanged()
{
    return false;
}

void DocSheetDJVU::onBrowserPageChanged(int page)
{
    operation().currentPage = page;
}

void DocSheetDJVU::onBrowserScaleChanged(Dr::ScaleMode mode, qreal scaleFactor)
{
    if (m_operation.scaleMode != mode || m_operation.scaleFactor != scaleFactor) {
        m_operation.scaleMode = mode;
        m_operation.scaleFactor = scaleFactor;
        emit sigFileChanged(this);
    }
}

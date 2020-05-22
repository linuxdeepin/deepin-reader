#include "DocSheetDJVU.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowserDJVU.h"
#include "database.h"

#include <QDebug>
#include <QFileInfo>

DocSheetDJVU::DocSheetDJVU(QString filePath, QWidget *parent) : DocSheet(Dr::DjVu, filePath, parent)
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_initBookmarks = m_bookmarks;

    m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL);

    m_browser = new SheetBrowserDJVU(this);
    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onBrowserPageChanged(int)));
    connect(m_browser, SIGNAL(sigSizeChanged()), this, SLOT(onBrowserSizeChanged()));
    connect(m_browser, SIGNAL(sigWheelUp()), this, SLOT(onBrowserWheelUp()));
    connect(m_browser, SIGNAL(sigWheelDown()), this, SLOT(onBrowserWheelDown()));
    connect(m_browser, SIGNAL(sigNeedPagePrev()), this, SLOT(onBrowserPagePrev()));
    connect(m_browser, SIGNAL(sigNeedPageNext()), this, SLOT(onBrowserPageNext()));
    connect(m_browser, SIGNAL(sigNeedPageFirst()), this, SLOT(onBrowserPageFirst()));
    connect(m_browser, SIGNAL(sigNeedPageLast()), this, SLOT(onBrowserPageLast()));
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

    m_browser->deform(m_operation);
    emit sigFileChanged(this);
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

    m_browser->deform(m_operation);
    emit sigFileChanged(this);
}

bool DocSheetDJVU::openFileExec()
{
    if (m_browser->openFilePath(filePath())) {
        m_browser->loadPages(m_operation);
        handleOpenSuccess();
        emit sigFileChanged(this);
        return true;
    }

    return false;
}

void DocSheetDJVU::setBookMark(int index, int state)
{
    if (state)
        m_bookmarks.insert(index);
    else
        m_bookmarks.remove(index);

    m_sidebar->setBookMark(index, state);
    m_browser->setBookMark(index, state);

    emit sigFileChanged(this);
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

void DocSheetDJVU::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode >= 0 && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setMouseShape(Dr::MouseShape shape)
{
    if (shape >= 0 && shape < Dr::NumberOfMouseShapes) {
        quitMagnifer();
        m_operation.mouseShape = shape;

        m_browser->loadMouseShape(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setScaleMode(Dr::ScaleMode mode)
{
    if (mode >= 0 && mode < Dr::NumberOfScaleModes) {
        m_operation.scaleMode = mode;
        m_browser->deform(m_operation);
    }
}

bool DocSheetDJVU::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode)
{
    if (m_browser)
        return m_browser->getImage(index, image, width, height);

    return false;
}

void DocSheetDJVU::setScaleFactor(qreal scaleFactor)
{
    m_operation.scaleMode = Dr::ScaleFactorMode;
    m_operation.scaleFactor = scaleFactor;
    m_browser->deform(m_operation);
    emit sigFileChanged(this);
}

bool DocSheetDJVU::getImageMax(int index, QImage &image, double max)
{
    if (m_browser)
        return m_browser->getImageMax(index, image, max);

    return false;
}

bool DocSheetDJVU::fileChanged()
{
    return !(m_initBookmarks == m_bookmarks);
}

bool DocSheetDJVU::saveData()
{
    return Database::instance()->saveBookmarks(filePath(), m_bookmarks);
}

bool DocSheetDJVU::saveAsData(QString filePath)
{
    Database::instance()->saveBookmarks(filePath, m_bookmarks);
    return QFile::copy(this->filePath(), filePath);
}

void DocSheetDJVU::openMagnifier()
{
    if (m_browser)
        m_browser->openMagnifier();
}

bool DocSheetDJVU::closeMagnifier()
{
    if (m_browser)
        m_browser->closeMagnifier();
}

void DocSheetDJVU::docBasicInfo(stFileInfo &info)
{
    info.strFilepath = filePath();

    QFileInfo fileInfo(filePath());
    info.size = fileInfo.size();
    info.CreateTime = fileInfo.birthTime();
    info.ChangeTime = fileInfo.lastModified();
    info.strAuther = fileInfo.owner();
    info.strFilepath = fileInfo.filePath();

    if (m_browser) {
        info.strFormat = QString("djvu");
//        info.boptimization = document->isLinearized();
//        info.strKeyword = document->keywords();
//        info.strTheme = document->title();
//        info.strProducter = document->producer();
//        info.strCreater = document->creator();
//        info.bsafe = document->isEncrypted();
        info.iWidth = m_browser->maxWidth();
        info.iHeight = m_browser->maxHeight();
        info.iNumpages = m_browser->allPages();
    }

}

QString DocSheetDJVU::filter()
{
    return "Djvu File (*.djvu)";
}

void DocSheetDJVU::onBrowserPageChanged(int page)
{
    if (m_operation.currentPage != page) {
        m_operation.currentPage = page;
        m_sidebar->setCurrentPage(page);
    }
}

void DocSheetDJVU::onBrowserSizeChanged()
{
    if (m_operation.scaleMode != Dr::ScaleFactorMode) {
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::onBrowserWheelUp()
{
    zoomin();
}

void DocSheetDJVU::onBrowserWheelDown()
{
    zoomout();
}

void DocSheetDJVU::onBrowserPageFirst()
{
    jumpToFirstPage();
}

void DocSheetDJVU::onBrowserPagePrev()
{
    jumpToPrevPage();
}

void DocSheetDJVU::onBrowserPageNext()
{
    jumpToNextPage();
}

void DocSheetDJVU::onBrowserPageLast()
{
    jumpToLastPage();
}

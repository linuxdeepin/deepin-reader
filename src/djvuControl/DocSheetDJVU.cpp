/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "DocSheetDJVU.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowserDJVU.h"
#include "Database.h"

#include <QDebug>
#include <QFileInfo>

DocSheetDJVU::DocSheetDJVU(QString filePath, QWidget *parent) : DocSheet(Dr::DjVu, filePath, parent)
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL | PREVIEW_BOOKMARK);
    m_sidebar->setMinimumWidth(266);

    m_browser = new SheetBrowserDJVU(this);
    m_browser->setMinimumWidth(481);

    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onBrowserPageChanged(int)));
    connect(m_browser, SIGNAL(sigSizeChanged()), this, SLOT(onBrowserSizeChanged()));
    connect(m_browser, SIGNAL(sigWheelUp()), this, SLOT(onBrowserWheelUp()));
    connect(m_browser, SIGNAL(sigWheelDown()), this, SLOT(onBrowserWheelDown()));
    connect(m_browser, SIGNAL(sigNeedPagePrev()), this, SLOT(onBrowserPagePrev()));
    connect(m_browser, SIGNAL(sigNeedPageNext()), this, SLOT(onBrowserPageNext()));
    connect(m_browser, SIGNAL(sigNeedPageFirst()), this, SLOT(onBrowserPageFirst()));
    connect(m_browser, SIGNAL(sigNeedPageLast()), this, SLOT(onBrowserPageLast()));
    connect(m_browser, SIGNAL(sigNeedBookMark(int, bool)), this, SLOT(onBrowserBookmark(int, bool)));
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
    m_sidebar->handleRotate(m_operation.rotation);
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
    m_sidebar->handleRotate(m_operation.rotation);
    emit sigFileChanged(this);
}

bool DocSheetDJVU::openFileExec()
{
    if (m_browser->openFilePath(filePath())) {
        if (!m_browser->loadPages(m_operation, m_bookmarks))
            return false;
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
    else {
        showTips(tr("The bookmark has been removed"));
        m_bookmarks.remove(index);
    }

    m_sidebar->setBookMark(index, state);
    m_browser->setBookMark(index, state);

    m_fileChanged = true;
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
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 1;

    return m_operation.currentPage;
}

int DocSheetDJVU::currentIndex()
{
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 0;

    return m_operation.currentPage - 1;
}

void DocSheetDJVU::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode == m_operation.layoutMode)
        return;

    if (mode >= 0 && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setMouseShape(Dr::MouseShape shape)
{
    if (shape >= 0 && shape < Dr::NumberOfMouseShapes) {
        closeMagnifier();
        m_operation.mouseShape = shape;
        m_browser->loadMouseShape(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setScaleMode(Dr::ScaleMode mode)
{
    if (mode == m_operation.scaleMode)
        return;

    if (mode >= 0 && mode < Dr::NumberOfScaleModes) {
        m_operation.scaleMode = mode;
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void DocSheetDJVU::setScaleFactor(qreal scaleFactor)
{
    if (Dr::ScaleFactorMode == m_operation.scaleMode && scaleFactor == m_operation.scaleFactor)
        return;

    if (scaleFactor > maxScaleFactor())
        scaleFactor = maxScaleFactor();

    m_operation.scaleMode = Dr::ScaleFactorMode;
    m_operation.scaleFactor = scaleFactor;
    m_browser->deform(m_operation);
    emit sigFileChanged(this);
}

bool DocSheetDJVU::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    if (m_browser)
        return m_browser->getImage(index, image, width, height, mode);

    return false;
}

bool DocSheetDJVU::fileChanged()
{
    return m_fileChanged;
}

bool DocSheetDJVU::saveData()
{
    if (Database::instance()->saveBookmarks(filePath(), m_bookmarks)) {
        m_fileChanged = false;
        return true;
    }
    return false;
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

void DocSheetDJVU::closeMagnifier()
{
    if (m_browser)
        m_browser->closeMagnifier();
}

void DocSheetDJVU::defaultFocus()
{
    if (m_browser)
        m_browser->setFocus();
}

bool DocSheetDJVU::magnifierOpened()
{
    if (m_browser)
        return m_browser->magnifierOpened();
    return false;
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

QList<qreal> DocSheetDJVU::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};
    QList<qreal> factorList;

    qreal maxFactor = maxScaleFactor();

    foreach (qreal factor, dataList) {
        if (maxFactor - factor > -0.0001)
            factorList.append(factor);
    }

    return  factorList;
}

qreal DocSheetDJVU::maxScaleFactor()
{
    qreal maxScaleFactor = 20000 / (m_browser->maxHeight() * qApp->devicePixelRatio());

    if (maxScaleFactor < 0.1)
        maxScaleFactor = 0.1;

    return maxScaleFactor;
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

void DocSheetDJVU::onBrowserBookmark(int index, bool state)
{
    setBookMark(index, state);
}

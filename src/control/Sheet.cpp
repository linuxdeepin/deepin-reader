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
* CentralDocPage(DocTabbar Sheets)
*
* Sheet(SheetSidebar SheetBrowser document)
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
#include "Sheet.h"
#include "lpreviewControl/SheetSidebar.h"
#include "SheetBrowser.h"
#include "Database.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>

DWIDGET_USE_NAMESPACE

QMap<QString, Sheet *> Sheet::g_map;
//bool isBlockShutdown = false;
//QDBusInterface *blockShutdownInterface = nullptr;
//QDBusReply<QDBusUnixFileDescriptor> blockShutdownReply;
Sheet::Sheet(Dr::FileType type, QString filePath,  QWidget *parent)
    : DSplitter(parent), m_filePath(filePath), m_type(type)
{
    m_uuid = QUuid::createUuid().toString();
    g_map[m_uuid] = this;

    Database::instance()->readOperation(this);
    Database::instance()->readBookmarks(m_filePath, m_bookmarks);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

//    m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
//    m_sidebar->setMinimumWidth(266);

    m_browser = new SheetBrowser(this);
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

Sheet::~Sheet()
{
    Database::instance()->saveOperation(this);
    g_map.remove(m_uuid);
}

bool Sheet::existFileChanged()
{

}

QUuid Sheet::getUuid(Sheet *)
{

}

Sheet *Sheet::getSheet(QString uuid)
{

}

void Sheet::blockShutdown()
{

}

void Sheet::unBlockShutdown()
{

}

void Sheet::initOperationData(const SheetOperation &opera)
{
    m_operation = opera;
}

bool Sheet::isOpen()
{
    //...
    return false;
}

void Sheet::openFile()
{

}

void Sheet::jumpToPage(int page)
{
    m_browser->setCurrentPage(page);
}

void Sheet::jumpToIndex(int index)
{
    m_browser->setCurrentPage(index + 1);
}

void Sheet::jumpToFirstPage()
{
    m_browser->setCurrentPage(1);
}

void Sheet::jumpToLastPage()
{
    jumpToPage(m_browser->allPages());
}

void Sheet::jumpToNextPage()
{
    int page = m_browser->currentPage() + (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page >= m_browser->allPages() ? m_browser->allPages() : page;

    jumpToPage(page);
}

void Sheet::jumpToPrevPage()
{
    int page = m_browser->currentPage() - (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page < 1 ? 1 : page;

    jumpToPage(page);
}

void Sheet::rotateLeft()
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

void Sheet::rotateRight()
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

bool Sheet::openFileExec()
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

void Sheet::setBookMark(int index, int state)
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

int Sheet::pagesNumber()
{
    if (m_browser)
        return m_browser->allPages();

    return 0;
}

int Sheet::currentPage()
{
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 1;

    return m_operation.currentPage;
}

int Sheet::currentIndex()
{
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 0;

    return m_operation.currentPage - 1;
}

void Sheet::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode == m_operation.layoutMode)
        return;

    if (mode >= Dr::SinglePageMode && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void Sheet::setMouseShape(Dr::MouseShape shape)
{
    if (shape >= Dr::MouseShapeNormal && shape < Dr::NumberOfMouseShapes) {
        closeMagnifier();
        m_operation.mouseShape = shape;
        m_browser->setMouseShape(m_operation.mouseShape);
        emit sigFileChanged(this);
    }
}

void Sheet::setScaleMode(Dr::ScaleMode mode)
{
    if (mode >= Dr::ScaleFactorMode && mode <= Dr::FitToPageWorHMode) {
        m_operation.scaleMode = mode;
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void Sheet::setScaleFactor(qreal scaleFactor)
{
    if (Dr::ScaleFactorMode == m_operation.scaleMode && qFuzzyCompare(scaleFactor, m_operation.scaleFactor))
        return;

    if (scaleFactor > maxScaleFactor())
        scaleFactor = maxScaleFactor();

    m_operation.scaleMode = Dr::ScaleFactorMode;
    m_operation.scaleFactor = scaleFactor;
    m_browser->deform(m_operation);
    emit sigFileChanged(this);
}

bool Sheet::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    if (m_browser)
        return m_browser->getImage(index, image, width, height, mode);

    return false;
}

bool Sheet::fileChanged()
{
    return m_fileChanged;
}

bool Sheet::saveData()
{
    if (Database::instance()->saveBookmarks(filePath(), m_bookmarks)) {
        m_fileChanged = false;
        return true;
    }
    return false;
}

bool Sheet::saveAsData(QString filePath)
{
    Database::instance()->saveBookmarks(filePath, m_bookmarks);
    return Utils::copyFile(this->filePath(), filePath);
}

void Sheet::handleSearch()
{

}

void Sheet::stopSearch()
{

}

void Sheet::copySelectedText()
{

}

void Sheet::highlightSelectedText()
{

}

void Sheet::addSelectedTextHightlightAnnotation()
{

}

void Sheet::openMagnifier()
{
    if (m_browser)
        m_browser->openMagnifier();
}

void Sheet::closeMagnifier()
{
    if (m_browser)
        m_browser->closeMagnifier();
}

void Sheet::defaultFocus()
{
    if (m_browser)
        m_browser->setFocus();
}

bool Sheet::magnifierOpened()
{
    if (m_browser)
        return m_browser->magnifierOpened();
    return false;
}

//void Sheet::docBasicInfo(stFileInfo &info)
//{
//    info.strFilepath = filePath();

//    QFileInfo fileInfo(filePath());
//    info.size = fileInfo.size();
//    info.CreateTime = fileInfo.birthTime();
//    info.ChangeTime = fileInfo.lastModified();
//    info.strAuther = fileInfo.owner();
//    info.strFilepath = fileInfo.filePath();

//    if (m_browser) {
//        info.strFormat = QString("PDFL");
////        info.boptimization = document->isLinearized();
////        info.strKeyword = document->keywords();
////        info.strTheme = document->title();
////        info.strProducter = document->producer();
////        info.strCreater = document->creator();
////        info.bsafe = document->isEncrypted();
//        info.iWidth = static_cast<unsigned int>(m_browser->maxWidth());
//        info.iHeight = static_cast<unsigned int>(m_browser->maxHeight());
//        info.iNumpages = static_cast<unsigned int>(m_browser->allPages());
//    }

//}

QList<deepin_reader::Annotation *> Sheet::annotations()
{
    if (nullptr == m_browser)
        return QList< deepin_reader::Annotation * > ();
    return m_browser->annotations();
}

bool Sheet::removeAnnotation(deepin_reader::Annotation *annotation)
{
    if (nullptr == m_browser)
        return false;
    return m_browser->removeAnnotation(annotation);
}

QList<qreal> Sheet::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};
    QList<qreal> factorList;

    qreal maxFactor = maxScaleFactor();

    foreach (qreal factor, dataList) {
        if (maxFactor - factor > 0.001)
            factorList.append(factor);
    }

    return  factorList;
}

qreal Sheet::maxScaleFactor()
{
    qreal maxScaleFactor = 20000 / (m_browser->maxHeight() * qApp->devicePixelRatio());

    if (maxScaleFactor < 0.1)
        maxScaleFactor = 0.1;

    return maxScaleFactor;
}

SheetOperation Sheet::operation()
{
    return m_operation;
}

Dr::FileType Sheet::type()
{
    return m_type;
}

QString Sheet::filePath()
{
    return QString();
}

bool Sheet::hasBookMark(int index)
{
    return false;
}

void Sheet::zoomin()
{

}

void Sheet::zoomout()
{

}

void Sheet::setSidebarVisible(bool isVisible)
{

}

void Sheet::handleOpenSuccess()
{

}

void Sheet::showTips(const QString &tips, int iconIndex)
{

}

void Sheet::openSlide()
{

}

void Sheet::closeSlide()
{

}

bool Sheet::slideOpened()
{
    return false;
}

void Sheet::openFullScreen()
{

}

void Sheet::handleSlideKeyPressEvent(const QString &sKey)
{

}

void Sheet::onBrowserPageChanged(int page)
{
    if (m_operation.currentPage != page) {
        m_operation.currentPage = page;
        m_sidebar->setCurrentPage(page);
    }
}

void Sheet::onBrowserSizeChanged()
{
    if (m_operation.scaleMode != Dr::ScaleFactorMode) {
        m_browser->deform(m_operation);
        emit sigFileChanged(this);
    }
}

void Sheet::onBrowserWheelUp()
{
    zoomin();
}

void Sheet::onBrowserWheelDown()
{
    zoomout();
}

void Sheet::onBrowserPageFirst()
{
    jumpToFirstPage();
}

void Sheet::onBrowserPagePrev()
{
    jumpToPrevPage();
}

void Sheet::onBrowserPageNext()
{
    jumpToNextPage();
}

void Sheet::onBrowserPageLast()
{
    jumpToLastPage();
}

void Sheet::onBrowserBookmark(int index, bool state)
{
    setBookMark(index, state);
}

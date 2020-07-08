/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#include "DocSheetOldOld.h"
#include "pdfControl/docview/CommonStruct.h"
#include "lpreviewControl/SheetSidebar.h"
#include "Database.h"
#include "widgets/SpinnerWidget.h"
#include "pdfControl/SheetBrowserPDF.h"
#include "CentralDocPage.h"
#include "app/ProcessController.h"
#include "pdfControl/docview/DocummentProxy.h"
#include "widgets/FindWidget.h"
#include "djvuControl/SheetBrowserDJVU.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>

DWIDGET_USE_NAMESPACE

QMap<QString, DocSheetOld *> DocSheetOld::g_map;
DocSheetOld::DocSheetOld(Dr::FileType type, QString filePath, DWidget *parent)
    : DSplitter(parent), m_filePath(filePath), m_type(type)
{
    m_uuid = QUuid::createUuid().toString();
    g_map[m_uuid] = this;

    if (type == Dr::PDF) {
        dApp->m_pDBService->qSelectData(m_filePath, DB_BOOKMARK);
        dApp->m_pDBService->qSelectData(m_filePath, DB_HISTROY);
        m_bookmarks = dApp->m_pDBService->getBookMarkList(m_filePath).toSet();
    } else {
        Database::instance()->readOperation(this);
        Database::instance()->readBookmarks(m_filePath, m_bookmarks);
    }
}

DocSheetOld::~DocSheetOld()
{
    Database::instance()->saveOperation(this);
    g_map.remove(m_uuid);
}

void DocSheetOld::initOperationData(const DocOperation &opera)
{
    m_operation = opera;
}

void DocSheetOld::openFile()
{
    qDebug() << "unrealized";
}

bool DocSheetOld::openFileExec()
{
    qDebug() << "unrealized";
    return false;
}

void DocSheetOld::jumpToIndex(int)
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToPage(int)
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToFirstPage()
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToLastPage()
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToNextPage()
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToPrevPage()
{
    qDebug() << "unrealized";
}

void DocSheetOld::rotateLeft()
{
    qDebug() << "unrealized";
}

void DocSheetOld::rotateRight()
{
    qDebug() << "unrealized";
}

void DocSheetOld::setLayoutMode(Dr::LayoutMode)
{
    qDebug() << "unrealized";
}

void DocSheetOld::setMouseShape(Dr::MouseShape)
{
    qDebug() << "unrealized";
}

void DocSheetOld::openMagnifier()
{
    qDebug() << "unrealized";
}

void DocSheetOld::closeMagnifier()
{
    qDebug() << "unrealized";
}

bool DocSheetOld::magnifierOpened()
{
    qDebug() << "unrealized";
    return  false;
}

void DocSheetOld::setScaleFactor(qreal)
{
    qDebug() << "unrealized";
}

void DocSheetOld::setScaleMode(Dr::ScaleMode)
{
    qDebug() << "unrealized";
}

void DocSheetOld::setBookMark(int, int)
{
    qDebug() << "unrealized";
}

void DocSheetOld::setBookMarks(const QList<BookMarkStatus_t> &)
{
    qDebug() << "unrealized";
}

void DocSheetOld::copySelectedText()
{
    qDebug() << "unrealized";
}

void DocSheetOld::highlightSelectedText()
{
    qDebug() << "unrealized";
}

void DocSheetOld::addSelectedTextHightlightAnnotation()
{
    qDebug() << "unrealized";
}

int DocSheetOld::pagesNumber()
{
    qDebug() << "unrealized";
    return 0;
}

int DocSheetOld::currentPage()
{
    qDebug() << "unrealized";
    return -1;
}

int DocSheetOld::currentIndex()
{
    qDebug() << "unrealized";
    return -1;
}

bool DocSheetOld::getImage(int, QImage &, double, double, Qt::AspectRatioMode)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheetOld::docBasicInfo(stFileInfo &)
{
    qDebug() << "unrealized";
}

void DocSheetOld::getAllAnnotation(QList<stHighlightContent> &)
{
    qDebug() << "unrealized";
}

Outline DocSheetOld::outline()
{
    qDebug() << "unrealized";
    return Outline();
}

void DocSheetOld::jumpToOutline(const qreal &, const qreal &, unsigned int)
{
    qDebug() << "unrealized";
}

bool DocSheetOld::isOpen()
{
    qDebug() << "unrealized";
    return false;
}

QString DocSheetOld::addIconAnnotation(const QPoint &, const QColor &, TextAnnoteType_Em)
{
    qDebug() << "unrealized";
    return "";
}

void DocSheetOld::deleteAnnotation(const int &, const QString &)
{
    qDebug() << "unrealized";
}

void DocSheetOld::deleteAnnotations(const QList<AnnotationInfo_t> &)
{
    qDebug() << "unrealized";
}

void DocSheetOld::jumpToHighLight(const QString &, int)
{
    qDebug() << "unrealized";
}

QString DocSheetOld::pagenum2label(int)
{
    qDebug() << "unrealized";
    return QString();
}

bool DocSheetOld::haslabel()
{
    return false;
}

int DocSheetOld::label2pagenum(QString)
{
    qDebug() << "unrealized";
    return -1;
}

QString DocSheetOld::filter()
{
    qDebug() << "unrealized";
    return "";
}

void DocSheetOld::defaultFocus()
{
    m_sidebar->setFocus();
}

bool DocSheetOld::fileChanged()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheetOld::saveData()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheetOld::saveAsData(QString)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheetOld::handleSearch()
{
    qDebug() << "unrealized";
}

void DocSheetOld::stopSearch()
{

}

QUuid DocSheetOld::getUuid(DocSheetOld *sheet)
{
    return g_map.key(sheet);
}

DocSheetOld *DocSheetOld::getSheet(QString uuid)
{
    if (g_map.contains(uuid))
        return g_map[uuid];

    return nullptr;
}

bool DocSheetOld::hasBookMark(int index)
{
    return m_bookmarks.contains(index);
}

void DocSheetOld::handleOpenSuccess()
{
    if (m_sidebar)
        m_sidebar->handleOpenSuccess();
}

void DocSheetOld::setSidebarVisible(bool isVisible)
{
    m_operation.sidebarVisible = isVisible;
    if (m_sidebar)
        m_sidebar->setVisible(isVisible);
    emit sigFileChanged(this);
}

QString DocSheetOld::filePath()
{
    return m_filePath;
}

QList<qreal> DocSheetOld::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};

    return  dataList;
}

qreal DocSheetOld::maxScaleFactor()
{
    qDebug() << "unrealized";
    return 1;
}

void DocSheetOld::setActive(const bool &)
{

}

QList<deepin_reader::Annotation *> DocSheetOld::annotations()
{
    return QList< deepin_reader::Annotation * > ();
}

void DocSheetOld::zoomin()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheetOld::zoomout()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = dataList.count() - 1; i >= 0; --i) {
        if (dataList[i] < (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

Dr::FileType DocSheetOld::type()
{
    return m_type;
}

void DocSheetOld::showTips(const QString &tips, int iconIndex)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showTips(tips, iconIndex);
}

bool DocSheetOld::existFileChanged()
{
    foreach (DocSheetOld *sheet, g_map.values()) {
        if (sheet->fileChanged())
            return true;
    }

    return false;
}

DocOperation DocSheetOld::operation()
{
    return m_operation;
}

void DocSheetOld::openSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->openSlide();
}

void DocSheetOld::closeSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitSlide();
}

void DocSheetOld::openFullScreen()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->openFullScreen();
}

QSet<int> DocSheetOld::getBookMarkList() const
{
    return m_bookmarks;
}

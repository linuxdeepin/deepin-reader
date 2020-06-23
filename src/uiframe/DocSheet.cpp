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
#include "DocSheet.h"
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

QMap<QString, DocSheet *> DocSheet::g_map;
DocSheet::DocSheet(Dr::FileType type, QString filePath, DWidget *parent)
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

DocSheet::~DocSheet()
{
    Database::instance()->saveOperation(this);
    g_map.remove(m_uuid);
}

void DocSheet::initOperationData(const DocOperation &opera)
{
    m_operation = opera;
}

void DocSheet::openFile()
{
    qDebug() << "unrealized";
}

bool DocSheet::openFileExec()
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::jumpToIndex(int)
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToPage(int)
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToFirstPage()
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToLastPage()
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToNextPage()
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToPrevPage()
{
    qDebug() << "unrealized";
}

void DocSheet::rotateLeft()
{
    qDebug() << "unrealized";
}

void DocSheet::rotateRight()
{
    qDebug() << "unrealized";
}

void DocSheet::setLayoutMode(Dr::LayoutMode)
{
    qDebug() << "unrealized";
}

void DocSheet::setMouseShape(Dr::MouseShape)
{
    qDebug() << "unrealized";
}

void DocSheet::openMagnifier()
{
    qDebug() << "unrealized";
}

void DocSheet::closeMagnifier()
{
    qDebug() << "unrealized";
}

bool DocSheet::magnifierOpened()
{
    qDebug() << "unrealized";
    return  false;
}

void DocSheet::setScaleFactor(qreal)
{
    qDebug() << "unrealized";
}

void DocSheet::setScaleMode(Dr::ScaleMode)
{
    qDebug() << "unrealized";
}

void DocSheet::setBookMark(int, int)
{
    qDebug() << "unrealized";
}

void DocSheet::setBookMarks(const QList<BookMarkStatus_t> &)
{
    qDebug() << "unrealized";
}

void DocSheet::copySelectedText()
{
    qDebug() << "unrealized";
}

void DocSheet::highlightSelectedText()
{
    qDebug() << "unrealized";
}

void DocSheet::addSelectedTextHightlightAnnotation()
{
    qDebug() << "unrealized";
}

int DocSheet::pagesNumber()
{
    qDebug() << "unrealized";
    return 0;
}

int DocSheet::currentPage()
{
    qDebug() << "unrealized";
    return -1;
}

int DocSheet::currentIndex()
{
    qDebug() << "unrealized";
    return -1;
}

bool DocSheet::getImage(int, QImage &, double, double, Qt::AspectRatioMode)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::docBasicInfo(stFileInfo &)
{
    qDebug() << "unrealized";
}

void DocSheet::getAllAnnotation(QList<stHighlightContent> &)
{
    qDebug() << "unrealized";
}

Outline DocSheet::outline()
{
    qDebug() << "unrealized";
    return Outline();
}

void DocSheet::jumpToOutline(const qreal &, const qreal &, unsigned int)
{
    qDebug() << "unrealized";
}

bool DocSheet::isOpen()
{
    qDebug() << "unrealized";
    return false;
}

QString DocSheet::addIconAnnotation(const QPoint &, const QColor &, TextAnnoteType_Em)
{
    qDebug() << "unrealized";
    return "";
}

void DocSheet::deleteAnnotation(const int &, const QString &)
{
    qDebug() << "unrealized";
}

void DocSheet::deleteAnnotations(const QList<AnnotationInfo_t> &)
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToHighLight(const QString &, int)
{
    qDebug() << "unrealized";
}

QString DocSheet::pagenum2label(int)
{
    qDebug() << "unrealized";
    return QString();
}

bool DocSheet::haslabel()
{
    return false;
}

int DocSheet::label2pagenum(QString)
{
    qDebug() << "unrealized";
    return -1;
}

QString DocSheet::filter()
{
    qDebug() << "unrealized";
    return "";
}

void DocSheet::defaultFocus()
{
    m_sidebar->setFocus();
}

bool DocSheet::fileChanged()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::saveData()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::saveAsData(QString)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::handleSearch()
{
    qDebug() << "unrealized";
}

QUuid DocSheet::getUuid(DocSheet *sheet)
{
    return g_map.key(sheet);
}

DocSheet *DocSheet::getSheet(QString uuid)
{
    if (g_map.contains(uuid))
        return g_map[uuid];

    return nullptr;
}

bool DocSheet::hasBookMark(int index)
{
    return m_bookmarks.contains(index);
}

void DocSheet::handleOpenSuccess()
{
    if (m_sidebar)
        m_sidebar->handleOpenSuccess();
}

void DocSheet::setSidebarVisible(bool isVisible)
{
    m_operation.sidebarVisible = isVisible;
    if (m_sidebar)
        m_sidebar->setVisible(isVisible);
    emit sigFileChanged(this);
}

QString DocSheet::filePath()
{
    return m_filePath;
}

QList<qreal> DocSheet::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};

    return  dataList;
}

qreal DocSheet::maxScaleFactor()
{
    qDebug() << "unrealized";
    return 1;
}

void DocSheet::zoomin()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheet::zoomout()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = dataList.count() - 1; i >= 0; --i) {
        if (dataList[i] < (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

Dr::FileType DocSheet::type()
{
    return m_type;
}

void DocSheet::showTips(const QString &tips, int iconIndex)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showTips(tips, iconIndex);
}

bool DocSheet::existFileChanged()
{
    foreach (DocSheet *sheet, g_map.values()) {
        if (sheet->fileChanged())
            return true;
    }

    return false;
}

DocOperation DocSheet::operation()
{
    return m_operation;
}

void DocSheet::openSlide()
{

}

void DocSheet::closeSlide()
{

}

QSet<int> DocSheet::getBookMarkList() const
{
    return m_bookmarks;
}

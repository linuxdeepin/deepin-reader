/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "pdfControl/docview/commonstruct.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>

#include "lpreviewControl/SheetSidebar.h"
#include "database.h"
#include "widgets/SpinnerWidget.h"
#include "pdfControl/SheetBrowserPDF.h"
#include "CentralDocPage.h"
#include "app/ProcessController.h"
#include "pdfControl/docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "djvuControl/SheetBrowserDJVU.h"

DWIDGET_USE_NAMESPACE

QMap<QString, DocSheet *> DocSheet::g_map;
DocSheet::DocSheet(Dr::FileType type, QString filePath, DWidget *parent)
    : DSplitter(parent), m_type(type), m_filePath(filePath)
{
    m_uuid = QUuid::createUuid().toString();
    g_map[m_uuid] = this;
    Database::instance()->readOperation(this);
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

void DocSheet::jumpToIndex(int index)
{
    qDebug() << "unrealized";
}

void DocSheet::jumpToPage(int page)
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

void DocSheet::setFileChanged(bool hasChanged)
{
    qDebug() << "unrealized";
}

void DocSheet::setLayoutMode(Dr::LayoutMode mode)
{
    m_operation.layoutMode = mode;
}

void DocSheet::setMouseShape(Dr::MouseShape shape)
{
    m_operation.mouseShape = shape;
}

void DocSheet::setCurrentPage(int page)
{
    m_operation.currentPage = page;
}

void DocSheet::openMagnifier()
{
    qDebug() << "unrealized";
}

void DocSheet::setScaleFactor(qreal scaleFactor)
{
    m_operation.scaleFactor = scaleFactor;
}

void DocSheet::setSidebarIndex(int index)
{
    m_operation.sidebarIndex = index;
}

void DocSheet::setRotation(Dr::Rotation rotation)
{
    m_operation.rotation = rotation;
}

void DocSheet::setScaleMode(Dr::ScaleMode mode)
{
    m_operation.scaleMode = mode;
}

void DocSheet::setBookMark(int page, int state)
{
    qDebug() << "unrealized";
}

void DocSheet::showNoteWidget(int page, const QString &uuid, const int &type)
{
    qDebug() << "unrealized";
}

bool DocSheet::isDoubleShow()
{
    qDebug() << "unrealized";
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

void DocSheet::print()
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

bool DocSheet::getImage(int, QImage &, double, double)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::docBasicInfo(stFileInfo &)
{
    qDebug() << "unrealized";
}

bool DocSheet::getAutoPlaySlideStatu()
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::setAutoPlaySlide(bool, int)
{
    qDebug() << "unrealized";
}

double DocSheet::getMaxZoomratio()
{
    qDebug() << "unrealized";
    return 0;
}

void DocSheet::getAllAnnotation(QList<stHighlightContent> &)
{
    qDebug() << "unrealized";
}

bool DocSheet::getImageMax(int, QImage &, double)
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::getImagePoint(int pagenum, QImage &image, double scaleFactor, QPoint point)
{
    qDebug() << "unrealized";
    return false;
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

int  DocSheet::pointInWhichPage(QPoint)
{
    qDebug() << "unrealized";
    return -1;
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
    qDebug() << "unrealized";
    return false;
}

int DocSheet::label2pagenum(QString label)
{
    qDebug() << "unrealized";
    return -1;
}

bool DocSheet::closeMagnifier()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::showSlideModel()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::exitSlideModel()
{
    qDebug() << "unrealized";
    return false;
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

QString DocSheet::filePath()
{
    return m_filePath;
}

QList<qreal> DocSheet::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};

    return  dataList;
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

QString DocSheet::filter()
{
    qDebug() << "unrealized";
    return "";
}

bool DocSheet::fileChanged()
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::saveOper()
{
    qDebug() << "unrealized";
}

bool DocSheet::saveData()
{
    qDebug() << "unrealized";
    return false;
}

bool DocSheet::saveAsData(QString filePath)
{
    qDebug() << "unrealized";
    return false;
}

void DocSheet::exitSliderShow()
{
    qDebug() << "unrealized";
}

void DocSheet::handleSearch()
{
    qDebug() << "unrealized";
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

void DocSheet::setCurrentState(int state)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    return doc->setCurrentState(state);
}

int DocSheet::currentState()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return Default_State;

    return doc->getCurrentState();
}

void DocSheet::showSlideControl()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showPlayControlWidget();
}

bool DocSheet::existFileChanged()
{
    foreach (DocSheet *sheet, g_map.values()) {
        if (sheet->fileChanged())
            return true;
    }

    return false;
}

void DocSheet::quitSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitSlide();
}

void DocSheet::quitMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitMagnifer();
}

DocOperation DocSheet::operation()
{
    return m_operation;
}

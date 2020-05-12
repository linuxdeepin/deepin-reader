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

void DocSheet::openFile()
{

}

bool DocSheet::openFileExec()
{
    return false;
}

void DocSheet::pageJump(int page)
{

}

void DocSheet::pageFirst()
{

}

void DocSheet::pageLast()
{

}

void DocSheet::pageNext()
{

}

void DocSheet::pagePrev()
{

}

void DocSheet::zoomin()
{
    qDebug() << "DocSheet::zoomin()";
}

void DocSheet::zoomout()
{
    qDebug() << "DocSheet::zoomout()";
}

void DocSheet::setDoubleShow(bool isShow)
{

}

void DocSheet::setRotateLeft()
{

}

void DocSheet::setRotateRight()
{

}

void DocSheet::setFileChanged(bool hasChanged)
{

}

void DocSheet::setMouseDefault()
{

}

void DocSheet::setMouseHand()
{

}

void DocSheet::setMouseShape(Dr::MouseShape shape)
{

}

void DocSheet::setScale(double scale)
{

}

void DocSheet::setScaleFactor(qreal scaleFactor)
{

}

void DocSheet::setFit(int fit)
{

}

void DocSheet::setScaleMode(Dr::ScaleMode mode)
{

}

void DocSheet::setBookMark(int page, int state)
{

}

void DocSheet::showNoteWidget(int page, const QString &uuid, const int &type)
{

}

bool DocSheet::isMouseHand()
{

}

bool DocSheet::isDoubleShow()
{

}

void DocSheet::handleOpenSuccess()
{

}

void DocSheet::setSidebarVisible(bool isVisible)
{

}

void DocSheet::copySelectedText()
{

}

void DocSheet::highlightSelectedText()
{

}

void DocSheet::addSelectedTextHightlightAnnotation()
{

}

void DocSheet::openSideBar()
{

}

void DocSheet::print()
{

}

QList<qreal> DocSheet::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};

    return  dataList;
}

int DocSheet::getPageSNum()
{
    return -1;
}

int DocSheet::currentPageNo()
{
    return -1;
}

bool DocSheet::getImage(int, QImage &, double, double)
{
    return false;
}

void DocSheet::docBasicInfo(stFileInfo &)
{

}

bool DocSheet::getAutoPlaySlideStatu()
{
    return false;
}

void DocSheet::setAutoPlaySlide(bool, int)
{

}

double DocSheet::getMaxZoomratio()
{
    return 0;
}

void DocSheet::getAllAnnotation(QList<stHighlightContent> &)
{

}

bool DocSheet::getImageMax(int, QImage &, double)
{
    return false;
}

bool DocSheet::setBookMarkState(int, bool)
{
    return false;
}

Outline DocSheet::outline()
{
    return Outline();
}

void DocSheet::jumpToOutline(const qreal &, const qreal &, unsigned int)
{

}

bool DocSheet::isOpen()
{
    return false;
}

QString DocSheet::addIconAnnotation(const QPoint &, const QColor &, TextAnnoteType_Em)
{
    return "";
}

int  DocSheet::pointInWhichPage(QPoint)
{
    return -1;
}

void DocSheet::jumpToHighLight(const QString &, int)
{

}

QString DocSheet::pagenum2label(int)
{
    return QString();
}

bool DocSheet::haslabel()
{
    return false;
}

int DocSheet::label2pagenum(QString label)
{
    return -1;
}

bool DocSheet::closeMagnifier()
{
    return false;
}

bool DocSheet::showSlideModel()
{
    return false;
}

bool DocSheet::exitSlideModel()
{
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

QString DocSheet::filter()
{
    return "";
}

bool DocSheet::getFileChanged()
{
    return false;
}

void DocSheet::saveOper()
{

}

bool DocSheet::saveData()
{
    return false;
}

bool DocSheet::saveAsData(QString filePath)
{
    return false;
}

void DocSheet::setOper(const int &, const QVariant &)
{

}

QVariant DocSheet::getOper(int type)
{
    return -1;
}

void DocSheet::OnOpenSliderShow()
{

}

void DocSheet::exitSliderShow()
{

}

void DocSheet::handleSearch()
{

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

int DocSheet::getCurrentState()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return Default_State;

    return doc->getCurrentState();
}

void DocSheet::showControl()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showPlayControlWidget();
}

void DocSheet::quitSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitSlide();
}

void DocSheet::openMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->openMagnifer();
}

void DocSheet::quitMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitMagnifer();
}

DocOperation &DocSheet::operation()
{
    return m_operation;
}

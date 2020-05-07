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
#include "pdfControl/SheetSidebarPDF.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>

#include "widgets/SpinnerWidget.h"
#include "pdfControl/SheetBrowserPDF.h"
#include "CentralDocPage.h"
#include "app/ProcessController.h"
#include "pdfControl/docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "djvu/SheetBrowserDJVU.h"

DWIDGET_USE_NAMESPACE

QMap<QString, DocSheet *> DocSheet::g_map;
DocSheet::DocSheet(Dr::FileType type, DWidget *parent)
    : DSplitter(parent), m_type(type)
{
    m_uuid = QUuid::createUuid().toString();
    g_map[m_uuid] = this;
}

DocSheet::~DocSheet()
{
    g_map.remove(m_uuid);
}

void DocSheet::handleShortcut(QString shortcut)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->handleShortcut(shortcut);
}

void DocSheet::openFile(const QString &filePath)
{

}

bool DocSheet::openFileExec(const QString &filePath)
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

}

void DocSheet::zoomout()
{

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

void DocSheet::setScale(double scale)
{

}

void DocSheet::setFit(int fit)
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

void DocSheet::onTitleShortCut(QString shortCut)
{

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
    return "";
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

DocummentProxy *DocSheet::getDocProxy()
{
    return nullptr;
}

void DocSheet::OnOpenSliderShow()
{

}

void DocSheet::exitSliderShow()
{

}

void DocSheet::ShowFindWidget()
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

void DocSheet::openSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnOpenSliderShow();
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

    doc->OnExitSliderShow();
}

void DocSheet::openMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnOpenMagnifer();
}

void DocSheet::quitMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnExitMagnifer();
}

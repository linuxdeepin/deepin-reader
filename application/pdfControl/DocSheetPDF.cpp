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
#include "DocSheetPDF.h"
#include "docview/commonstruct.h"
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
#include "docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "djvu/SheetBrowserDJVU.h"

DocSheetPDF::DocSheetPDF(DWidget *parent)
    : DocSheet(Dr::PDF, parent)
{
    m_pRightWidget = new QStackedWidget(this);
    m_pSpinnerWidget = new SpinnerWidget(this);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    SheetBrowserPDF *browser = new SheetBrowserPDF(this, this);
    SheetSidebarPDF *sidebar = new SheetSidebarPDF(this);

    m_sidebar = sidebar;
    m_browser = browser;

    connect(sidebar, SIGNAL(sigDeleteAnntation(const int &, const QString &)), browser, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    connect(browser, SIGNAL(sigPageChanged(int)), sidebar, SLOT(onPageChanged(int)));
    connect(browser, SIGNAL(sigFileOpenResult(const QString &, const bool &)), this, SLOT(SlotFileOpenResult(const QString &, const bool &)));
    connect(browser, SIGNAL(sigAnntationMsg(const int &, const QString &)), this, SLOT(onAnntationMsg(int, QString)));
    connect(browser, SIGNAL(sigFileChanged()), this, SLOT(onFileChanged()));
    connect(browser, SIGNAL(sigRotateChanged(int)), this, SLOT(onRotate(int)));
    connect(browser, SIGNAL(sigFindContantComming(const stSearchRes &)), this, SLOT(onFindContentComming(const stSearchRes &)));
    connect(browser, SIGNAL(sigFindFinished()), this, SLOT(onFindFinished()));

    int tW = 36;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pSpinnerWidget->setSpinnerSize(QSize(tW, tH));
    m_pSpinnerWidget->startSpinner();

    m_pRightWidget->addWidget(m_pSpinnerWidget);
    m_pRightWidget->addWidget(browser);

    addWidget(sidebar);
    addWidget(m_pRightWidget);

    QList<int> list_src;
    tW = LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);
    tW = 1000 - LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);

    setSizes(list_src);

    setAcceptDrops(true);

//    //文档刚打开时，模拟鼠标点击文档区域事件
//    QPoint pos(m_pRightWidget->geometry().x() + 10, m_pRightWidget->geometry().y() + 10);
//    QMouseEvent event0(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    //    QApplication::sendEvent(m_pRightWidget, &event0);

    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved(int, int)));
}

DocSheetPDF::~DocSheetPDF()
{

}

void DocSheetPDF::handleShortcut(QString shortcut)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->handleShortcut(shortcut);
}

void DocSheetPDF::openFile(const QString &filePath)
{

    m_browser->OpenFilePath(filePath);
}

void DocSheetPDF::pageJump(int page)
{
    DocummentProxy *_proxy =  m_browser->GetDocProxy();
    if (_proxy) {
        int nPageSize = _proxy->getPageSNum();      //  总页数
        if (page < 0 || page == nPageSize) {
            return;
        }

        int nCurPage = _proxy->currentPageNo();
        if (nCurPage != page) {
            _proxy->pageJump(page);
        }
    }
}

void DocSheetPDF::pageFirst()
{
    pageJump(0);
}

void DocSheetPDF::pageLast()
{
    pageJump(getDocProxy()->getPageSNum() - 1);
}

void DocSheetPDF::pageNext()
{
    bool isDoubleShow = m_browser->isDoubleShow();

    int nCurPage = getDocProxy()->currentPageNo();

    int page = nCurPage + (isDoubleShow ? 2 : 1);

    page = (page >= (getDocProxy()->getPageSNum() - 1) ? (getDocProxy()->getPageSNum() - 1) : page);

    pageJump(page);
}

void DocSheetPDF::pagePrev()
{
    bool isDoubleShow = m_browser->isDoubleShow();

    int nCurPage = getDocProxy()->currentPageNo();

    int page = nCurPage - (isDoubleShow ? 2 : 1);

    pageJump(page);
}

void DocSheetPDF::zoomin()
{
    QList<int> dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > getOper(Scale).toDouble()) {
            setScale(dataList[i]);
            emit sigFileChanged(this);
            return;
        }
    }
}

void DocSheetPDF::zoomout()
{
    QList<int> dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};

    for (int i = dataList.count() - 1; i > 0; --i) {
        if (dataList[i] < getOper(Scale).toInt()) {
            setScale(dataList[i]);
            emit sigFileChanged(this);
            return;
        }
    }
}

void DocSheetPDF::setDoubleShow(bool isShow)
{

    m_browser->setDoubleShow(isShow);
}

void DocSheetPDF::setRotateLeft()
{

    m_browser->setRotateLeft();
}

void DocSheetPDF::setRotateRight()
{

    m_browser->setRotateRight();
}

void DocSheetPDF::setFileChanged(bool hasChanged)
{

    m_browser->setFileChanged(hasChanged);
}

void DocSheetPDF::setMouseDefault()
{

    m_browser->setMouseDefault();
}

void DocSheetPDF::setMouseHand()
{

    m_browser->setMouseHand();
}

void DocSheetPDF::setScale(double scale)
{

    m_browser->setScale(scale);
}

void DocSheetPDF::setFit(int fit)
{

    m_browser->setFit(fit);
}

void DocSheetPDF::setBookMark(int page, int state)
{
    m_sidebar->setBookMark(page, state);
    m_browser->setBookMark(page, state);
}

void DocSheetPDF::showNoteWidget(int page, const QString &uuid, const int &type)
{
    m_browser->showNoteWidget(page, uuid, type);
}

bool DocSheetPDF::isMouseHand()
{
    return m_browser->isMouseHand();
}

bool DocSheetPDF::isDoubleShow()
{

    return m_browser->isDoubleShow();
}

void DocSheetPDF::SlotFileOpenResult(const QString &s, const bool &res)
{
    if (res) {
        if (m_pRightWidget && m_pSpinnerWidget) {
            m_pRightWidget->removeWidget(m_pSpinnerWidget);

            delete  m_pSpinnerWidget;
            m_pSpinnerWidget = nullptr;
        }

        handleOpenSuccess();
    }

    emit sigOpened(this, res);
    emit sigOpenFileResult(s, res);
}

void DocSheetPDF::handleOpenSuccess()
{
    m_sidebar->handleOpenSuccess();
}

void DocSheetPDF::setSidebarVisible(bool isVisible)
{
    m_sidebar->setVisible(isVisible);
}

void DocSheetPDF::onShowTips(const QString &tips, int index)
{
    showTips(tips, index);
}

void DocSheetPDF::onFileChanged()
{
    emit sigFileChanged(this);
}

void DocSheetPDF::onSplitterMoved(int a, int b)
{
    setFit(0);
    emit sigFileChanged(this);
}

void DocSheetPDF::onTitleShortCut(QString shortCut)
{
    m_browser->qDealWithShortKey(shortCut);
}

void DocSheetPDF::onFindOperation(int type, QString text)
{
    m_sidebar->handleFindOperation(type);
    m_browser->handleFindOperation(type, text);

    emit sigFindOperation(type);
}

void DocSheetPDF::onFindContentComming(const stSearchRes &res)
{
    m_sidebar->handleFindContentComming(res);
}

void DocSheetPDF::onFindFinished()
{
    int count = m_sidebar->handleFindFinished();
    m_pFindWidget->setEditAlert(count == 0);
}

void DocSheetPDF::onRotate(int rotate)
{
    m_sidebar->handleRotate(rotate);
}

void DocSheetPDF::onAnntationMsg(const int &msg, const QString &text)
{
    m_sidebar->handleAnntationMsg(msg, text);

    QStringList t_strList = text.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        int page = t_strList.at(2).trimmed().toInt();
        m_sidebar->handleUpdateThumbnail(page);
    }
}

QString DocSheetPDF::filePath()
{
    return m_browser->getFilePath();
}

int DocSheetPDF::qGetFileChange()
{
    int istatus = m_browser->getFileChange() ? 1 : 0;
    return  istatus;
}

void DocSheetPDF::saveOper()
{
    m_browser->saveOper();
}

bool DocSheetPDF::saveData()
{
    return m_browser->saveData();
}

bool DocSheetPDF::saveAsData(QString filePath)
{
    return m_browser->saveAsData(filePath);
}

void DocSheetPDF::setData(const int &type, const QVariant &value)
{

    m_browser->setOper(type, value);
}

QVariant DocSheetPDF::getOper(int type)
{
    return m_browser->getOper(type);
}

DocummentProxy *DocSheetPDF::getDocProxy()
{
    return m_browser->GetDocProxy();
}

void DocSheetPDF::OnOpenSliderShow()
{
    m_bOldState = m_sidebar->isVisible();
    m_sidebar->setVisible(false);
}

void DocSheetPDF::OnExitSliderShow()
{

    m_sidebar->setVisible(m_bOldState);
}

void DocSheetPDF::ShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(m_browser);
        m_browser->setFindWidget(m_pFindWidget);
        connect(m_pFindWidget, SIGNAL(sigFindOperation(const int &, const QString &)), this, SLOT(onFindOperation(const int &, const QString &)));
    } else {
        m_pFindWidget->setEditAlert(0);
    }

    m_pFindWidget->showPosition(m_browser->width());
    m_pFindWidget->setSearchEditFocus();
}

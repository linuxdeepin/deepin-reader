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
#include "djvuControl/SheetBrowserDJVU.h"
#include "business/PrintManager.h"

DocSheetPDF::DocSheetPDF(DWidget *parent)
    : DocSheet(Dr::PDF, parent)
{
    m_pRightWidget = new QStackedWidget(this);
    m_pSpinnerWidget = new SpinnerWidget(this);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_browser = new SheetBrowserPDF(this, this);
    m_sidebar = new SheetSidebarPDF(this);

    connect(m_sidebar, SIGNAL(sigDeleteAnntation(const int &, const QString &)), m_browser, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    connect(m_browser, SIGNAL(sigPageChanged(int)), m_sidebar, SLOT(onPageChanged(int)));
    connect(m_browser, SIGNAL(sigFileOpenResult(const QString &, const bool &)), this, SLOT(SlotFileOpenResult(const QString &, const bool &)));
    connect(m_browser, SIGNAL(sigAnntationMsg(const int &, const QString &)), this, SLOT(onAnntationMsg(int, QString)));
    connect(m_browser, SIGNAL(sigFileChanged()), this, SLOT(onFileChanged()));
    connect(m_browser, SIGNAL(sigRotateChanged(int)), this, SLOT(onRotate(int)));
    connect(m_browser, SIGNAL(sigFindContantComming(const stSearchRes &)), this, SLOT(onFindContentComming(const stSearchRes &)));
    connect(m_browser, SIGNAL(sigFindFinished()), this, SLOT(onFindFinished()));

    int tW = 36;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pSpinnerWidget->setSpinnerSize(QSize(tW, tH));
    m_pSpinnerWidget->startSpinner();

    m_pRightWidget->addWidget(m_pSpinnerWidget);
    m_pRightWidget->addWidget(m_browser);

    addWidget(m_sidebar);
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

void DocSheetPDF::openFile(const QString &filePath)
{
    m_browser->OpenFilePath(filePath);
}

void DocSheetPDF::pageJump(int page)
{
    if (!m_browser->GetDocProxy())
        return;

    DocummentProxy *_proxy =  m_browser->GetDocProxy();
    int nPageSize = _proxy->getPageSNum();      //  总页数
    if (page < 0 || page == nPageSize) {
        return;
    }

    int nCurPage = _proxy->currentPageNo();
    if (nCurPage != page) {
        _proxy->pageJump(page);
    }
}

void DocSheetPDF::pageFirst()
{
    pageJump(0);
}

void DocSheetPDF::pageLast()
{
    if (!m_browser->GetDocProxy())
        return;

    pageJump(getDocProxy()->getPageSNum() - 1);
}

void DocSheetPDF::pageNext()
{
    if (!m_browser->GetDocProxy())
        return;

    bool isDoubleShow = m_browser->isDoubleShow();

    int nCurPage = getDocProxy()->currentPageNo();

    int page = nCurPage + (isDoubleShow ? 2 : 1);

    page = (page >= (getDocProxy()->getPageSNum() - 1) ? (getDocProxy()->getPageSNum() - 1) : page);

    pageJump(page);
}

void DocSheetPDF::pagePrev()
{
    if (!m_browser->GetDocProxy())
        return;

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
            setFit(NO_ADAPTE_State);
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
            setFit(NO_ADAPTE_State);
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
    quitMagnifer();

    m_browser->setMouseDefault();
}

void DocSheetPDF::setMouseHand()
{
    quitMagnifer();

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
    setFit(NO_ADAPTE_State);
    emit sigFileChanged(this);
}

void DocSheetPDF::copySelectedText()
{
    m_browser->copySelectedText();
}

void DocSheetPDF::highlightSelectedText()
{
    m_browser->highlightSelectedText();
}

void DocSheetPDF::addSelectedTextHightlightAnnotation()
{
    m_browser->addSelectedTextHightlightAnnotation();
}

void DocSheetPDF::openSideBar()
{
    setSidebarVisible(true);

    setOper(Thumbnail, "1");

    emit sigFileChanged(this);
}

void DocSheetPDF::print()
{
    PrintManager p(this);
    p.showPrintDialog(this);
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

QString DocSheetPDF::filter()
{
    return "Pdf File (*.pdf)";
}

bool DocSheetPDF::getFileChanged()
{
    return m_browser->getFileChange();
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

QVariant DocSheetPDF::getOper(int type)
{
    return m_browser->getOper(type);
}

void DocSheetPDF::setOper(const int &type, const QVariant &value)
{
    m_browser->setOper(type, value);
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

void DocSheetPDF::exitSliderShow()
{
    m_sidebar->setVisible(m_bOldState);
}

void DocSheetPDF::handleSearch()
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

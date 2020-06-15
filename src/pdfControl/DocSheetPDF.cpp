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
#include "lpreviewControl/SheetSidebar.h"

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
#include "Database.h"

DocSheetPDF::DocSheetPDF(QString filePath, DWidget *parent)
    : DocSheet(Dr::PDF, filePath, parent)
{
    m_operation.scaleFactor = 100.0;
    m_pRightWidget = new QStackedWidget(this);
    m_pSpinnerWidget = new SpinnerWidget(this);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_browser = new SheetBrowserPDF(this, this);
    m_browser->setMinimumWidth(481);

    m_sidebar = new SheetSidebar(this);
    m_sidebar->setMinimumWidth(266);

    connect(m_sidebar, SIGNAL(sigDeleteAnntation(const int &, const QString &)), m_browser, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    connect(m_browser, SIGNAL(sigSizeChanged(double)), this, SLOT(onBrowserSizeChanged(double)));
    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onPageChanged(int)));
    connect(m_browser, SIGNAL(sigDataChanged()), this, SLOT(onDataChanged()));
    connect(m_browser, SIGNAL(sigFileOpenResult(const QString &, const bool &)), this, SLOT(onFileOpenResult(const QString &, const bool &)));
    connect(m_browser, SIGNAL(sigAnntationMsg(const int &, const QString &)), this, SLOT(onAnntationMsg(int, QString)));
    connect(m_browser, SIGNAL(sigFindContantComming(const stSearchRes &)), this, SLOT(onFindContentComming(const stSearchRes &)));
    connect(m_browser, SIGNAL(sigFindFinished()), this, SLOT(onFindFinished()));
    connect(m_browser, &SheetBrowserPDF::sigUpdateThumbnail, this, [ = ](const int &page) {
        m_sidebar->handleUpdateThumbnail(page);
    });

    int tW = 36;
    int tH = 36;

    m_pSpinnerWidget->setSpinnerSize(QSize(tW, tH));
    m_pSpinnerWidget->startSpinner();

    m_pRightWidget->addWidget(m_pSpinnerWidget);
    m_pRightWidget->addWidget(m_browser);

    addWidget(m_sidebar);
    addWidget(m_pRightWidget);

    QList<int> list_src;
    tW = LEFTNORMALWIDTH;

    list_src.append(tW);
    tW = 1000 - LEFTNORMALWIDTH;

    list_src.append(tW);

    setSizes(list_src);

    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved(int, int)));
}

DocSheetPDF::~DocSheetPDF()
{
    if (m_browser)
        m_browser->saveOper();
}

void DocSheetPDF::openFile()
{
    m_browser->OpenFilePath(filePath());
}

void DocSheetPDF::jumpToPage(int page)
{
    jumpToIndex(page - 1);
}

void DocSheetPDF::jumpToIndex(int index)
{
    if (!m_browser->GetDocProxy())
        return;

    DocummentProxy *_proxy =  m_browser->GetDocProxy();
    int nPageSize = _proxy->getPageSNum();      //  总页数
    if (index < 0 || index == nPageSize) {
        return;
    }

    int nCurPage = _proxy->currentPageNo();
    if (nCurPage != index) {
        _proxy->pageJump(index);
    }
}

void DocSheetPDF::jumpToFirstPage()
{
    jumpToIndex(0);
}

void DocSheetPDF::jumpToLastPage()
{
    if (!m_browser->GetDocProxy())
        return;

    jumpToIndex(this->pagesNumber() - 1);
}

void DocSheetPDF::jumpToNextPage()
{
    if (!m_browser->GetDocProxy())
        return;

    bool isDoubleShow = m_browser->isDoubleShow();

    int nCurPage = this->currentIndex();

    int page = nCurPage + (isDoubleShow ? 2 : 1);

    page = (page >= (this->pagesNumber() - 1) ? (this->pagesNumber() - 1) : page);

    jumpToIndex(page);
}

void DocSheetPDF::jumpToPrevPage()
{
    if (!m_browser->GetDocProxy())
        return;

    bool isDoubleShow = m_browser->isDoubleShow();

    int curIndex = this->currentIndex();

    int jIndex = curIndex - (isDoubleShow ? 2 : 1);

    jumpToIndex(jIndex);
}

void DocSheetPDF::rotateLeft()
{
    m_operation.rotation = static_cast<Dr::Rotation>(m_browser->rotateLeft());
    m_sidebar->handleRotate(m_operation.rotation);
}

void DocSheetPDF::rotateRight()
{
    m_operation.rotation = static_cast<Dr::Rotation>(m_browser->rotateRight());
    m_sidebar->handleRotate(m_operation.rotation);
}

void DocSheetPDF::setFileChanged(bool hasChanged)
{
    m_browser->setFileChanged(hasChanged);
}

void DocSheetPDF::setBookMark(int page, int state)
{
    if (state)
        m_bookmarks.insert(page);
    else {
        m_bookmarks.remove(page);
        showTips(tr("The bookmark has been removed"));
    }
    m_sidebar->setBookMark(page, state);
    m_browser->setBookMark(page, state);
    this->setFileChanged(true);
}

void DocSheetPDF::showNoteWidget(int page, const QString &uuid, const int &type)
{
    m_browser->showNoteWidget(page, uuid, type);
}

void DocSheetPDF::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode >= Dr::SinglePageMode && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        if (Dr::SinglePageMode == mode)
            m_browser->setDoubleShow(false);
        else if (Dr::TwoPagesMode == mode)
            m_browser->setDoubleShow(true);
    }
}

void DocSheetPDF::setMouseShape(Dr::MouseShape shape)
{
    if (shape >= Dr::MouseShapeNormal && shape < Dr::NumberOfMouseShapes) {
        quitMagnifer();
        m_operation.mouseShape = shape;

        if (shape == Dr::MouseShapeNormal)
            m_browser->setMouseDefault();
        else if (shape == Dr::MouseShapeHand)
            m_browser->setMouseHand();
        emit sigFileChanged(this);
    }
}

void DocSheetPDF::setScaleMode(Dr::ScaleMode mode)
{
    if (mode >= Dr::ScaleFactorMode && mode < Dr::NumberOfScaleModes) {
        m_operation.scaleMode = mode;
        m_operation.scaleFactor = m_browser->setFit(mode) / 100.00;
        emit sigFileChanged(this);
    }
}

void DocSheetPDF::setScaleFactor(qreal scaleFactor)
{
    m_operation.scaleMode = Dr::ScaleFactorMode;
    m_operation.scaleFactor = scaleFactor;
    m_browser->setScale(scaleFactor * 100);
    emit sigFileChanged(this);
}

void DocSheetPDF::openMagnifier()
{
    m_currentState = Magnifer_State;
}

void DocSheetPDF::closeMagnifier()
{
    m_currentState = Default_State;

    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        docProxy->closeMagnifier();
    }

}

bool DocSheetPDF::magnifierOpened()
{
    return m_currentState == Magnifer_State;;
}

bool DocSheetPDF::isDoubleShow()
{
    return m_browser->isDoubleShow();
}

void DocSheetPDF::quitMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitMagnifer();
}

void DocSheetPDF::setCurrentState(int state)
{
    m_currentState = state;
}

int DocSheetPDF::currentState()
{
    return m_currentState;
}

void DocSheetPDF::onFileOpenResult(const QString &, const bool &res)
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
}

void DocSheetPDF::onShowTips(const QString &tips, int index)
{
    showTips(tips, index);
}

void DocSheetPDF::onDataChanged()
{
    emit sigFileChanged(this);
}

void DocSheetPDF::onSplitterMoved(int, int)
{
    setScaleMode(Dr::ScaleFactorMode);
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

void DocSheetPDF::onAnntationMsg(const int &msg, const QString &text)
{
    m_sidebar->handleAnntationMsg(msg, text);

    QStringList t_strList = text.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        int page = t_strList.at(2).trimmed().toInt();
        m_sidebar->handleUpdateThumbnail(page);
    }
}

QString DocSheetPDF::filter()
{
    return "Pdf File (*.pdf)";
}

bool DocSheetPDF::fileChanged()
{
    return m_browser->getFileChange();
}

bool DocSheetPDF::saveData()
{
    Database::instance()->saveBookmarks(filePath(), m_bookmarks);
    return m_browser->saveData();
}

bool DocSheetPDF::saveAsData(QString filePath)
{
    Database::instance()->saveBookmarks(filePath, m_bookmarks);
    return m_browser->saveAsData(filePath);
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

int DocSheetPDF::pagesNumber()
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->getPageSNum();
    }
    return -1;
}

int DocSheetPDF::currentIndex()
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->currentPageNo();
    }
    return -1;
}

bool DocSheetPDF::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->getImage(index, image, width, height, mode);
    }
    return false;
}

void DocSheetPDF::docBasicInfo(stFileInfo &info)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        docProxy->docBasicInfo(info);
    }
}

double DocSheetPDF::getMaxZoomratio()
{

}

void DocSheetPDF::getAllAnnotation(QList<stHighlightContent> &listres)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        docProxy->getAllAnnotation(listres);
    }
}

Outline DocSheetPDF::outline()
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->outline();
    }
    return Outline();
}

void DocSheetPDF::jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        docProxy->jumpToOutline(realleft, realtop, ipage);
    }
}

bool DocSheetPDF::isOpen()
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    return docProxy != nullptr;
}

QString DocSheetPDF::addIconAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->addIconAnnotation(pos, color, type);
    }
    return "";
}

void DocSheetPDF::defaultFocus()
{
    if (m_browser)
        m_browser->defaultFocus();
}

QList<qreal> DocSheetPDF::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};
    QList<qreal> factorList;

    qreal maxZoom = 0;
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        maxZoom = docProxy->getMaxZoomratio();
    }

    foreach (qreal factor, dataList) {
        if (maxZoom - factor > 0.001)
            factorList.append(factor);
    }

    return  factorList;
}

int  DocSheetPDF::pointInWhichPage(QPoint pos)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->pointInWhichPage(pos);
    }
    return -1;
}

void DocSheetPDF::jumpToHighLight(const QString &uuid, int index)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        docProxy->jumpToHighLight(uuid, index);
    }
}

QString DocSheetPDF::pagenum2label(int index)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->pagenum2label(index);
    }
    return QString();
}

bool DocSheetPDF::haslabel()
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->haslabel();
    }
    return false;
}

int DocSheetPDF::label2pagenum(QString label)
{
    DocummentProxy *docProxy = m_browser->GetDocProxy();
    if (docProxy) {
        return docProxy->label2pagenum(label);
    }
    return -1;
}

void DocSheetPDF::onPageChanged(int index)
{
    int page = index + 1;
    m_operation.currentPage = page;
    m_sidebar->setCurrentPage(page);
}

void DocSheetPDF::onBrowserSizeChanged(double scaleFactor)
{
    if (m_operation.scaleMode != Dr::ScaleFactorMode) {
        m_browser->setScale(scaleFactor * 100);
    }
}

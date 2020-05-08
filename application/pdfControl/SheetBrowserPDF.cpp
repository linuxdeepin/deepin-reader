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
#include "SheetBrowserPDF.h"

#include "widgets/FindWidget.h"

#include "SheetBrowserPDFPrivate.h"
#include "controller/ProxyData.h"
#include "controller/ProxyFileDataModel.h"
#include "controller/ProxyViewDisplay.h"
#include "pdfControl/docview/docummentproxy.h"
#include "DocSheet.h"

void SheetBrowserPDF::setDoubleShow(bool isShow)
{
    Q_D(SheetBrowserPDF);
    setOper(DoubleShow, isShow);
    d->m_pDocViewProxy->setDoubleShow(isShow);
    d->m_pDocViewProxy->setScaleRotateViewModeAndShow();

    double scale = d->handleResize(size());
    setOper(Scale, scale);
    emit setFileChanged(getFileChange());
}

void SheetBrowserPDF::setRotateLeft()
{
    Q_D(SheetBrowserPDF);
    int rotate = d->m_pDocViewProxy->setViewRotateLeft();
    setOper(Rotate, rotate);
    emit sigRotateChanged(rotate);
}

void SheetBrowserPDF::setRotateRight()
{
    Q_D(SheetBrowserPDF);
    int rotate = d->m_pDocViewProxy->setViewRotateRight();
    setOper(Rotate, rotate);
    emit sigRotateChanged(rotate);
}

SheetBrowserPDF::SheetBrowserPDF(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), d_ptr(new SheetBrowserPDFPrivate(sheet, this))
{
    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();
}

SheetBrowserPDF::~SheetBrowserPDF()
{

}

void SheetBrowserPDF::setFileChanged(bool hasChanged)
{
    Q_D(SheetBrowserPDF);

    d->m_pProxyData->setFileChanged(hasChanged);
}

void SheetBrowserPDF::setMouseDefault()
{
    Q_D(SheetBrowserPDF);

    if (!d->m_pProxy)
        return;

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    d->m_pProxy->mouseSelectTextClear();

    d->__SetCursor(Qt::ArrowCursor);

    setOper(HandShape, 0);

    emit sigFileChanged();
}

void SheetBrowserPDF::setMouseHand()
{
    Q_D(SheetBrowserPDF);

    if (!d->m_pProxy)
        return;

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    d->m_pProxy->mouseSelectTextClear();

    d->__SetCursor(Qt::OpenHandCursor);

    setOper(HandShape, 1);

    emit sigFileChanged();
}

bool SheetBrowserPDF::isMouseHand()
{
    return 1 == getOper(HandShape).toInt();
}

bool SheetBrowserPDF::isDoubleShow()
{
    Q_D(SheetBrowserPDF);

    return d->m_pDocViewProxy->isDoubleShow();
}

void SheetBrowserPDF::setScale(double scale)
{
    Q_D(SheetBrowserPDF);

    setOper(Scale, scale);

    d->m_pDocViewProxy->setScale(scale);

    d->m_pDocViewProxy->setScaleRotateViewModeAndShow();

}

bool SheetBrowserPDF::setFit(int fit)
{
    Q_D(SheetBrowserPDF);

    if (d->m_pProxyFileDataModel->getOper(Fit) == fit)
        return false;

    setOper(Fit, fit);

    double scale = d->m_pDocViewProxy->setFit(fit);

    if (-1 != scale && fit != NO_ADAPTE_State) {
        setOper(Scale, scale);
    }

    emit sigFileChanged();

    return true;
}

void SheetBrowserPDF::setBookMark(int page, int state)
{
    Q_D(SheetBrowserPDF);

    d->m_pProxy->setBookMarkState(page, state);
}

void SheetBrowserPDF::showNoteWidget(int page, const QString &uuid, const int &type)
{
    Q_D(SheetBrowserPDF);

    QString text;

    d->m_pProxy->getAnnotationText(uuid, text, page);

    d->showNoteViewWidget(QString::number(page), uuid, text, type);
}

void SheetBrowserPDF::initWidget()
{
}

//  鼠标移动
void SheetBrowserPDF::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(SheetBrowserPDF);
    d->mouseMoveEvent(event);
}

//  鼠标左键 按下
void SheetBrowserPDF::mousePressEvent(QMouseEvent *event)
{
    Q_D(SheetBrowserPDF);
    d->mousePressEvent(event);
}

//  鼠标松开
void SheetBrowserPDF::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(SheetBrowserPDF);
    d->mouseReleaseEvent(event);
}

//  文档 显示区域 大小变化
void SheetBrowserPDF::resizeEvent(QResizeEvent *event)
{
    Q_D(SheetBrowserPDF);

    if (d->hasOpened()) {
        double scale = d->handleResize(event->size());

        if (getOper(Fit) != NO_ADAPTE_State) {
            setOper(Scale, scale);
            emit setFileChanged(getFileChange());
        }
    }

    CustomWidget::resizeEvent(event);
}

void SheetBrowserPDF::wheelEvent(QWheelEvent *event)
{
    Q_D(SheetBrowserPDF);
    d->wheelEvent(event);

    CustomWidget::wheelEvent(event);
}

void SheetBrowserPDF::leaveEvent(QEvent *event)
{
    Q_D(SheetBrowserPDF);
    d->hidetipwidget();
    CustomWidget::leaveEvent(event);
}

void SheetBrowserPDF::OpenFilePath(const QString &sPath)
{
    Q_D(SheetBrowserPDF);
    d->OpenFilePath(sPath);
}

QString SheetBrowserPDF::getFilePath()
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxyData->getPath();
}

void SheetBrowserPDF::saveOper()
{
    Q_D(SheetBrowserPDF);
    d->m_pProxyFileDataModel->saveOper();
}

bool SheetBrowserPDF::saveData()
{
    Q_D(SheetBrowserPDF);

    bool result = d->m_pProxy->save(d->m_pProxyData->getPath());
    if (!result)
        return false;

    d->m_pProxyData->setFileChanged(false);

    d->m_pProxyFileDataModel->saveData();

    return true;
}

bool SheetBrowserPDF::saveAsData(QString targetFilePath)
{
    Q_D(SheetBrowserPDF);

    bool result = d->m_pProxy->save(targetFilePath);

    if (!result)
        return false;

    d->m_pProxyFileDataModel->saveAsData(d->m_pProxyData->getPath(), targetFilePath);

    return true;
}

void SheetBrowserPDF::handleFindOperation(const int &iType, const QString &strFind)
{
    Q_D(SheetBrowserPDF);

    if (iType == E_FIND_NEXT) {
        d->m_pProxy->findNext();
    } else if (iType == E_FIND_PREV) {
        d->m_pProxy->findPrev();
    } else if (iType == E_FIND_EXIT) {
        d->m_pProxy->clearsearch();
    } else {
        d->m_pProxy->search(strFind);
    }
}

void SheetBrowserPDF::setFindWidget(FindWidget *findWidget)
{
    Q_D(SheetBrowserPDF);

    d->setFindWidget(findWidget);
}

void SheetBrowserPDF::copySelectedText()
{
    Q_D(SheetBrowserPDF);
    //  处于幻灯片模式下
    int nState = d->m_sheet->getCurrentState();

    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (isMouseHand())
        return;

    d->DocFile_ctrl_c();
}

void SheetBrowserPDF::highlightSelectedText()
{
    Q_D(SheetBrowserPDF);
    //  处于幻灯片模式下
    int nState = d->m_sheet->getCurrentState();

    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (isMouseHand())
        return;

    d->DocFile_ctrl_l();
}

void SheetBrowserPDF::addSelectedTextHightlightAnnotation()
{
    Q_D(SheetBrowserPDF);
    //  处于幻灯片模式下
    int nState = d->m_sheet->getCurrentState();

    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (isMouseHand())
        return;

    d->DocFile_ctrl_i();
}

//  信号槽　初始化
void SheetBrowserPDF::initConnections()
{
    Q_D(SheetBrowserPDF);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), d, SLOT(slotCustomContextMenuRequested(const QPoint &)));
    connect(this, SIGNAL(sigDeleteAnntation(const int &, const QString &)), d, SLOT(SlotDeleteAnntation(const int &, const QString &)));
}

bool SheetBrowserPDF::getFileChange()
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxyData->IsFileChanged();
}

DocummentProxy *SheetBrowserPDF::GetDocProxy()
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxy;
}

void SheetBrowserPDF::setOper(const int &nType, const QVariant &sValue)
{
    Q_D(SheetBrowserPDF);
    d->m_pProxyFileDataModel->setOper(nType, sValue);
}

QVariant SheetBrowserPDF::getOper(int type)
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxyFileDataModel->getOper(type);
}

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

void SheetBrowserPDF::setDoubleShow(bool isShow)
{
    Q_D(SheetBrowserPDF);
    d->m_pDocViewProxy->setDoubleShow(isShow);
    d->m_pDocViewProxy->setScaleRotateViewModeAndShow();
    d->handleResize(size());
}

SheetBrowserPDF::SheetBrowserPDF(DocSheet *sheet, DWidget *parent)
    : CustomWidget(FILE_VIEW_WIDGET, parent), d_ptr(new SheetBrowserPDFPrivate(sheet, this))
{
    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

SheetBrowserPDF::~SheetBrowserPDF()
{
    dApp->m_pModelService->removeObserver(this);
}

void SheetBrowserPDF::setFileChanged(bool hasChanged)
{
    Q_D(SheetBrowserPDF);
    d->m_pProxyData->setFileChanged(hasChanged);
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
    d->resizeEvent(event);

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

void SheetBrowserPDF::ShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
        connect(m_pFindWidget, SIGNAL(sigFindOperation(const int &, const QString &)), SLOT(SlotFindOperation(const int &, const QString &)));
    }

    int nParentWidth = this->width();
    m_pFindWidget->showPosition(nParentWidth);
    m_pFindWidget->setSearchEditFocus();
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

void SheetBrowserPDF::saveData()
{
    Q_D(SheetBrowserPDF);
    d->m_pProxyData->setFileChanged(false);
    d->m_pProxyFileDataModel->saveData();
}

void SheetBrowserPDF::SlotFindOperation(const int &iType, const QString &strFind)
{
    Q_D(SheetBrowserPDF);
    emit sigFindOperation(iType);

    d->FindOperation(iType, strFind);
}

//  信号槽　初始化
void SheetBrowserPDF::initConnections()
{
    Q_D(SheetBrowserPDF);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), d, SLOT(slotCustomContextMenuRequested(const QPoint &)));
    connect(this, SIGNAL(sigDeleteAnntation(const int &, const QString &)), d, SLOT(SlotDeleteAnntation(const int &, const QString &)));
}

//  消息 数据 处理
int SheetBrowserPDF::dealWithData(const int &msgType, const QString &msgContent)
{
    Q_D(SheetBrowserPDF);
    return d->dealWithData(msgType, msgContent);
}

int SheetBrowserPDF::qDealWithShortKey(const QString &sKey)
{
    Q_D(SheetBrowserPDF);
    return d->qDealWithShortKey(sKey);
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

void SheetBrowserPDF::setData(const int &nType, const QString &sValue)
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxyFileDataModel->setData(nType, sValue);
}

FileDataModel SheetBrowserPDF::qGetFileData()
{
    Q_D(SheetBrowserPDF);
    return d->m_pProxyFileDataModel->qGetFileData();
}

void SheetBrowserPDF::qSetFileData(const FileDataModel &fdm)
{
    Q_D(SheetBrowserPDF);
    d->m_pProxyFileDataModel->qSetFileData(fdm);
}

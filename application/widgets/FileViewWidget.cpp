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
#include "FileViewWidget.h"

#include "FindWidget.h"

#include "menu/DefaultOperationMenu.h"
#include "controller/FileViewWidgetPrivate.h"
#include "widgets/controller/ProxyData.h"
#include "widgets/controller/ProxyFileDataModel.h"

FileViewWidget::FileViewWidget(DWidget *parent)
    : CustomWidget(FILE_VIEW_WIDGET, parent), d_ptr(new FileViewWidgetPrivate(this))
{
    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

FileViewWidget::~FileViewWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void FileViewWidget::initWidget()
{
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(FileViewWidget);
    d->mouseMoveEvent(event);
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(FileViewWidget);
    d->mousePressEvent(event);
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(FileViewWidget);
    d->mouseReleaseEvent(event);
}

//  文档 显示区域 大小变化
void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(FileViewWidget);
    d->resizeEvent(event);

    CustomWidget::resizeEvent(event);
}

void FileViewWidget::wheelEvent(QWheelEvent *event)
{
    Q_D(FileViewWidget);
    d->wheelEvent(event);

    CustomWidget::wheelEvent(event);
}

void FileViewWidget::leaveEvent(QEvent *event)
{
    Q_D(FileViewWidget);
    d->hidetipwidget();
    CustomWidget::leaveEvent(event);
}

void FileViewWidget::ShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
        connect(m_pFindWidget, SIGNAL(sigFindOperation(const int &, const QString &)), SLOT(SlotFindOperation(const int &, const QString &)));
    }

    int nParentWidth = this->width();
    m_pFindWidget->showPosition(nParentWidth);
    m_pFindWidget->setSearchEditFocus();
}

void FileViewWidget::OpenFilePath(const QString &sPath)
{
    Q_D(FileViewWidget);
    d->OpenFilePath(sPath);
}

QString FileViewWidget::getFilePath()
{
    Q_D(FileViewWidget);
    return d->m_pProxyData->getPath();
}

void FileViewWidget::saveData()
{
    Q_D(FileViewWidget);
    setFileChange(false);
    d->m_pProxyFileDataModel->saveData();
}

void FileViewWidget::SlotFindOperation(const int &iType, const QString &strFind)
{
    Q_D(FileViewWidget);
    emit sigFindOperation(iType);

    d->FindOperation(iType, strFind);
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    Q_D(FileViewWidget);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), d, SLOT(slotCustomContextMenuRequested(const QPoint &)));
    connect(this, SIGNAL(sigDeleteAnntation(const int &, const QString &)), d, SLOT(SlotDeleteAnntation(const int &, const QString &)));
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    Q_D(FileViewWidget);
    return d->dealWithData(msgType, msgContent);
}

int FileViewWidget::qDealWithShortKey(const QString &sKey)
{
    Q_D(FileViewWidget);
    return d->qDealWithShortKey(sKey);
}

void FileViewWidget::setFileChange(bool bchanged)
{
    Q_D(FileViewWidget);
    d->m_pProxyData->setFileChanged(bchanged);
}

bool FileViewWidget::getFileChange()
{
    Q_D(FileViewWidget);
    return d->m_pProxyData->IsFileChanged();
}

DocummentProxy *FileViewWidget::GetDocProxy()
{
    Q_D(FileViewWidget);
    return d->m_pProxy;
}

FileDataModel FileViewWidget::qGetFileData()
{
    Q_D(FileViewWidget);
    return d->m_pProxyFileDataModel->qGetFileData();
}

void FileViewWidget::qSetFileData(const FileDataModel &fdm)
{
    Q_D(FileViewWidget);
    d->m_pProxyFileDataModel->qSetFileData(fdm);
}

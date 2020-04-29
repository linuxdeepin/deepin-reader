/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "imagelistview.h"
#include "DocSheet.h"
#include "docview/docummentproxy.h"
#include "pdfControl/imageviewdelegate.h"
#include "pdfControl/imageviewmodel.h"
#include "application.h"

#include <QMouseEvent>

ImageListView::ImageListView(DocSheet *sheet, QWidget* parent)
    : DListView(parent)
    , m_docSheet(sheet)
{
    initControl();
    setProperty("adaptScale", 1.0);
    setSpacing(4);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Fixed);
    setViewMode(QListView::ListMode);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void ImageListView::initControl()
{
    m_imageModel = new ImageViewModel(this);
    m_imageModel->setDocSheet(m_docSheet);
    this->setModel(m_imageModel);

    m_imageDelegate = new ImageViewDelegate(this);
    this->setItemDelegate(m_imageDelegate);

    connect(this, &DListView::clicked, this, &ImageListView::onItemClicked);
}

void ImageListView::setBoolMarkVisible(int pageIndex, bool visible)
{
    m_imageModel->setBookMarkVisible(pageIndex, visible);
}

void ImageListView::handleOpenSuccess()
{
    DocummentProxy* docProxy = m_docSheet->getDocProxy();
    Q_ASSERT(docProxy);
    const QList<int>& pageList = dApp->m_pDBService->getBookMarkList(m_docSheet->filePath());
    for(int pageIndex : pageList){
        m_imageModel->setBookMarkVisible(pageIndex, true, false);
    }
    m_imageModel->setPageCount(docProxy->getPageSNum());
}

void ImageListView::onUpdatePageImage(int pageIndex)
{
    m_imageModel->onUpdatePageImage(pageIndex);
}

void ImageListView::updatePageIndex(int index)
{
    m_imageModel->updatePageIndex(index);
}

void ImageListView::onItemClicked(const QModelIndex &index)
{
    m_docSheet->pageJump(index.row());
}

void ImageListView::scrollToIndex(int pageIndex)
{
    const QModelIndex &index = this->model()->index(pageIndex, 0);
    this->scrollTo(index);
    this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
}

void ImageListView::mousePressEvent(QMouseEvent *event)
{
    DListView::mousePressEvent(event);
    onItemClicked(this->indexAt(event->pos()));
}

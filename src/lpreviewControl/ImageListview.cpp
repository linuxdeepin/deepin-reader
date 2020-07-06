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
#include "ImageListview.h"
#include "DocSheet.h"
#include "pdfControl/docview/DocummentProxy.h"
#include "ImageViewModel.h"
#include "Application.h"
#include "menu/BookMarkMenu.h"
#include "menu/NoteMenu.h"

#include <QMouseEvent>

ImageListView::ImageListView(DocSheet *sheet, QWidget *parent)
    : DListView(parent)
    , m_docSheet(sheet)
{
    initControl();
    setAutoScroll(false);
    setProperty("adaptScale", 1.0);
    setSpacing(4);
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Fixed);
    setViewMode(QListView::ListMode);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void ImageListView::initControl()
{
    m_pBookMarkMenu = nullptr;
    m_pNoteMenu = nullptr;
    m_listType = E_SideBar::SIDE_THUMBNIL;
    m_imageModel = new ImageViewModel(this);
    m_imageModel->setDocSheet(m_docSheet);
    this->setModel(m_imageModel);
}

ImageViewModel *ImageListView::getImageModel()
{
    return m_imageModel;
}

void ImageListView::setListType(int type)
{
    m_listType = type;
}

void ImageListView::handleOpenSuccess()
{
    if (m_listType == E_SideBar::SIDE_THUMBNIL) {
        const QSet<int> &pageList = m_docSheet->getBookMarkList();
        for (int pageIndex : pageList) {
            m_imageModel->setBookMarkVisible(pageIndex, true, false);
        }

        QList<ImagePageInfo_t> pageSrclst;
        int pagesNum = m_docSheet->pagesNumber();
        for (int index = 0; index < pagesNum; index++)
            pageSrclst << index;
        m_imageModel->initModelLst(pageSrclst);
    } else if (m_listType == E_SideBar::SIDE_BOOKMARK) {
        const QSet<int> &pageList = m_docSheet->getBookMarkList();
        QList<ImagePageInfo_t> pageSrclst;
        for (int pageIndex : pageList)
            pageSrclst << pageIndex;
        m_imageModel->initModelLst(pageSrclst, true);
    } else if (m_listType == E_SideBar::SIDE_NOTE) {
        QList<stHighlightContent> noteLst;
        m_docSheet->getAllAnnotation(noteLst);
        QList<ImagePageInfo_t> pageSrclst;
        int pagesNum = noteLst.size();
        for (int index = 0; index < pagesNum; index++) {
            const stHighlightContent &stH = noteLst.at(index);
            if (!stH.strcontents.isEmpty()) {
                int pageIndex = static_cast<int>(stH.ipage);
                ImagePageInfo_t tImagePageInfo;
                tImagePageInfo.pageIndex = pageIndex;
                tImagePageInfo.iType = Note_Type::NOTE_HIGHLIGHT;
                tImagePageInfo.struuid = stH.struuid;
                tImagePageInfo.strcontents = stH.strcontents;
                pageSrclst << tImagePageInfo;
            }
        }
        m_imageModel->initModelLst(pageSrclst);
    }
}

void ImageListView::onItemClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        m_docSheet->jumpToIndex(m_imageModel->getPageIndexForModelIndex(index.row()));
        emit sigListItemClicked(index.row());
    }
}

bool ImageListView::scrollToIndex(int index, bool scrollTo)
{
    const QList<QModelIndex> &indexlst = m_imageModel->getModelIndexForPageIndex(index);
    if (indexlst.size() > 0) {
        const QModelIndex &index = indexlst.first();
        if (scrollTo)
            this->scrollTo(index);
        this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(index);
        return true;
    } else {
        this->clearSelection();
        return false;
    }
}

void ImageListView::scrollToModelInexPage(const QModelIndex &index, bool scrollto)
{
    if (index.isValid()) {
        this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(index);
        if (scrollto) this->scrollTo(index);
    }
}

void ImageListView::mousePressEvent(QMouseEvent *event)
{
    DListView::mousePressEvent(event);
    onItemClicked(this->indexAt(event->pos()));
    //Menu
    if (event->button() == Qt::RightButton) {
        const QModelIndex &modelIndex = this->indexAt(event->pos());
        if (modelIndex.isValid()) {
            if (m_listType == E_SideBar::SIDE_NOTE) {
                showNoteMenu();
            } else if (m_listType == E_SideBar::SIDE_BOOKMARK) {
                showBookMarkMenu();
            }
        }
    }
}

void ImageListView::showNoteMenu()
{
    if (m_pNoteMenu == nullptr) {
        m_pNoteMenu = new NoteMenu(this);
        connect(m_pNoteMenu, SIGNAL(sigClickAction(const int &)), this, SIGNAL(sigListMenuClick(const int &)));
    }
    m_pNoteMenu->exec(QCursor::pos());
}

void ImageListView::showBookMarkMenu()
{
    if (m_pBookMarkMenu == nullptr) {
        m_pBookMarkMenu = new BookMarkMenu(this);
        connect(m_pBookMarkMenu, SIGNAL(sigClickAction(const int &)), this, SIGNAL(sigListMenuClick(const int &)));
    }
    m_pBookMarkMenu->exec(QCursor::pos());
}

int  ImageListView::getModelIndexForPageIndex(int pageIndex)
{
    const QList<QModelIndex> &indexlst = m_imageModel->getModelIndexForPageIndex(pageIndex);
    if (indexlst.size() > 0) {
        return indexlst.first().row();
    }
    return -1;
}

int  ImageListView::getPageIndexForModelIndex(int row)
{
    return m_imageModel->getPageIndexForModelIndex(row);
}

void ImageListView::onUpdatePageImage(int pageIndex)
{
    m_imageModel->onUpdatePageImage(pageIndex);
}

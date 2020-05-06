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
#include "pdfControl/imageviewmodel.h"
#include "application.h"
#include "WidgetHeader.h"
#include "menu/BookMarkMenu.h"
#include "menu/NoteMenu.h"

#include <QMouseEvent>

ImageListView::ImageListView(DocSheet *sheet, QWidget* parent)
    : DListView(parent)
    , m_docSheet(sheet)
{
    initControl();
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
    m_listType = DR_SPACE::E_THUMBAIL_WIDGET;
    m_imageModel = new ImageViewModel(this);
    m_imageModel->setDocSheet(m_docSheet);
    this->setModel(m_imageModel);

    connect(this, &DListView::clicked, this, &ImageListView::onItemClicked);
}

void ImageListView::setBoolMarkVisible(int pageIndex, bool visible)
{
    m_imageModel->setBookMarkVisible(pageIndex, visible);
}

void ImageListView::setListType(int type)
{
    m_listType = type;
}

void ImageListView::handleOpenSuccess()
{
    DocummentProxy* docProxy = m_docSheet->getDocProxy();
    Q_ASSERT(docProxy);
    const QList<int>& pageList = dApp->m_pDBService->getBookMarkList(m_docSheet->filePath());
    if(m_listType == DR_SPACE::E_THUMBAIL_WIDGET){
        for(int pageIndex : pageList){
            m_imageModel->setBookMarkVisible(pageIndex, true, false);
        }

        QList<int> pageSrclst;
        int pagesNum = docProxy->getPageSNum();
        for(int index = 0; index < pagesNum; index++)
            pageSrclst << index;
        m_imageModel->initModelLst(pageSrclst);
    }
    else if(m_listType == DR_SPACE::E_BOOKMARK_WIDGET){
        m_imageModel->initModelLst(pageList, true);
    }
}

void ImageListView::onUpdatePageImage(int pageIndex)
{
    m_imageModel->onUpdatePageImage(pageIndex);
}

void ImageListView::updatePageIndex(int index)
{
    m_imageModel->updatePageIndex(index);
}

void ImageListView::insertPageIndex(int pageIndex)
{
    m_imageModel->insertPageIndex(pageIndex);
}
void ImageListView::removePageIndex(int pageIndex)
{
    m_imageModel->removePageIndex(pageIndex);
}

void ImageListView::onItemClicked(const QModelIndex &index)
{
    m_docSheet->pageJump(m_imageModel->getPageIndexForModelIndex(index.row()));
}

bool ImageListView::scrollToIndex(int pageIndex, bool scrollTo)
{
    const QModelIndex &index = m_imageModel->getModelIndexForPageIndex(pageIndex);
    if(index.isValid()){
        if(scrollTo)
            this->scrollTo(index);
        this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(index);
        return true;
    }
    else{
        this->clearSelection();
        return false;
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
            if (m_listType == DR_SPACE::E_NOTE_WIDGET) {
                showNoteMenu();
            } else if (m_listType == DR_SPACE::E_BOOKMARK_WIDGET) {
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
    const QModelIndex &index = m_imageModel->getModelIndexForPageIndex(pageIndex);
    if(index.isValid()){
        return index.row();
    }
    return -1;
}

int  ImageListView::getPageIndexForModelIndex(int row)
{
    return m_imageModel->getPageIndexForModelIndex(row);
}

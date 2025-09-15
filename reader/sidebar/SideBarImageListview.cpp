// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SideBarImageListview.h"
#include "DocSheet.h"
#include "SideBarImageViewModel.h"
#include "Application.h"
#include "MsgHeader.h"

#include <QScroller>
#include <QScrollBar>
#include <QSet>
#include <QMouseEvent>
#include <QDebug>
SideBarImageListView::SideBarImageListView(DocSheet *sheet, QWidget *parent)
    : DListView(parent)
    , m_docSheet(sheet)
{
    qDebug() << "SideBarImageListView created for document:" << (sheet ? sheet->filePath() : "null");
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

    QScroller::grabGesture(this, QScroller::TouchGesture);//滑动
    qDebug() << "Enabled touch gesture scrolling";

    connect(verticalScrollBar(), &QScrollBar::sliderPressed, this, &SideBarImageListView::onRemoveThumbnailListSlideGesture);
    connect(verticalScrollBar(), &QScrollBar::sliderReleased, this, &SideBarImageListView::onSetThumbnailListSlideGesture);
    qDebug() << "Connected scrollbar signals";
}


void SideBarImageListView::initControl()
{
    qDebug() << "SideBarImageListView::initControl start";
    m_pBookMarkMenu = nullptr;
    m_pNoteMenu = nullptr;
    m_listType = E_SideBar::SIDE_THUMBNIL;
    m_imageModel = new SideBarImageViewModel(m_docSheet, this);
    this->setModel(m_imageModel);
    qDebug() << "SideBarImageListView::initControl end";
}

SideBarImageViewModel *SideBarImageListView::getImageModel()
{
    return m_imageModel;
}

void SideBarImageListView::showMenu()
{
    qDebug() << "SideBarImageListView::showMenu start";
    if (!this->currentIndex().isValid()) {
        qDebug() << "Current index not valid, returning";
        return;
    }

    const QRect &visualRect = this->visualRect(this->currentIndex());
    QPoint point = mapToGlobal(QPoint(this->width() / 2 - 4, visualRect.y() + visualRect.height() / 2 - 4));
    if (m_listType == E_SideBar::SIDE_NOTE) {
        qDebug() << "Showing note menu";
        showNoteMenu(point);
    } else if (m_listType == E_SideBar::SIDE_BOOKMARK) {
        qDebug() << "Showing bookmark menu";
        showBookMarkMenu(point);
    }
    qDebug() << "SideBarImageListView::showMenu end";
}

void SideBarImageListView::setListType(int type)
{
    qDebug() << "Setting list type to:" << type;
    m_listType = type;
}

void SideBarImageListView::handleOpenSuccess()
{
    qDebug() << "Handling open success for list type:" << m_listType;
    if (m_listType == E_SideBar::SIDE_THUMBNIL) {
        qDebug() << "Handling open success for thumbnail widget";
        const QSet<int> &pageList = m_docSheet->getBookMarkList();
        for (int pageIndex : pageList) {
            m_imageModel->setBookMarkVisible(pageIndex, true, false);
        }

        QList<ImagePageInfo_t> pageSrclst;
        int pagesNum = m_docSheet->pageCount();
        for (int index = 0; index < pagesNum; index++)
            pageSrclst << ImagePageInfo_t(index);
        m_imageModel->initModelLst(pageSrclst);
    } else if (m_listType == E_SideBar::SIDE_BOOKMARK) {
        qDebug() << "Handling open success for bookmark widget";
        const QSet<int> &pageList = m_docSheet->getBookMarkList();
        QList<ImagePageInfo_t> pageSrclst;
        for (int pageIndex : pageList)
            pageSrclst << ImagePageInfo_t(pageIndex);
        m_imageModel->initModelLst(pageSrclst, true);
    } else if (m_listType == E_SideBar::SIDE_NOTE) {
        qDebug() << "Handling open success for notes widget";
        const QList< deepin_reader::Annotation * > &annotationlst = m_docSheet->annotations();
        QList<ImagePageInfo_t> pageSrclst;
        int pagesNum = annotationlst.size();
        for (int index = 0; index < pagesNum; index++) {
            deepin_reader::Annotation *annotion = annotationlst.at(index);
            if (!annotion->contents().isEmpty()) {
                int pageIndex = static_cast<int>(annotion->page) - 1;
                ImagePageInfo_t tImagePageInfo;
                tImagePageInfo.pageIndex = pageIndex;
                tImagePageInfo.strcontents = annotion->contents();
                tImagePageInfo.annotation = annotion;
                pageSrclst << tImagePageInfo;
            }
        }

        m_imageModel->initModelLst(pageSrclst);
    }
}

void SideBarImageListView::onItemClicked(const QModelIndex &index)
{
    // qDebug() << "Item clicked at row:" << index.row();
    if (index.isValid()) {
        // qDebug() << "Item clicked at row:" << index.row();
        int pageIndex = m_imageModel->getPageIndexForModelIndex(index.row());
        // qDebug() << "Item clicked at row:" << index.row() << "jumping to page:" << pageIndex;
        m_docSheet->jumpToIndex(pageIndex);
        emit sigListItemClicked(index.row());
    } else {
        qWarning() << "Invalid index in onItemClicked";
    }
}

void SideBarImageListView::onSetThumbnailListSlideGesture()
{
    qDebug() << "Setting thumbnail list slide gesture";
    QScroller::grabGesture(this, QScroller::TouchGesture);//缩略图列表滑动
}

void SideBarImageListView::onRemoveThumbnailListSlideGesture()
{
    qDebug() << "Removing thumbnail list slide gesture";
    QScroller::grabGesture(this, QScroller::MiddleMouseButtonGesture);//缩略图列表不滑动
}

bool SideBarImageListView::scrollToIndex(int index, bool scrollTo)
{
    qDebug() << "Scrolling to index:" << index << "scrollTo:" << scrollTo;
    const QList<QModelIndex> &indexlst = m_imageModel->getModelIndexForPageIndex(index);
    if (indexlst.size() > 0) {
        qDebug() << "Found model index for page index:" << index;
        const QModelIndex &index = indexlst.first();
        if (scrollTo)
            this->scrollTo(index);
        this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(index);
        qDebug() << "Successfully scrolled to model index:" << index.row();
        return true;
    } else {
        qWarning() << "No model index found for page index:" << index;
        this->setCurrentIndex(QModelIndex());
        this->clearSelection();
        return false;
    }
}

void SideBarImageListView::scrollToModelInexPage(const QModelIndex &index, bool scrollto)
{
    qDebug() << "Scrolling to model index:" << index << "scrollto:" << scrollto;
    if (index.isValid()) {
        qDebug() << "Model index is valid";
        this->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(index);
        if (scrollto) this->scrollTo(index);
    }
    qDebug() << "Scrolling to model index end";
}

void SideBarImageListView::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "SideBarImageListView::mousePressEvent start - button:" << event->button();
    DListView::mousePressEvent(event);
    onItemClicked(this->indexAt(event->pos()));
    //Menu
    if (event->button() == Qt::RightButton) {
        // qDebug() << "Right button pressed, checking for menu";
        const QModelIndex &modelIndex = this->indexAt(event->pos());
        if (modelIndex.isValid()) {
            if (m_listType == E_SideBar::SIDE_NOTE) {
                // qDebug() << "Showing note menu";
                showNoteMenu(QCursor::pos());
            } else if (m_listType == E_SideBar::SIDE_BOOKMARK) {
                // qDebug() << "Showing bookmark menu";
                showBookMarkMenu(QCursor::pos());
            }
        }
    }
    // qDebug() << "SideBarImageListView::mousePressEvent end";
}

void SideBarImageListView::showNoteMenu(const QPoint &point)
{
    qDebug() << "Showing note menu at:" << point;
    if (m_pNoteMenu == nullptr) {
        qDebug() << "Creating new note menu";
        m_pNoteMenu = new DMenu(this);
        m_pNoteMenu->setAccessibleName("Menu_Note");

        QAction *copyAction = m_pNoteMenu->addAction(tr("Copy"));
        connect(copyAction, &QAction::triggered, [this]() {
            qDebug() << "Copy note action triggered";
            emit sigListMenuClick(E_NOTE_COPY);
        });

        QAction *delAction = m_pNoteMenu->addAction(tr("Remove annotation"));
        connect(delAction, &QAction::triggered, [this]() {
            qDebug() << "Delete note action triggered";
            emit sigListMenuClick(E_NOTE_DELETE);
        });

        QAction *delAllAction = m_pNoteMenu->addAction(tr("Remove all"));
        connect(delAllAction, &QAction::triggered, [this]() {
            qDebug() << "Delete all notes action triggered";
            emit sigListMenuClick(E_NOTE_DELETE_ALL);
        });
    }
    m_pNoteMenu->exec(point);
}

void SideBarImageListView::showBookMarkMenu(const QPoint &point)
{
    qDebug() << "Showing bookmark menu at:" << point;
    if (m_pBookMarkMenu == nullptr) {
        m_pBookMarkMenu = new DMenu(this);
        m_pBookMarkMenu->setAccessibleName("Menu_BookMark");

        QAction *dltBookMarkAction = m_pBookMarkMenu->addAction(tr("Remove bookmark"));
        connect(dltBookMarkAction, &QAction::triggered, [this]() {
            emit sigListMenuClick(E_BOOKMARK_DELETE);
        });

        QAction *dltAllBookMarkAction = m_pBookMarkMenu->addAction(tr("Remove all"));
        connect(dltAllBookMarkAction, &QAction::triggered, [this]() {
            emit sigListMenuClick(E_BOOKMARK_DELETE_ALL);
        });
    }
    m_pBookMarkMenu->exec(point);
    qDebug() << "Showing bookmark menu end";
}

int  SideBarImageListView::getModelIndexForPageIndex(int pageIndex)
{
    qDebug() << "Getting model index for page index:" << pageIndex;
    const QList<QModelIndex> &indexlst = m_imageModel->getModelIndexForPageIndex(pageIndex);
    if (indexlst.size() > 0) {
        qDebug() << "Found model index:" << indexlst.first().row() << "for page:" << pageIndex;
        return indexlst.first().row();
    }
    qWarning() << "No model index found for page:" << pageIndex;
    return -1;
}

int  SideBarImageListView::getPageIndexForModelIndex(int row)
{
    qDebug() << "Getting page index for model index:" << row;
    return m_imageModel->getPageIndexForModelIndex(row);
}

QModelIndex SideBarImageListView::pageUpIndex()
{
    qDebug() << "Getting page up index";
    return DListView::moveCursor(QAbstractItemView::MovePageUp, Qt::NoModifier);
}

QModelIndex SideBarImageListView::pageDownIndex()
{
    qDebug() << "Getting page down index";
    return DListView::moveCursor(QAbstractItemView::MovePageDown, Qt::NoModifier);
}

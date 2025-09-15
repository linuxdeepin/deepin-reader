// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NotesWidget.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "NotesDelegate.h"
#include "SaveDialog.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "Utils.h"

#include <DPushButton>
#include <DHorizontalLine>
#include <QDebug>

#include <QVBoxLayout>

const int LEFTMINHEIGHT = 80;
NotesWidget::NotesWidget(DocSheet *sheet, DWidget *parent)
    : BaseWidget(parent), m_sheet(sheet)
{
    qDebug() << "NotesWidget created for document:" << (sheet ? sheet->filePath() : "null");
    initWidget();
    qDebug() << "NotesWidget::NotesWidget() - Constructor completed";
}

NotesWidget::~NotesWidget()
{
    // qDebug() << "NotesWidget destroyed";
}


void NotesWidget::initWidget()
{
    qDebug() << "Initializing NotesWidget UI";
    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->setContentsMargins(0, 10, 0, 0);
    pVLayout->setSpacing(0);
    this->setLayout(pVLayout);

    m_pImageListView = new SideBarImageListView(m_sheet, this);
    m_pImageListView->setAccessibleName("View_ImageList");
    m_pImageListView->setListType(E_SideBar::SIDE_NOTE);
    NotesDelegate *imageDelegate = new NotesDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);
    connect(m_pImageListView, SIGNAL(sigListMenuClick(const int &)), SLOT(onListMenuClick(const int &)));
    connect(m_pImageListView, SIGNAL(sigListItemClicked(int)), SLOT(onListItemClicked(int)));

    m_pAddAnnotationBtn = new DPushButton(this);
    m_pAddAnnotationBtn->setObjectName("NotesAddBtn");
    m_pAddAnnotationBtn->setAccessibleName("NotesAdd");
    m_pAddAnnotationBtn->setMinimumWidth(170);
    m_pAddAnnotationBtn->setText(tr("Add annotation"));
    DFontSizeManager::instance()->bind(m_pAddAnnotationBtn, DFontSizeManager::T6);
    connect(m_pAddAnnotationBtn, SIGNAL(clicked()), this, SLOT(onAddAnnotation()));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setContentsMargins(10, 6, 10, 6);
    pVLayout->addWidget(m_pImageListView);

    DHorizontalLine *line = new DHorizontalLine(this);
    line->setAccessibleName("NotesLine");
    pVLayout->addWidget(line);
    pHBoxLayout->addWidget(m_pAddAnnotationBtn);
    pVLayout->addItem(pHBoxLayout);

    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH, LEFTMINHEIGHT));
    qDebug() << "NotesWidget::initWidget() - Widget initialization completed";
}

void NotesWidget::prevPage()
{
    qDebug() << "NotesWidget::prevPage() - Navigating to previous page";
    if (m_sheet.isNull()) {
        qWarning() << "NotesWidget::prevPage() - Sheet is null";
        return;
    }
    int curPage = m_pImageListView->currentIndex().row() - 1;
    if (curPage < 0) {
        qDebug() << "NotesWidget::prevPage() - Already at first page";
        return;
    }
    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
    qDebug() << "NotesWidget::prevPage() - Navigated to page:" << pageIndex;
}

void NotesWidget::pageUp()
{
    qDebug() << "NotesWidget::pageUp() - Starting page up navigation";
    if (m_sheet.isNull()) {
        qWarning() << "NotesWidget::pageUp() - Sheet is null";
        return;
    }

    const QModelIndex &newCurrent = m_pImageListView->pageUpIndex();
    if (!newCurrent.isValid()) {
        qDebug() << "NotesWidget::pageUp() - Invalid current index";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(newCurrent.row());
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(newCurrent);
    qDebug() << "NotesWidget::pageUp() - Navigated to page:" << pageIndex;
}

void NotesWidget::nextPage()
{
    qDebug() << "NotesWidget::nextPage() - Starting next page navigation";
    if (m_sheet.isNull()) {
        qWarning() << "NotesWidget::nextPage() - Sheet is null";
        return;
    }

    int curPage = m_pImageListView->currentIndex().row() + 1;
    if (curPage >= m_pImageListView->model()->rowCount()) {
        qDebug() << "NotesWidget::nextPage() - Already at last page";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
    qDebug() << "NotesWidget::nextPage() - Navigated to page:" << pageIndex;
}

void NotesWidget::pageDown()
{
    qDebug() << "NotesWidget::pageDown() - Starting page down navigation";
    if (m_sheet.isNull()) {
        qWarning() << "NotesWidget::pageDown() - Sheet is null";
        return;
    }

    const QModelIndex &newCurrent = m_pImageListView->pageDownIndex();
    if (!newCurrent.isValid()) {
        qDebug() << "NotesWidget::pageDown() - Invalid current index";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(newCurrent.row());
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(newCurrent);
    qDebug() << "NotesWidget::pageDown() - Navigated to page:" << pageIndex;
}

void NotesWidget::deleteItemByKey()
{
    qDebug() << "NotesWidget::deleteItemByKey() - Deleting selected annotation";
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qDebug() << "NotesWidget::deleteItemByKey() - Removing annotation from page:" << tImagePageInfo.pageIndex;
        m_sheet->removeAnnotation(tImagePageInfo.annotation);
    }
}

void NotesWidget::deleteAllItem()
{
    qDebug() << "NotesWidget::deleteAllItem() - Removing all annotations";
    m_sheet->removeAllAnnotation();
    qDebug() << "NotesWidget::deleteAllItem() - All annotations removed";
}

void NotesWidget::addNoteItem(deepin_reader::Annotation *anno)
{
    qDebug() << "NotesWidget::addNoteItem() - Adding note item";
    if (anno == nullptr || anno->contents().isEmpty()) {
        qWarning() << "Attempt to add invalid annotation";
        return;
    }

    ImagePageInfo_t tImagePageInfo;
    tImagePageInfo.pageIndex = anno->page - 1;
    tImagePageInfo.strcontents = anno->contents();
    tImagePageInfo.annotation = anno;
    m_pImageListView->getImageModel()->insertPageIndex(tImagePageInfo);
    qDebug() << "Added note item for page:" << tImagePageInfo.pageIndex
            << "content:" << tImagePageInfo.strcontents.left(20) + "...";

    int modelIndex = m_pImageListView->getImageModel()->findItemForAnno(anno);
    if (modelIndex >= 0)
        m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(modelIndex, 0));
}

void NotesWidget::deleteNoteItem(deepin_reader::Annotation *anno)
{
    qDebug() << "NotesWidget::deleteNoteItem() - Starting deletion";
    if (!anno) {
        qWarning() << "Attempt to delete null annotation";
        return;
    }
    qDebug() << "Deleting note item for page:" << anno->page - 1;
    m_pImageListView->getImageModel()->removeItemForAnno(anno);
    qDebug() << "NotesWidget::deleteNoteItem() - Deletion completed";
}

void NotesWidget::handleOpenSuccess()
{
    qDebug() << "NotesWidget::handleOpenSuccess() - Starting open success handling";
    if (bIshandOpenSuccess) {
        qDebug() << "NotesWidget::handleOpenSuccess() - Already handled open success";
        return;
    }

    bIshandOpenSuccess = true;
    m_pImageListView->handleOpenSuccess();
    qDebug() << "NotesWidget::handleOpenSuccess() - Open success handling completed";
}

void NotesWidget::onListMenuClick(const int &iAction)
{
    qDebug() << "NotesWidget::onListMenuClick() - List menu action triggered:" << iAction;
    if (iAction == E_NOTE_COPY) {
        qDebug() << "NotesWidget::onListMenuClick() - Copy action selected";
        copyNoteContent();
    } else if (iAction == E_NOTE_DELETE) {
        qDebug() << "NotesWidget::onListMenuClick() - Delete action selected";
        deleteItemByKey();
    } else if (iAction == E_NOTE_DELETE_ALL) {
        qDebug() << "NotesWidget::onListMenuClick() - Delete all action selected";
        int result = SaveDialog::showTipDialog(tr("Are you sure you want to delete all annotations?") ,this);
        if (result == 1) {
            qDebug() << "NotesWidget::onListMenuClick() - User confirmed delete all";
            deleteAllItem();
        }
    }
    qDebug() << "NotesWidget::onListMenuClick() - Menu action completed";
}

void NotesWidget::onListItemClicked(int row)
{
    qDebug() << "List item clicked at row:" << row;
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(row, tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qDebug() << "NotesWidget::onListItemClicked() - Jumping to page:" << tImagePageInfo.pageIndex;
        m_sheet->jumpToHighLight(tImagePageInfo.annotation, tImagePageInfo.pageIndex);
    }
    qDebug() << "NotesWidget::onListItemClicked() - Item click handling completed";
}

void NotesWidget::onAddAnnotation()
{
    qDebug() << "Add annotation button clicked";
    m_sheet->setAnnotationInserting(true);
    qDebug() << "NotesWidget::onAddAnnotation() - Annotation insertion mode enabled";
}

void NotesWidget::handleAnntationMsg(const int &msgType, deepin_reader::Annotation *anno)
{
    qDebug() << "Handling annotation message type:" << msgType;
    if (msgType == MSG_NOTE_ADD) {
        qDebug() << "NotesWidget::handleAnntationMsg() - Adding note item";
        addNoteItem(anno);
    } else if (msgType == MSG_NOTE_DELETE) {
        qDebug() << "NotesWidget::handleAnntationMsg() - Deleting note item";
        deleteNoteItem(anno);
    } else if (msgType == MSG_ALL_NOTE_DELETE) {
        qDebug() << "NotesWidget::handleAnntationMsg() - Deleting all notes";
        m_pImageListView->getImageModel()->resetData();
    }
    qDebug() << "NotesWidget::handleAnntationMsg() - Message handling completed";
}

void NotesWidget::copyNoteContent()
{
    qDebug() << "NotesWidget::copyNoteContent() - Starting copy operation";
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qDebug() << "NotesWidget::copyNoteContent() - Copying content from page:" << tImagePageInfo.pageIndex;
        Utils::copyText(tImagePageInfo.strcontents);
    }
    qDebug() << "NotesWidget::copyNoteContent() - Copy operation completed";
}

void NotesWidget::adaptWindowSize(const double &scale)
{
    qDebug() << "NotesWidget::adaptWindowSize() - Adapting window size with scale:" << scale;
    const QModelIndex &curModelIndex = m_pImageListView->currentIndex();
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), LEFTMINHEIGHT));
    m_pImageListView->reset();
    m_pImageListView->scrollToModelInexPage(curModelIndex, false);
    qDebug() << "NotesWidget::adaptWindowSize() - Window size adaptation completed";
}

void NotesWidget::showMenu()
{
    qDebug() << "NotesWidget::showMenu() - Showing context menu";
    if (m_pImageListView) {
        qDebug() << "NotesWidget::showMenu() - Delegating to image list view";
        m_pImageListView->showMenu();
    }
    qDebug() << "NotesWidget::showMenu() - Menu display completed";
}

void NotesWidget::changeResetModelData()
{
    qDebug() << "NotesWidget::changeResetModelData() - Starting model data reset";
    const QList< deepin_reader::Annotation * > &annotationlst = m_sheet->annotations();
    QList<ImagePageInfo_t> pageSrclst;
    int pagesNum = annotationlst.size();
    qDebug() << "NotesWidget::changeResetModelData() - Processing" << pagesNum << "annotations";
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
    m_pImageListView->getImageModel()->changeModelData(pageSrclst);
    qDebug() << "NotesWidget::changeResetModelData() - Model data reset completed";
}

void NotesWidget::setTabOrderWidget(QList<QWidget *> &tabWidgetlst)
{
    qDebug() << "NotesWidget::setTabOrderWidget() - Setting tab order widget";
    tabWidgetlst << m_pAddAnnotationBtn;
    qDebug() << "NotesWidget::setTabOrderWidget() - Tab order widget set";
}

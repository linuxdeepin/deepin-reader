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
#include "ddlog.h"

#include <DPushButton>
#include <DHorizontalLine>
#include <QDebug>

#include <QVBoxLayout>

const int LEFTMINHEIGHT = 80;
NotesWidget::NotesWidget(DocSheet *sheet, DWidget *parent)
    : BaseWidget(parent), m_sheet(sheet)
{
    qCDebug(appLog) << "NotesWidget created for document:" << (sheet ? sheet->filePath() : "null");
    initWidget();
    qCDebug(appLog) << "NotesWidget::NotesWidget() - Constructor completed";
}

NotesWidget::~NotesWidget()
{
    // qCDebug(appLog) << "NotesWidget destroyed";
}


void NotesWidget::initWidget()
{
    qCDebug(appLog) << "Initializing NotesWidget UI";
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
    qCDebug(appLog) << "NotesWidget::initWidget() - Widget initialization completed";
}

void NotesWidget::prevPage()
{
    qCDebug(appLog) << "NotesWidget::prevPage() - Navigating to previous page";
    if (m_sheet.isNull()) {
        qCWarning(appLog) << "NotesWidget::prevPage() - Sheet is null";
        return;
    }
    int curPage = m_pImageListView->currentIndex().row() - 1;
    if (curPage < 0) {
        qCDebug(appLog) << "NotesWidget::prevPage() - Already at first page";
        return;
    }
    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
    qCDebug(appLog) << "NotesWidget::prevPage() - Navigated to page:" << pageIndex;
}

void NotesWidget::pageUp()
{
    qCDebug(appLog) << "NotesWidget::pageUp() - Starting page up navigation";
    if (m_sheet.isNull()) {
        qCWarning(appLog) << "NotesWidget::pageUp() - Sheet is null";
        return;
    }

    const QModelIndex &newCurrent = m_pImageListView->pageUpIndex();
    if (!newCurrent.isValid()) {
        qCDebug(appLog) << "NotesWidget::pageUp() - Invalid current index";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(newCurrent.row());
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(newCurrent);
    qCDebug(appLog) << "NotesWidget::pageUp() - Navigated to page:" << pageIndex;
}

void NotesWidget::nextPage()
{
    qCDebug(appLog) << "NotesWidget::nextPage() - Starting next page navigation";
    if (m_sheet.isNull()) {
        qCWarning(appLog) << "NotesWidget::nextPage() - Sheet is null";
        return;
    }

    int curPage = m_pImageListView->currentIndex().row() + 1;
    if (curPage >= m_pImageListView->model()->rowCount()) {
        qCDebug(appLog) << "NotesWidget::nextPage() - Already at last page";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
    qCDebug(appLog) << "NotesWidget::nextPage() - Navigated to page:" << pageIndex;
}

void NotesWidget::pageDown()
{
    qCDebug(appLog) << "NotesWidget::pageDown() - Starting page down navigation";
    if (m_sheet.isNull()) {
        qCWarning(appLog) << "NotesWidget::pageDown() - Sheet is null";
        return;
    }

    const QModelIndex &newCurrent = m_pImageListView->pageDownIndex();
    if (!newCurrent.isValid()) {
        qCDebug(appLog) << "NotesWidget::pageDown() - Invalid current index";
        return;
    }

    int pageIndex = m_pImageListView->getPageIndexForModelIndex(newCurrent.row());
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(newCurrent);
    qCDebug(appLog) << "NotesWidget::pageDown() - Navigated to page:" << pageIndex;
}

void NotesWidget::deleteItemByKey()
{
    qCDebug(appLog) << "NotesWidget::deleteItemByKey() - Deleting selected annotation";
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qCDebug(appLog) << "NotesWidget::deleteItemByKey() - Removing annotation from page:" << tImagePageInfo.pageIndex;
        m_sheet->removeAnnotation(tImagePageInfo.annotation);
    }
}

void NotesWidget::deleteAllItem()
{
    qCDebug(appLog) << "NotesWidget::deleteAllItem() - Removing all annotations";
    m_sheet->removeAllAnnotation();
    qCDebug(appLog) << "NotesWidget::deleteAllItem() - All annotations removed";
}

void NotesWidget::addNoteItem(deepin_reader::Annotation *anno)
{
    qCDebug(appLog) << "NotesWidget::addNoteItem() - Adding note item";
    if (anno == nullptr || anno->contents().isEmpty()) {
        qCWarning(appLog) << "Attempt to add invalid annotation";
        return;
    }

    ImagePageInfo_t tImagePageInfo;
    tImagePageInfo.pageIndex = anno->page - 1;
    tImagePageInfo.strcontents = anno->contents();
    tImagePageInfo.annotation = anno;
    m_pImageListView->getImageModel()->insertPageIndex(tImagePageInfo);
    qCDebug(appLog) << "Added note item for page:" << tImagePageInfo.pageIndex
            << "content:" << tImagePageInfo.strcontents.left(20) + "...";

    int modelIndex = m_pImageListView->getImageModel()->findItemForAnno(anno);
    if (modelIndex >= 0)
        m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(modelIndex, 0));
}

void NotesWidget::deleteNoteItem(deepin_reader::Annotation *anno)
{
    qCDebug(appLog) << "NotesWidget::deleteNoteItem() - Starting deletion";
    if (!anno) {
        qCWarning(appLog) << "Attempt to delete null annotation";
        return;
    }
    qCDebug(appLog) << "Deleting note item for page:" << anno->page - 1;
    m_pImageListView->getImageModel()->removeItemForAnno(anno);
    qCDebug(appLog) << "NotesWidget::deleteNoteItem() - Deletion completed";
}

void NotesWidget::handleOpenSuccess()
{
    qCDebug(appLog) << "NotesWidget::handleOpenSuccess() - Starting open success handling";
    if (bIshandOpenSuccess) {
        qCDebug(appLog) << "NotesWidget::handleOpenSuccess() - Already handled open success";
        return;
    }

    bIshandOpenSuccess = true;
    m_pImageListView->handleOpenSuccess();
    qCDebug(appLog) << "NotesWidget::handleOpenSuccess() - Open success handling completed";
}

void NotesWidget::onListMenuClick(const int &iAction)
{
    qCDebug(appLog) << "NotesWidget::onListMenuClick() - List menu action triggered:" << iAction;
    if (iAction == E_NOTE_COPY) {
        qCDebug(appLog) << "NotesWidget::onListMenuClick() - Copy action selected";
        copyNoteContent();
    } else if (iAction == E_NOTE_DELETE) {
        qCDebug(appLog) << "NotesWidget::onListMenuClick() - Delete action selected";
        deleteItemByKey();
    } else if (iAction == E_NOTE_DELETE_ALL) {
        qCDebug(appLog) << "NotesWidget::onListMenuClick() - Delete all action selected";
        int result = SaveDialog::showTipDialog(tr("Are you sure you want to delete all annotations?") ,this);
        if (result == 1) {
            qCDebug(appLog) << "NotesWidget::onListMenuClick() - User confirmed delete all";
            deleteAllItem();
        }
    }
    qCDebug(appLog) << "NotesWidget::onListMenuClick() - Menu action completed";
}

void NotesWidget::onListItemClicked(int row)
{
    qCDebug(appLog) << "List item clicked at row:" << row;
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(row, tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qCDebug(appLog) << "NotesWidget::onListItemClicked() - Jumping to page:" << tImagePageInfo.pageIndex;
        m_sheet->jumpToHighLight(tImagePageInfo.annotation, tImagePageInfo.pageIndex);
    }
    qCDebug(appLog) << "NotesWidget::onListItemClicked() - Item click handling completed";
}

void NotesWidget::onAddAnnotation()
{
    qCDebug(appLog) << "Add annotation button clicked";
    m_sheet->setAnnotationInserting(true);
    qCDebug(appLog) << "NotesWidget::onAddAnnotation() - Annotation insertion mode enabled";
}

void NotesWidget::handleAnntationMsg(const int &msgType, deepin_reader::Annotation *anno)
{
    qCDebug(appLog) << "Handling annotation message type:" << msgType;
    if (msgType == MSG_NOTE_ADD) {
        qCDebug(appLog) << "NotesWidget::handleAnntationMsg() - Adding note item";
        addNoteItem(anno);
    } else if (msgType == MSG_NOTE_DELETE) {
        qCDebug(appLog) << "NotesWidget::handleAnntationMsg() - Deleting note item";
        deleteNoteItem(anno);
    } else if (msgType == MSG_ALL_NOTE_DELETE) {
        qCDebug(appLog) << "NotesWidget::handleAnntationMsg() - Deleting all notes";
        m_pImageListView->getImageModel()->resetData();
    }
    qCDebug(appLog) << "NotesWidget::handleAnntationMsg() - Message handling completed";
}

void NotesWidget::copyNoteContent()
{
    qCDebug(appLog) << "NotesWidget::copyNoteContent() - Starting copy operation";
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        qCDebug(appLog) << "NotesWidget::copyNoteContent() - Copying content from page:" << tImagePageInfo.pageIndex;
        Utils::copyText(tImagePageInfo.strcontents);
    }
    qCDebug(appLog) << "NotesWidget::copyNoteContent() - Copy operation completed";
}

void NotesWidget::adaptWindowSize(const double &scale)
{
    qCDebug(appLog) << "NotesWidget::adaptWindowSize() - Adapting window size with scale:" << scale;
    const QModelIndex &curModelIndex = m_pImageListView->currentIndex();
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), LEFTMINHEIGHT));
    m_pImageListView->reset();
    m_pImageListView->scrollToModelInexPage(curModelIndex, false);
    qCDebug(appLog) << "NotesWidget::adaptWindowSize() - Window size adaptation completed";
}

void NotesWidget::showMenu()
{
    qCDebug(appLog) << "NotesWidget::showMenu() - Showing context menu";
    if (m_pImageListView) {
        qCDebug(appLog) << "NotesWidget::showMenu() - Delegating to image list view";
        m_pImageListView->showMenu();
    }
    qCDebug(appLog) << "NotesWidget::showMenu() - Menu display completed";
}

void NotesWidget::changeResetModelData()
{
    qCDebug(appLog) << "NotesWidget::changeResetModelData() - Starting model data reset";
    const QList< deepin_reader::Annotation * > &annotationlst = m_sheet->annotations();
    QList<ImagePageInfo_t> pageSrclst;
    int pagesNum = annotationlst.size();
    qCDebug(appLog) << "NotesWidget::changeResetModelData() - Processing" << pagesNum << "annotations";
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
    qCDebug(appLog) << "NotesWidget::changeResetModelData() - Model data reset completed";
}

void NotesWidget::setTabOrderWidget(QList<QWidget *> &tabWidgetlst)
{
    qCDebug(appLog) << "NotesWidget::setTabOrderWidget() - Setting tab order widget";
    tabWidgetlst << m_pAddAnnotationBtn;
    qCDebug(appLog) << "NotesWidget::setTabOrderWidget() - Tab order widget set";
}

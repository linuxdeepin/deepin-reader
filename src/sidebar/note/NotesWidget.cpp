/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     leiyu
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
#include "NotesWidget.h"

#include "sidebar/ImageListview.h"
#include "sidebar/ImageViewModel.h"
#include "NotesDelegate.h"
#include "widgets/SaveDialog.h"

#include "DocSheet.h"
#include "MsgHeader.h"


#include <DPushButton>
#include <DHorizontalLine>
#include <QVBoxLayout>

const int LEFTMINHEIGHT = 80;
NotesWidget::NotesWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
}

NotesWidget::~NotesWidget()
{

}

void NotesWidget::initWidget()
{
    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->setContentsMargins(0, 10, 0, 0);
    pVLayout->setSpacing(0);
    this->setLayout(pVLayout);

    m_pImageListView = new ImageListView(m_sheet, this);
    m_pImageListView->setListType(E_SideBar::SIDE_NOTE);
    NotesDelegate *imageDelegate = new NotesDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);
    connect(m_pImageListView, SIGNAL(sigListMenuClick(const int &)), SLOT(onListMenuClick(const int &)));
    connect(m_pImageListView, SIGNAL(sigListItemClicked(int)), SLOT(onListItemClicked(int)));

    m_pAddAnnotationBtn = new DPushButton(this);
    m_pAddAnnotationBtn->setFixedHeight(36);
    m_pAddAnnotationBtn->setMinimumWidth(170);
    m_pAddAnnotationBtn->setText(tr("Add annotation"));
    DFontSizeManager::instance()->bind(m_pAddAnnotationBtn, DFontSizeManager::T6);
    connect(m_pAddAnnotationBtn, SIGNAL(clicked()), this, SLOT(onAddAnnotation()));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setContentsMargins(10, 6, 10, 6);
    pVLayout->addWidget(m_pImageListView);
    pVLayout->addWidget(new DHorizontalLine(this));
    pHBoxLayout->addWidget(m_pAddAnnotationBtn);
    pVLayout->addItem(pHBoxLayout);

    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH /** 1.0*/, LEFTMINHEIGHT/* * 1.0*/));
}

void NotesWidget::prevPage()
{
    if (m_sheet.isNull())
        return;
    int curPage = m_pImageListView->currentIndex().row() - 1;
    if (curPage < 0)
        return;
    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
}

void NotesWidget::nextPage()
{
    if (m_sheet.isNull())
        return;
    int curPage = m_pImageListView->currentIndex().row() + 1;
    if (curPage >= m_pImageListView->model()->rowCount())
        return;
    int pageIndex = m_pImageListView->getPageIndexForModelIndex(curPage);
    m_sheet->jumpToIndex(pageIndex);
    m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(curPage, 0));
}

void NotesWidget::DeleteItemByKey()
{
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        m_sheet->removeAnnotation(tImagePageInfo.annotation);
    }
}

void NotesWidget::deleteAllItem()
{
    int itemsize = m_pImageListView->model()->rowCount();
    for (int i = 0; i < itemsize; i++) {
        ImagePageInfo_t tImagePageInfo;
        m_pImageListView->getImageModel()->getModelIndexImageInfo(i, tImagePageInfo);
        if (tImagePageInfo.pageIndex >= 0) {
            m_sheet->removeAnnotation(tImagePageInfo.annotation);
        }
    }
}

void NotesWidget::addNoteItem(deepin_reader::Annotation *anno)
{
    ImagePageInfo_t tImagePageInfo;
    tImagePageInfo.pageIndex = anno->page - 1;
    tImagePageInfo.strcontents = anno->contents();
    tImagePageInfo.annotation = anno;
    m_pImageListView->getImageModel()->insertPageIndex(tImagePageInfo);

    int modelIndex = m_pImageListView->getImageModel()->findItemForAnno(anno);
    if (modelIndex >= 0)
        m_pImageListView->scrollToModelInexPage(m_pImageListView->model()->index(modelIndex, 0));
}

void NotesWidget::deleteNoteItem(deepin_reader::Annotation *anno)
{
    m_pImageListView->getImageModel()->removeItemForAnno(anno);
}

void NotesWidget::handleOpenSuccess()
{
    if (bIshandOpenSuccess)
        return;
    bIshandOpenSuccess = true;
    m_pImageListView->handleOpenSuccess();
}

void NotesWidget::onListMenuClick(const int &iAction)
{
    if (iAction == E_NOTE_COPY) {
        copyNoteContent();
    } else if (iAction == E_NOTE_DELETE) {
        DeleteItemByKey();
    } else if (iAction == E_NOTE_DELETE_ALL) {
        int result = SaveDialog::showTipDialog(tr("Are you sure you want to delete all notes?"));
        if (result) deleteAllItem();
    }
}

void NotesWidget::onListItemClicked(int row)
{
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(row, tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        m_sheet->jumpToHighLight(tImagePageInfo.annotation);
    }
}

void NotesWidget::onAddAnnotation()
{
    m_sheet->setAnnotationInserting(true);
}

void NotesWidget::handleAnntationMsg(const int &msgType, deepin_reader::Annotation *anno)
{
    if (msgType == MSG_NOTE_ADD) {
        addNoteItem(anno);
    } else if (msgType == MSG_NOTE_DELETE) {
        deleteNoteItem(anno);
    }
}

void NotesWidget::copyNoteContent()
{
    ImagePageInfo_t tImagePageInfo;
    m_pImageListView->getImageModel()->getModelIndexImageInfo(m_pImageListView->currentIndex().row(), tImagePageInfo);
    if (tImagePageInfo.pageIndex >= 0) {
        Utils::copyText(tImagePageInfo.strcontents);
    }
}

void NotesWidget::adaptWindowSize(const double &scale)
{
    const QModelIndex &curModelIndex = m_pImageListView->currentIndex();
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), LEFTMINHEIGHT));
    m_pImageListView->reset();
    m_pImageListView->scrollToModelInexPage(curModelIndex, false);
}

void NotesWidget::updateThumbnail(const int &pageIndex)
{
    m_pImageListView->getImageModel()->updatePageIndex(pageIndex);
}

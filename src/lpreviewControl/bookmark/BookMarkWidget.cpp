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
#include "BookMarkWidget.h"
#include "DocSheet.h"
#include "WidgetHeader.h"
#include "pdfControl/docview/DocummentProxy.h"
#include "lpreviewControl/ImageListview.h"
#include "lpreviewControl/ImageViewModel.h"
#include "BookMarkDelegate.h"
#include "widgets/SaveDialog.h"

#include <DHorizontalLine>

const int LEFTMINHEIGHT = 80;
BookMarkWidget::BookMarkWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
    onUpdateTheme();
}

BookMarkWidget::~BookMarkWidget()
{

}

void BookMarkWidget::initWidget()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &BookMarkWidget::onUpdateTheme);

    m_pImageListView = new ImageListView(m_sheet, this);
    m_pImageListView->setListType(DR_SPACE::E_BOOKMARK_WIDGET);
    BookMarkDelegate *imageDelegate = new BookMarkDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);

    m_pAddBookMarkBtn = new DPushButton(this);
    m_pAddBookMarkBtn->setFixedHeight(36);
    m_pAddBookMarkBtn->setMinimumWidth(170);
    m_pAddBookMarkBtn->setText(tr("Add bookmark"));
    DFontSizeManager::instance()->bind(m_pAddBookMarkBtn, DFontSizeManager::T6);
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(onAddBookMarkClicked()));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setContentsMargins(10, 6, 10, 6);
    pHBoxLayout->addWidget(m_pAddBookMarkBtn);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 10, 0, 0);
    pVBoxLayout->setSpacing(0);

    pVBoxLayout->addWidget(m_pImageListView);
    pVBoxLayout->addWidget(new DHorizontalLine(this));
    pVBoxLayout->addItem(pHBoxLayout);
    this->setLayout(pVBoxLayout);

    connect(m_pImageListView, SIGNAL(sigListMenuClick(const int &)), SLOT(onListMenuClick(const int &)));
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH/* * 1.0*/, LEFTMINHEIGHT/* * 1.0*/));
}


void BookMarkWidget::prevPage()
{
    if (m_sheet.isNull())
        return;
    int curPage = m_pImageListView->currentIndex().row() - 1;
    if (curPage < 0)
        return;
    m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(curPage));
}

void BookMarkWidget::nextPage()
{
    int curPage = m_pImageListView->currentIndex().row() + 1;
    if (curPage >= m_pImageListView->model()->rowCount())
        return;
    m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(curPage));
}

void BookMarkWidget::handleOpenSuccess()
{
    const QSet<int> &pageList = m_sheet->getBookMarkList();
    if (pageList.contains(m_sheet->currentIndex()))
        m_pAddBookMarkBtn->setEnabled(false);
    m_pImageListView->handleOpenSuccess();
}

void BookMarkWidget::handlePage(int index)
{
    bool result = m_pImageListView->scrollToIndex(index);
    m_pAddBookMarkBtn->setDisabled(result);
}

void BookMarkWidget::handleBookMark(int index, int state)
{
    if (state) {
        int nCurIndex = m_sheet->currentIndex();
        if (nCurIndex == index) m_pAddBookMarkBtn->setEnabled(false);
        m_pImageListView->getImageModel()->insertPageIndex(index);
    } else {
        int nCurIndex = m_sheet->currentIndex();
        if (nCurIndex == index) m_pAddBookMarkBtn->setEnabled(true);
        m_pImageListView->getImageModel()->removePageIndex(index);
    }
    m_pImageListView->scrollToIndex(m_sheet->currentIndex(), true);
}

void BookMarkWidget::DeleteItemByKey()
{
    int curIndex = m_pImageListView->getPageIndexForModelIndex(m_pImageListView->currentIndex().row());
    if (curIndex >= 0)
        m_sheet->setBookMark(curIndex, false);
}

void BookMarkWidget::deleteAllItem()
{
    QList<BookMarkStatus_t> bookmarks;
    int itemsize = m_pImageListView->model()->rowCount();
    for (int i = 0; i < itemsize; i++) {
        int curIndex = m_pImageListView->getPageIndexForModelIndex(i);
        if (curIndex >= 0) {
            bookmarks << BookMarkStatus_t(curIndex, false);
        }
    }
    m_sheet->setBookMarks(bookmarks);
}

void BookMarkWidget::onAddBookMarkClicked()
{
    if (m_sheet.isNull()) return;
    int nPage = m_sheet->currentIndex();
    m_sheet->setBookMark(nPage, true);
}

void BookMarkWidget::adaptWindowSize(const double &scale)
{
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), LEFTMINHEIGHT));
    m_pImageListView->reset();
    m_pImageListView->scrollToIndex(m_sheet->currentIndex(), false);
}

void BookMarkWidget::updateThumbnail(const int &index)
{
    m_pImageListView->getImageModel()->updatePageIndex(index);
}

void BookMarkWidget::onUpdateTheme()
{
    DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
    m_pAddBookMarkBtn->setPalette(plt);
}

void BookMarkWidget::onListMenuClick(const int &iType)
{
    if (iType == E_BOOKMARK_DELETE) {
        DeleteItemByKey();
    } else if (iType == E_BOOKMARK_DELETE_ALL) {
        int result = SaveDialog::showTipDialog(tr("Are you sure you want to delete all bookmarks?"));
        if (result) {
            deleteAllItem();
        }
    }
}

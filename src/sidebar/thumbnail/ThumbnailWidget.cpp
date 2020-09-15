/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ThumbnailWidget.h"
#include "DocSheet.h"
#include "Application.h"
#include "PagingWidget.h"
#include "sidebar/ImageListview.h"
#include "sidebar/thumbnail/ThumbnailDelegate.h"
#include "sidebar/ImageViewModel.h"

#include <DHorizontalLine>

#include <QVBoxLayout>

const int LEFTMINHEIGHT = 220;

ThumbnailWidget::ThumbnailWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent)
    , m_sheet(sheet)
{
    initWidget();
}

ThumbnailWidget::~ThumbnailWidget()
{

}

void ThumbnailWidget::initWidget()
{
    m_pImageListView = new ImageListView(m_sheet, this);
    ThumbnailDelegate *imageDelegate = new ThumbnailDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);

    m_pPageWidget = new PagingWidget(m_sheet, this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(m_pImageListView);
    vBoxLayout->addWidget(new DHorizontalLine(this));
    vBoxLayout->addWidget(m_pPageWidget);
    this->setLayout(vBoxLayout);
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH, LEFTMINHEIGHT));
}

void ThumbnailWidget::handleOpenSuccess()
{
    if (bIshandOpenSuccess)
        return;
    bIshandOpenSuccess = true;
    m_pImageListView->handleOpenSuccess();
    m_pPageWidget->handleOpenSuccess();
    scrollToCurrentPage();
}

void ThumbnailWidget::handleRotate()
{
    m_pImageListView->reset();
    scrollToCurrentPage();
}

void ThumbnailWidget::handlePage(int index)
{
    m_pImageListView->scrollToIndex(index);
    m_pPageWidget->setIndex(index);
}

void ThumbnailWidget::setBookMarkState(const int &index, const int &type)
{
    m_pImageListView->getImageModel()->setBookMarkVisible(index, type);
}

void ThumbnailWidget::prevPage()
{
    if (!m_sheet.isNull())
        m_sheet->jumpToPrevPage();
}

void ThumbnailWidget::pageUp()
{
    if (m_sheet.isNull())
        return;

    const QModelIndex &pageIndex = m_pImageListView->pageUpIndex();
    if (!pageIndex.isValid())
        return;

    m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(pageIndex.row()));
}

void ThumbnailWidget::nextPage()
{
    if (!m_sheet.isNull())
        m_sheet->jumpToNextPage();
}

void ThumbnailWidget::pageDown()
{
    if (m_sheet.isNull())
        return;

    const QModelIndex &pageIndex = m_pImageListView->pageDownIndex();
    if (!pageIndex.isValid())
        return;

    m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(pageIndex.row()));
}

void ThumbnailWidget::adaptWindowSize(const double &scale)
{
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), static_cast<int>(qMax(LEFTMINHEIGHT * scale, LEFTMINHEIGHT * 1.0))));
    m_pImageListView->reset();
    scrollToCurrentPage();
}

void ThumbnailWidget::updateThumbnail(const int &index)
{
    m_pImageListView->getImageModel()->updatePageIndex(index);
}

void ThumbnailWidget::scrollToCurrentPage()
{
    m_pImageListView->scrollToIndex(m_sheet->currentIndex());
}

void ThumbnailWidget::setTabOrderWidget(QList<QWidget *> &tabWidgetlst)
{
    m_pPageWidget->setTabOrderWidget(tabWidgetlst);
}

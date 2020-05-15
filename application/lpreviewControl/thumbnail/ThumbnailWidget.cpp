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
#include "ThumbnailWidget.h"
#include "DocSheet.h"
#include "application.h"
#include "PagingWidget.h"
#include "lpreviewControl/ImageListview.h"
#include "pdfControl/docview/docummentproxy.h"
#include "lpreviewControl/thumbnail/ThumbnailDelegate.h"
#include "lpreviewControl/ImageViewModel.h"

#include <QVBoxLayout>
#include <DHorizontalLine>

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
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH * 1.0, LEFTMINHEIGHT * 1.0));
}

void ThumbnailWidget::handleOpenSuccess()
{
    m_pImageListView->handleOpenSuccess();
    m_pPageWidget->handleOpenSuccess();
}

void ThumbnailWidget::handleRotate(int)
{
    m_pImageListView->reset();
    scrollToCurrentPage();
}

void ThumbnailWidget::handlePage(int page)
{
    m_pImageListView->scrollToIndex(page);
    m_pPageWidget->setIndex(page);
}

void ThumbnailWidget::setBookMarkState(const int &pageIndex, const int &type)
{
    m_pImageListView->getImageModel()->setBookMarkVisible(pageIndex, type);
}

void ThumbnailWidget::prevPage()
{
    if (!m_sheet.isNull())
        m_sheet->jumpToPrevPage();
}

void ThumbnailWidget::nextPage()
{
    if (!m_sheet.isNull())
        m_sheet->jumpToNextPage();
}

void ThumbnailWidget::adaptWindowSize(const double &scale)
{
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH * scale, qMax(LEFTMINHEIGHT * scale, LEFTMINHEIGHT * 1.0)));
    m_pImageListView->reset();
    scrollToCurrentPage();
}

void ThumbnailWidget::updateThumbnail(const int &pageIndex)
{
    m_pImageListView->getImageModel()->updatePageIndex(pageIndex);
}

void ThumbnailWidget::scrollToCurrentPage()
{
    m_pImageListView->scrollToIndex(m_sheet->currentIndex());
}

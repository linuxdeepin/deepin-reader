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
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "ThumbnailDelegate.h"

#include <DHorizontalLine>

#include <QVBoxLayout>

const int LEFTMINHEIGHT = 220;

ThumbnailWidget::ThumbnailWidget(DocSheet *sheet, DWidget *parent)
    : BaseWidget(parent)
    , m_sheet(sheet)
{
    initWidget();
}

ThumbnailWidget::~ThumbnailWidget()
{

}

void ThumbnailWidget::initWidget()
{
    m_pImageListView = new SideBarImageListView(m_sheet, this);
    m_pImageListView->setAccessibleName("View_ImageList");
    ThumbnailDelegate *imageDelegate = new ThumbnailDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);

    m_pPageWidget = new PagingWidget(m_sheet, this);
    m_pPageWidget->setAccessibleName("Paging");
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(m_pImageListView);

    DHorizontalLine *line = new DHorizontalLine(this);
    line->setAccessibleName("ThumbnailLine");
    vBoxLayout->addWidget(line);
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

    if(m_pImageListView->currentIndex().row() != 0){
        qDebug() <<"向上翻页";
        const QModelIndex &pageIndex = m_pImageListView->pageUpIndex();
        if (!pageIndex.isValid())
            return;
        qDebug() <<"当前项的索引: " << m_pImageListView->currentIndex().row() ;
        m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(pageIndex.row()));
    }
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

    if(m_pImageListView->currentIndex().row() != m_pImageListView->count()-1){
        const QModelIndex &pageIndex = m_pImageListView->pageDownIndex();
        qDebug() <<"向下翻页";
        if (!pageIndex.isValid())
            return;
        qDebug() <<"当前项的索引: " << m_pImageListView->currentIndex().row() ;
        m_sheet->jumpToIndex(m_pImageListView->getPageIndexForModelIndex(pageIndex.row()));
    }

}

void ThumbnailWidget::adaptWindowSize(const double &scale)
{
    m_pImageListView->setProperty("adaptScale", scale);
//    QList<QModelIndex> indexList = m_pImageListView->getImageModel()->getModelIndexForPageIndex(0);
//    if (indexList.size() > 0) {
//        qreal pixscale = m_pImageListView->property("adaptScale").toDouble();
//        QModelIndex index = m_pImageListView->getImageModel()->getModelIndexForPageIndex(0).first();
//        QSize pageSize = index.data(ImageinfoType_e::IMAGE_PAGE_SIZE).toSize();
//        qDebug() << pageSize;
//        int rotate = index.data(ImageinfoType_e::IMAGE_ROTATE).toInt();
//        if (rotate == 90 || rotate == 270)
//            pageSize = QSize(pageSize.height(), pageSize.width());
//        pageSize.scale(static_cast<int>(174 * pixscale * dApp->devicePixelRatio()), static_cast<int>(174 * pixscale * dApp->devicePixelRatio()), Qt::KeepAspectRatio);
//        QSize setSize = pageSize/dApp->devicePixelRatio();
//        m_pImageListView->setItemSize(QSize(setSize.width(), setSize.height() + 32));
//    }
//  m_pImageListView->setItemSize(QSize(static_cast<int>(LEFTMINWIDTH * scale), static_cast<int>(qMax(LEFTMINHEIGHT * scale, LEFTMINHEIGHT * 1.0))));
    m_pImageListView->reset();
    scrollToCurrentPage();
}

void ThumbnailWidget::scrollToCurrentPage()
{
    m_pImageListView->scrollToIndex(m_sheet->currentIndex());
}

void ThumbnailWidget::setTabOrderWidget(QList<QWidget *> &tabWidgetlst)
{
    m_pPageWidget->setTabOrderWidget(tabWidgetlst);
}

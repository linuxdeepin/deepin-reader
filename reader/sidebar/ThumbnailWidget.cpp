// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThumbnailWidget.h"
#include "DocSheet.h"
#include "Application.h"
#include "PagingWidget.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "ThumbnailDelegate.h"

#include <DHorizontalLine>
#include <QDebug>

#include <QVBoxLayout>

const int LEFTMINHEIGHT = 220;
ThumbnailWidget::ThumbnailWidget(DocSheet *sheet, DWidget *parent)
    : BaseWidget(parent)
    , m_sheet(sheet)
{
    qDebug() << "ThumbnailWidget created for document:" << (sheet ? sheet->filePath() : "null");
    initWidget();
}

ThumbnailWidget::~ThumbnailWidget()
{

}

void ThumbnailWidget::initWidget()
{
    qDebug() << "Initializing ThumbnailWidget UI";
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
    if (bIshandOpenSuccess) {
        qDebug() << "Already handled open success";
        return;
    }
    qDebug() << "Handling document open success";
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
    qDebug() << "Setting bookmark state for page:" << index << "type:" << type;
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
    qDebug() << "Adapting window size with scale:" << scale;
    m_pImageListView->setProperty("adaptScale", scale);
    QSize newSize(static_cast<int>(LEFTMINWIDTH * scale), static_cast<int>(qMax(LEFTMINHEIGHT * scale, LEFTMINHEIGHT * 1.0)));
    qDebug() << "Setting new item size:" << newSize;
    m_pImageListView->setItemSize(newSize);
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

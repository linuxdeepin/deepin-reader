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
#include "SearchResWidget.h"
#include "DocSheet.h"
#include "pdfControl/docview/docummentproxy.h"
#include "pdfControl/imagelistview.h"
#include "pdfControl/imageviewmodel.h"
#include "WidgetHeader.h"
#include "searchresdelegate.h"

#include <QStackedLayout>

const int SEARCH_INDEX = 0;
const int TIPS_INDEX = 1;
const int LEFTMINHEIGHT = 80;
SearchResWidget::SearchResWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
}

SearchResWidget::~SearchResWidget()
{

}

void SearchResWidget::initWidget()
{
    m_stackLayout = new QStackedLayout;
    m_stackLayout->setContentsMargins(0, 8, 0, 0);
    m_stackLayout->setSpacing(0);
    this->setLayout(m_stackLayout);

    m_pImageListView = new ImageListView(m_sheet, this);
    m_pImageListView->setListType(DR_SPACE::E_SEARCH_RES_WIDGET);
    SearchResDelegate* imageDelegate = new SearchResDelegate(m_pImageListView);
    m_pImageListView->setItemDelegate(imageDelegate);
    m_stackLayout->addWidget(m_pImageListView);

    DLabel *tipLab = new DLabel(tr("No search results"));
    tipLab->setForegroundRole(QPalette::ToolTipText);
    tipLab->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLab, DFontSizeManager::T6);
    m_stackLayout->addWidget(tipLab);
    m_stackLayout->setCurrentIndex(SEARCH_INDEX);
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH * 1.0, LEFTMINHEIGHT * 1.0));
}

void SearchResWidget::handFindContentComming(const stSearchRes &search)
{
    QString strText;
    for (const QString &s : search.listtext) {
        strText += s.trimmed();
        strText += QString("    ");
    }
    addSearchsItem(search.ipage, strText, search.listtext.size());
}

int  SearchResWidget::handleFindFinished()
{
    int searchCount = m_pImageListView->model()->rowCount();
    if(searchCount <= 0)
        m_stackLayout->setCurrentIndex(TIPS_INDEX);
    return searchCount;
}

void SearchResWidget::clearFindResult()
{
    m_stackLayout->setCurrentIndex(SEARCH_INDEX);
    m_pImageListView->getImageModel()->resetData();
}

void SearchResWidget::handleRotate(int)
{
    //not Todo
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    if (nullptr == m_sheet)
        return;
    ImagePageInfo_t tImagePageInfo;
    tImagePageInfo.iPage = page;
    tImagePageInfo.struuid = QString::number(page);
    tImagePageInfo.strcontents = text;
    tImagePageInfo.strSearchcount = tr("%1 items found").arg(resultNum);
    m_pImageListView->getImageModel()->insertPageIndex(tImagePageInfo);
}

void SearchResWidget::adaptWindowSize(const double &scale)
{
    const QModelIndex &curModelIndex = m_pImageListView->currentIndex();
    m_pImageListView->setProperty("adaptScale", scale);
    m_pImageListView->setItemSize(QSize(LEFTMINWIDTH * scale, qMax(LEFTMINHEIGHT * scale, LEFTMINHEIGHT * 1.0)));
    m_pImageListView->reset();
    m_pImageListView->scrollToModelInexPage(curModelIndex, false);
}

void SearchResWidget::updateThumbnail(const int &page)
{
    m_pImageListView->getImageModel()->updatePageIndex(page);
}

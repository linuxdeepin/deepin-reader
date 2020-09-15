/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#include "CatalogWidget.h"
#include "CatalogTreeView.h"
#include "DocSheet.h"

#include <QVBoxLayout>

CatalogWidget::CatalogWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
}

CatalogWidget::~CatalogWidget()
{

}

void CatalogWidget::initWidget()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;

    titleLayout->setSpacing(0);

    titleLayout->setContentsMargins(10, 0, 10, 0);

    titleLabel = new DLabel("", this);

    titleLabel->setForegroundRole(DPalette::TextTips);

    titleLabel->setAlignment(Qt::AlignCenter);

    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T8);

    titleLayout->addWidget(titleLabel);

    auto mainLayout = new QVBoxLayout;

    mainLayout->addLayout(titleLayout);

    m_pTree = new CatalogTreeView(m_sheet, this);
    mainLayout->addWidget(m_pTree);
    this->setLayout(mainLayout);
}

void CatalogWidget::resizeEvent(QResizeEvent *event)
{
    if (m_strTheme != "" && titleLabel) {
        setTitleTheme();
    }

    CustomWidget::resizeEvent(event);
}

void CatalogWidget::setTitleTheme()
{
    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);

    QFontMetrics fm(font);

    QString sTheme = fm.elidedText(m_strTheme, Qt::ElideMiddle, this->width() - 40, Qt::TextSingleLine);

    titleLabel->setText(sTheme);

}

void CatalogWidget::handleOpenSuccess()
{
    if (nullptr == m_sheet || bIshandOpenSuccess)
        return;

    bIshandOpenSuccess = true;
    deepin_reader::FileInfo fileInfo;
    m_sheet->docBasicInfo(fileInfo);
    m_strTheme = fileInfo.theme;
    if (m_strTheme != "") {
        setTitleTheme();
    }
    m_pTree->handleOpenSuccess();
}

void CatalogWidget::handlePage(int index)
{
    m_pTree->setIndex(index);
    m_pTree->setRightControl(true);
}

void CatalogWidget::nextPage()
{
    m_pTree->nextPage();
}

void CatalogWidget::prevPage()
{
    m_pTree->prevPage();
}

void CatalogWidget::pageDown()
{
    m_pTree->pageDownPage();
}

void CatalogWidget::pageUp()
{
    m_pTree->pageUpPage();
}

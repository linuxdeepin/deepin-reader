/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#include <QVBoxLayout>

#include "CatalogTreeView.h"
#include "CustomControl/CustomClickLabel.h"
#include "pdfControl/docview/docummentproxy.h"
#include "DocSheet.h"

CatalogWidget::CatalogWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
    initConnections();
}

CatalogWidget::~CatalogWidget()
{

}

void CatalogWidget::initWidget()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;

    titleLayout->setSpacing(0);

    titleLayout->setContentsMargins(10, 0, 10, 0);

    titleLabel = new CustomClickLabel("", this);

    titleLabel->setForegroundRole(DPalette::TextTips);

    titleLabel->setAlignment(Qt::AlignCenter);

    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T8);

    titleLayout->addWidget(titleLabel);

    auto mainLayout = new QVBoxLayout;

    mainLayout->addItem(titleLayout);

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

void CatalogWidget::initConnections()
{
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
    stFileInfo fileInfo;
    m_sheet->docBasicInfo(fileInfo);
    m_strTheme = fileInfo.strTheme;
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

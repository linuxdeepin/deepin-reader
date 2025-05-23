// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CatalogWidget.h"
#include "CatalogTreeView.h"
#include "DocSheet.h"

#include <QVBoxLayout>

#include <DFontSizeManager>

CatalogWidget::CatalogWidget(DocSheet *sheet, DWidget *parent)
    : BaseWidget(parent), m_sheet(sheet)
{
    qDebug() << "Creating CatalogWidget for document:" << (sheet ? sheet->filePath() : "null");

    initWidget();
}

CatalogWidget::~CatalogWidget()
{
    qDebug() << "Destroying CatalogWidget";
}

void CatalogWidget::initWidget()
{
    qDebug() << "Initializing CatalogWidget UI components";

    QHBoxLayout *titleLayout = new QHBoxLayout;

    titleLayout->setSpacing(0);

    titleLayout->setContentsMargins(10, 0, 10, 0);

    titleLabel = new DLabel("", this);

    titleLabel->setAccessibleName("Label_title");

    titleLabel->setForegroundRole(DPalette::TextTips);

    titleLabel->setAlignment(Qt::AlignCenter);

    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T8);

    titleLayout->addWidget(titleLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addLayout(titleLayout);

    m_pTree = new CatalogTreeView(m_sheet, this);
    m_pTree->setAccessibleName("View_CatalogTree");

    mainLayout->addWidget(m_pTree);
    this->setLayout(mainLayout);
}

void CatalogWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "Handling CatalogWidget resize event";

    if (m_strTheme != "" && titleLabel) {
        setTitleTheme();
    }

    BaseWidget::resizeEvent(event);
}

void CatalogWidget::setTitleTheme()
{
    qDebug() << "Updating catalog title theme:" << m_strTheme << "with width:" << this->width();

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);

    QFontMetrics fm(font);

    QString sTheme = fm.elidedText(m_strTheme, Qt::ElideMiddle, this->width() - 40, Qt::TextSingleLine);

    titleLabel->setText(sTheme);

}

void CatalogWidget::handleOpenSuccess()
{
    qDebug() << "Handling document open success, loading catalog";

    if (nullptr == m_sheet) {
        qCritical() << "Cannot load catalog - document sheet is null";
        return;
    }
    if (bIshandOpenSuccess) {
        qDebug() << "Catalog already loaded, skipping";
        return;
    }

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
    qDebug() << "Handling page navigation to index:" << index;

    m_pTree->setIndex(index);
    m_pTree->setRightControl(true);
}

void CatalogWidget::nextPage()
{
    qDebug() << "Navigating to next catalog item";

    m_pTree->nextPage();
}

void CatalogWidget::prevPage()
{
    qDebug() << "Navigating to previous catalog item";

    m_pTree->prevPage();
}

void CatalogWidget::pageDown()
{
    qDebug() << "Performing page down navigation in catalog";

    m_pTree->pageDownPage();
}

void CatalogWidget::pageUp()
{
    qDebug() << "Performing page up navigation in catalog";

    m_pTree->pageUpPage();
}

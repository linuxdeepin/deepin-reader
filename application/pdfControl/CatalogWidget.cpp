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

#include "catalog/CatalogTreeView.h"

#include "CustomControl/CustomClickLabel.h"

CatalogWidget::CatalogWidget(DWidget *parent)
    : CustomWidget("CatalogWidget", parent)
{
    initWidget();
    initConnections();

    m_pMsgList = { MSG_CATALOG_FILE_TITLE };

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

CatalogWidget::~CatalogWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

int CatalogWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

void CatalogWidget::initWidget()
{
    titleLabel = new CustomClickLabel("");
    titleLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T8);

    auto mainLayout = new QVBoxLayout;

    mainLayout->addWidget(titleLabel);

    auto tree = new CatalogTreeView(this);

    auto pModel = new QStandardItemModel;
    pModel->setColumnCount(3);
    tree->setModel(pModel);

    mainLayout->addWidget(tree);

    this->setLayout(mainLayout);
}

void CatalogWidget::initConnections()
{
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
}

void CatalogWidget::SlotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_CATALOG_FILE_TITLE) {
        setCatalogTitle(msgContent);
    }
}

void CatalogWidget::setCatalogTitle(const QString &msgContent)
{
    if (titleLabel) {
        titleLabel->setText(msgContent);
    }
}

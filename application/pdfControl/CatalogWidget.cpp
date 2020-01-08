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

#include <DLabel>
#include <QVBoxLayout>
#include <DTreeWidget>
#include <QHeaderView>
#include "catalog/CatalogTreeView.h"

CatalogWidget::CatalogWidget(DWidget *parent)
    : CustomWidget("CatalogWidget", parent)
{
    initWidget();

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

int CatalogWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void CatalogWidget::initWidget()
{
    auto titleLabel = new DLabel("XXXXXXXX");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);

    auto tree = new CatalogTreeView(1);
//    tree->addRootItem("aasd", "1");
//    tree->setFrameShape(QFrame::NoFrame);
//    tree->header()->setHidden(true);

//    auto rootItem = new QTreeWidgetItem;
//    rootItem->setText(0, "123441");
//    tree->addTopLevelItem(rootItem);

//    for (int i = 0; i < 10; i++) {
//        auto item = new QTreeWidgetItem;
//        item->setText(0, QString("%1").arg(i));
//        rootItem->addChild(item);
//    }

    mainLayout->addWidget(tree);
    this->setLayout(mainLayout);
}

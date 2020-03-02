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
#include "CustomControl/DFMGlobal.h"
#include "docview/docummentproxy.h"

#include "widgets/main/MainTabWidgetEx.h"

CatalogWidget::CatalogWidget(DWidget *parent)
    : CustomWidget("CatalogWidget", parent)
{
    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

CatalogWidget::~CatalogWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

int CatalogWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    return 0;
}

int CatalogWidget::qDealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        OnDocOpenFileOk(msgContent);
    }

    m_pTree->qDealWithData(msgType, msgContent);

    int nRes = MSG_NO_OK;
    return nRes;
}

void CatalogWidget::initWidget()
{
    titleLabel = new CustomClickLabel("", this);
    titleLabel->setForegroundRole(DPalette::TextTips);
    titleLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T8);

    auto mainLayout = new QVBoxLayout;

    mainLayout->addWidget(titleLabel);

    m_pTree = new CatalogTreeView(this);

    auto pModel = new QStandardItemModel;
    pModel->setColumnCount(3);
    m_pTree->setModel(pModel);

    mainLayout->addWidget(m_pTree);

    this->setLayout(mainLayout);
}

void CatalogWidget::initConnections()
{
}

void CatalogWidget::OnDocOpenFileOk(const QString &sPath)
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    DocummentProxy *_pProxy = pMtwe->getCurFileAndProxy(sPath);
    if (_pProxy) {

        stFileInfo fileInfo;
        _pProxy->docBasicInfo(fileInfo);

        QString strTheme =  fileInfo.strTheme;
        if (strTheme == "") {
            strTheme = tr("Unknown");
        }

        if (titleLabel) {
            QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QString t = DFMGlobal::elideText(strTheme, QSize(120, 18), QTextOption::WrapAnywhere, font, Qt::ElideMiddle, 0);
            QStringList labelTexts = t.split("\n");
            if (labelTexts.size() < 3) {
                titleLabel->setText(strTheme);
            } else {
                QString sStart = labelTexts.at(0);
                QString sEnd = labelTexts.at(labelTexts.size() - 1);

                titleLabel->setText(sStart + "..." + sEnd);
            }
        }
    }
}


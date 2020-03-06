/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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

#include "SearchItemWidget.h"

#include "docview/docummentproxy.h"

#include "widgets/main/MainTabWidgetEx.h"

SearchResWidget::SearchResWidget(DWidget *parent)
    : CustomWidget(SEARCH_RES_WIDGET, parent)
{
    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

SearchResWidget::~SearchResWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//  查询有数据, 则填充
void SearchResWidget::slotGetSearchContant(const stSearchRes &search)
{
    int resultNum = 0;
    QString strText = "";

    int page = search.ipage;
    foreach (QString s, search.listtext) {
        strText += s.trimmed();
        strText += QString("    ");

        ++resultNum;
    }

    addSearchsItem(page, strText, resultNum);
}

void SearchResWidget::slotSearchOver()
{
    if (m_pSearchList->count() == 0) {      //  无结果
        QJsonObject obj;
        obj.insert("to", E_MAIN_TAB_WIDGET + Constant::sQStringSep + E_DOC_SHOW_SHELL_WIDGET + Constant::sQStringSep + E_FIND_WIDGET);

        QJsonDocument doc(obj);

        notifyMsg(MSG_FIND_NONE_CONTENT, doc.toJson(QJsonDocument::Compact));
        showTips();
    }
}

void SearchResWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);
}

//  打开成功之后, 链接搜索信号
void SearchResWidget::OnOpenFileOk(const QString &sPath)
{
    DocummentProxy *_proxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(sPath);
    if (_proxy) {
        connect(_proxy, SIGNAL(signal_searchRes(stSearchRes)), SLOT(slotGetSearchContant(const stSearchRes &)));
        connect(_proxy, SIGNAL(signal_searchover()), SLOT(slotSearchOver()));
    }
}

void SearchResWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 8, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pSearchList = new CustomListWidget;
    connect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), SLOT(slotSelectItem(QListWidgetItem *)));

    m_pVLayout->addWidget(m_pSearchList);
}

void SearchResWidget::initConnections()
{
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    auto item = m_pSearchList->insertWidgetItem(page);
    if (item) {
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(LEFTMINWIDTH, 80));

        auto itemWidget = new SearchItemWidget(this);

        itemWidget->setLabelPage(page, 1);
        itemWidget->setTextEditText(text);
        itemWidget->setSerchResultText(tr("%1 items found").arg(resultNum));
        itemWidget->setMinimumSize(QSize(LEFTMINWIDTH - 5, 80));

        auto dproxy = MainTabWidgetEx::Instance()->getCurFileAndProxy();
        if (nullptr != dproxy) {
            QImage image;
            bool bl = dproxy->getImage(page, image, 48, 68 /*42, 62*/);
            if (bl) {
                itemWidget->setLabelImage(image);
            }
        }

        m_pSearchList->setItemWidget(item, itemWidget);
    }
}

void SearchResWidget::showTips()
{
    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto tipWidget = new DWidget;
    auto tipLab = new DLabel(tr("No search results"));
    tipLab->setForegroundRole(/*DPalette::TextTips*/ QPalette::ToolTipText);
    DFontSizeManager::instance()->bind(tipLab, DFontSizeManager::T6);

    tipWidget->setMinimumSize(QSize(226, 528));

    hLayout->addStretch(1);
    hLayout->addWidget(tipLab);
    hLayout->addStretch(1);
    vLayout->addStretch(1);
    vLayout->addItem(hLayout);
    vLayout->addStretch(1);
    tipWidget->setLayout(vLayout);

    auto item = new QListWidgetItem;
    item->setSizeHint(QSize(226, 528));
    item->setFlags(Qt::NoItemFlags);
    m_pSearchList->addItem(item);
    m_pSearchList->setItemWidget(item, tipWidget);
}

void SearchResWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    clearItemColor();

    m_pSearchList->setCurrentItem(item);

    auto t_widget = reinterpret_cast<SearchItemWidget *>(m_pSearchList->itemWidget(item));
    if (t_widget) {
        t_widget->setBSelect(true);
    }
}

void SearchResWidget::clearItemColor()
{
    if (m_pSearchList == nullptr)
        return;
    auto pCurItem = m_pSearchList->currentItem();
    if (pCurItem) {
        auto pItemWidget =
            reinterpret_cast<SearchItemWidget *>(m_pSearchList->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

/**
 * @brief SearchResWidget::dealWithData
 * 处理全局信号函数
 * @param msgType
 * @param msgContent
 * @return
 */
int SearchResWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        OnOpenFileOk(msgContent);
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

void SearchResWidget::OnExitSearch()
{
    m_pSearchList->clear();
}

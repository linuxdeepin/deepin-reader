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

#include "controller/DataManager.h"
#include "widgets/LeftSidebarWidget.h"
#include "docview/docummentproxy.h"

SearchResWidget::SearchResWidget(DWidget *parent)
    : CustomWidget(QString("SearchResWidget"), parent)
{
    m_loadSearchResThread.setSearchResW(this);

    m_pMsgList = {MSG_CLEAR_FIND_CONTENT};

    initWidget();
    initConnections();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

SearchResWidget::~SearchResWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }

    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }
}

void SearchResWidget::SlotDealWithData(const int &msgType, const QString &)
{
    if (MSG_CLEAR_FIND_CONTENT == msgType) {
        __ClearSearchContent();
    }
}

void SearchResWidget::__ClearSearchContent()
{
    m_isSearch = false;

    m_bShowList = false;
    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }
    if (m_pSearchList->count() > 0) {
        DocummentProxy *_proxy = DocummentProxy::instance();
        if (_proxy) {
            _proxy->clearsearch();
        }

        m_pSearchList->clear();
    }

    bool bShowThunmb = DataManager::instance()->bThumbnIsShow();
    if (!bShowThunmb) {
        //  侧边栏 隐藏
        notifyMsg(MSG_HIDE_FIND_WIDGET);
    } else {
        notifyMsg(MSG_FIND_EXIT);       //  查询结束
    }
}

//  关闭应用
void SearchResWidget::slotCloseFile()
{
    m_bShowList = false;

    if (m_pSearchList->count() > 0) {
        DocummentProxy *_proxy = DocummentProxy::instance();
        if (_proxy) {
            _proxy->clearsearch();
        }

        m_pSearchList->clear();
    }
}

void SearchResWidget::slotFlushSearchWidget(const QString &msgContent)
{
    m_isSearch = true;  //  开始搜索,  标志位 为 true

    notifyMsg(MSG_SWITCHLEFTWIDGET, QString::number(WIDGET_BUFFER));    //  窗口 显示 转圈圈
    notifyMsg(MSG_SLIDER_SHOW_STATE, QString::number(1));
    m_bShowList = true;

    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        connect(_proxy, SIGNAL(signal_searchRes(stSearchRes)), this, SLOT(slotGetSearchContant(stSearchRes)));

        QMap<int, stSearchRes> resMap;
        _proxy->search(msgContent, resMap, Qt::red);

        disconnect(_proxy, SIGNAL(signal_searchover()), this, SLOT(slotSearchOver()));
        connect(_proxy, SIGNAL(signal_searchover()), this, SLOT(slotSearchOver()));
    }
}

void SearchResWidget::slotGetSearchContant(stSearchRes search)
{
    if (search.listtext.size() < 1) {
        return;
    }
    m_loadSearchResThread.pushSearch(search);
}

void SearchResWidget::slotSearchOver()
{
    if (!m_isSearch) {      //  不搜索了, 不要搜索结果了
        return;
    }

    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }

    if (m_pSearchList == nullptr)
        return;

    m_pSearchList->clear();

    disconnect(DocummentProxy::instance(), SIGNAL(signal_searchRes(stSearchRes)), this,
               SLOT(slotGetSearchContant(stSearchRes)));

    QList<stSearchRes> list = m_loadSearchResThread.searchList();

    if (list.size() <= 0) {
//        disconnect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), this,
//                   SLOT(slotSelectItem(QListWidgetItem *)));
        showTips();

        //显示无结果窗口
        notifyMsg(MSG_SWITCHLEFTWIDGET, QString::number(WIDGET_SEARCH));
        //让搜索框变粉红色
        notifyMsg(MSG_FIND_NONE);
        //发送提示消息
        // notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("No search results") + ConstantMsg::g_warningtip_suffix);
        bool t_bTnumbnIsShow = DataManager::instance()->bThumbnIsShow();
        if (!t_bTnumbnIsShow) {
            notifyMsg(MSG_SLIDER_SHOW_STATE, QString::number(!t_bTnumbnIsShow));
        }
    } else {
//        connect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), this,
//                SLOT(slotSelectItem(QListWidgetItem *)));
        //让搜索框回复正常
//        notifyMsg(MSG_FIND_NONE, "0");
        //生成左侧搜索列表
        // to do and send flush thumbnail and contant
        initSearchList(list);
    }
}

void SearchResWidget::slotLoadImage(const int &page, const QImage &image)
{
    if (m_pSearchItemWidget) {
        if (page == m_pSearchItemWidget->nPageIndex()) {
            m_pSearchItemWidget->setLabelImage(image);
        }
    }
}

void SearchResWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);
}

void SearchResWidget::slotStopFind()
{
//    notifyMsg(MSG_FIND_STOP);
    if (m_bShowList) {
        notifyMsg(MSG_SWITCHLEFTWIDGET, QString::number(WIDGET_SEARCH));

        bool t_bTnumbnIsShow = DataManager::instance()->bThumbnIsShow();
        if (!t_bTnumbnIsShow) {
            notifyMsg(MSG_SLIDER_SHOW_STATE, QString::number(!t_bTnumbnIsShow));
        }
    }
}

void SearchResWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 8, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pSearchList = new CustomListWidget;

    m_pVLayout->addWidget(m_pSearchList);
}

void SearchResWidget::initConnections()
{
    connect(&m_loadSearchResThread, SIGNAL(sigLoadImage(const int &, const QImage &)), this,
            SLOT(slotLoadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(SlotDealWithData(const int &, const QString &)));

//    connect(this, SIGNAL(sigClearWidget()), this, SLOT(slotClearWidget()));
    connect(this, SIGNAL(sigFlushSearchWidget(const QString &)), this,
            SLOT(slotFlushSearchWidget(const QString &)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
//    connect(this, SIGNAL(sigFindPrev()), this, SLOT(slotFindPrev()));
//    connect(this, SIGNAL(sigFindNext()), this, SLOT(slotFindNext()));
    connect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), this,
            SLOT(slotSelectItem(QListWidgetItem *)));
    connect(&m_loadSearchResThread, &LoadSearchResThread::sigStopFind, this,
            &SearchResWidget::slotStopFind);
}

void SearchResWidget::initSearchList(const QList<stSearchRes> &list)
{
    int resultNum = 0;
    QString strText = "";

    foreach (stSearchRes it, list) {
        int page = static_cast<int>(it.ipage);
        foreach (QString s, it.listtext) {
            strText += s.trimmed();
            strText += QString("    ");

            ++resultNum;
        }

        addSearchsItem(page, strText, resultNum);
        resultNum = 0;
        strText.clear();
    }

    //开始填充缩略图线程
    m_loadSearchResThread.setRunning(true);
    m_loadSearchResThread.start();
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    auto item = m_pSearchList->insertWidgetItem(page);
    if (item) {
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(LEFTMINWIDTH, 80));

        auto itemWidget = new SearchItemWidget(this);
//        itemWidget->setNoteSigne(false);
        itemWidget->setLabelPage(page, 1);
        itemWidget->setTextEditText(text);
        itemWidget->setSerchResultText(tr("%1 items found").arg(resultNum));
        itemWidget->setMinimumSize(QSize(LEFTMINWIDTH - 5, 80));

//    auto item = new QListWidgetItem(m_pSearchList);
//    item->setFlags(Qt::NoItemFlags);
//    item->setSizeHint(QSize(LEFTMINWIDTH, 80));

//        m_pSearchList->addItem(item);
        m_pSearchList->setItemWidget(item, itemWidget);
    }
}

void SearchResWidget::showTips()
{
    if (m_pSearchList == nullptr) {
        return;
    }

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
//        m_pSearchItem = item;
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
//    if (msgType == MSG_FIND_PREV) {
//        emit sigFindPrev();
//        return ConstantMsg::g_effective_res;
//    }

//    if (msgType == MSG_FIND_NEXT) {
//        emit sigFindNext();
//        return ConstantMsg::g_effective_res;
//    }

    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return  ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_FIND_START) {  //  查询内容
        if (msgContent != QString("")) {
            emit sigFlushSearchWidget(msgContent);
        }
    }  else if (MSG_CLOSE_FILE == msgType) {  //  关闭w文件通知消息
        emit sigCloseFile();
    }

    return 0;
}

/**
 * @brief SearchResWidget::getSearchPage
 * 得到当前搜索页码
 * @param index
 * @return
 */
int SearchResWidget::getSearchPage(const int &index)
{
    auto pItem = m_pSearchList->item(index);
    if (pItem) {
        m_pSearchItemWidget = reinterpret_cast<SearchItemWidget *>(m_pSearchList->itemWidget(pItem));
        if (m_pSearchItemWidget) {
            return m_pSearchItemWidget->nPageIndex();
        }
    }
    return -1;
}

/************************************LoadSearchResList*******************************************************/
/************************************加载搜索列表缩略图*********************************************************/

LoadSearchResThread::LoadSearchResThread(QObject *parent)
    : QThread(parent)
{
}

/**
 * @brief LoadSearchResThread::stopThread
 * 停止线程
 */
void LoadSearchResThread::stopThread()
{
    m_isRunning = false;

    quit();
    wait();  //阻塞等待
}

/**
 * @brief LoadSearchResThread::run
 * 线程主工作区
 */
void LoadSearchResThread::run()
{
    m_pages = m_searchContantList.count();
    m_searchContantList.clear();

    int m_nStartIndex = 0;
    int m_nEndIndex = 19;
    bool t_bSendMSG = true;

    while (m_isRunning) {
        msleep(50);
        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_pages) {
            m_nEndIndex = m_pages - 1;
        }

        int page = -1;

        if (!m_pSearchResWidget) {
            break;
        }
        auto dproxy = DocummentProxy::instance();
        if (nullptr == dproxy) {
            break;
        }

        for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {
            if (!m_isRunning) {
                break;
            }
            page = m_pSearchResWidget->getSearchPage(index);

            if (page == -1) {
                continue;
            }

            QImage image;
            bool bl = dproxy->getImage(page, image, 48, 68 /*42, 62*/);
            if (bl) {
                emit sigLoadImage(page, image);
                msleep(50);
            }
        }

        if ((m_nEndIndex >= m_pages - 1) && t_bSendMSG) {
            emit sigStopFind();
            t_bSendMSG = false;
        }

        if ((m_nEndIndex <= FIRST_LOAD_PAGES) && t_bSendMSG) {
            emit sigStopFind();
            t_bSendMSG = false;
        }

        if (m_nEndIndex >= m_pages - 1) {
            m_isRunning = false;
            break;
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex += FIRST_LOAD_PAGES;
    }

    if (t_bSendMSG) {
        emit sigStopFind();
        t_bSendMSG = false;
    }
}

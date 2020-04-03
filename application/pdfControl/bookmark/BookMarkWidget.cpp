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
#include "BookMarkWidget.h"

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"
#include "DocSheet.h"
#include "CentralDocPage.h"

BookMarkWidget::BookMarkWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(BOOKMARK_WIDGET, parent), m_sheet(sheet)
{
    setFocusPolicy(Qt::ClickFocus);

    m_loadBookMarkThread.setBookMark(this);

    initWidget();
    initConnection();
    slotUpdateTheme();

    m_pMsgList = {MSG_OPERATION_DELETE_BOOKMARK, MSG_OPERATION_ADD_BOOKMARK};

    m_pKeyMsgList = {KeyStr::g_ctrl_b};

    dApp->m_pModelService->addObserver(this);
}
/**
 * @brief BookMarkWidget::~BookMarkWidget
 * 等待子线程退出
 */
BookMarkWidget::~BookMarkWidget()
{
    m_loadBookMarkThread.stopThreadRun();
    dApp->m_pModelService->removeObserver(this);
}

/**
 * @brief BookMarkWidget::prevPage
 * 上一页
 */
void BookMarkWidget::prevPage()
{
    if (m_pBookMarkListWidget == nullptr) {
        return;
    }

    if (m_pBookMarkListWidget->count() <= 0) {
        return;
    }

    int nCurIndex  = m_pBookMarkListWidget->currentRow();
    nCurIndex--;
    if (nCurIndex < 0)
        return;
    auto pCurItem = m_pBookMarkListWidget->item(nCurIndex);
    if (pCurItem != nullptr) {
        auto t_widget = qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (t_widget) {
            if (nullptr != m_sheet)
                m_sheet->pageJump(t_widget->nPageIndex());
        }
    }
}

/**
 * @brief BookMarkWidget::nextPage
 * 下一页
 */
void BookMarkWidget::nextPage()
{
    if (m_pBookMarkListWidget->count() <= 0) {
        return;
    }

    int curIndex = m_pBookMarkListWidget->currentRow();
    curIndex++;
    if (curIndex >= m_pBookMarkListWidget->count())
        return;
    auto pCurItem = m_pBookMarkListWidget->item(curIndex);
    if (pCurItem) {
        auto t_widget = qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (t_widget) {
            if (nullptr != m_sheet)
                m_sheet->pageJump(t_widget->nPageIndex());
        }
    }
}

void BookMarkWidget::DeleteItemByKey()
{
    auto pCurItem = m_pBookMarkListWidget->currentItem();
    if (pCurItem) {
        auto t_widget =
            qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (t_widget) {
            int nPageIndex = t_widget->nPageIndex();

            slotDeleteBookItem(QString::number(nPageIndex));
        }
    }
}

/**
 * @brief BookMarkWidget::slotAddBookMark
 * @brief 点击按钮添加 当前页 为 书签
 */
void BookMarkWidget::slotAddBookMark()
{
    if (!m_pAddBookMarkBtn->isEnabled()) {
        return;
    }

    if (nullptr == m_sheet)
        return;

    DocummentProxy *proxy =  m_sheet->getDocProxy();
    if (proxy) {
        int nPage = proxy->currentPageNo();
        slotAddBookMark(QString::number(nPage));
        proxy->setBookMarkState(nPage, true);
    }
}

//  书签状态 添加指定页
void BookMarkWidget::slotAddBookMark(const QString &sContent)
{
    int nPage = sContent.toInt();

    QString sPath = CentralDocPage::Instance()->qGetCurPath();
    QList<int> pageList = dApp->m_pDBService->getBookMarkList(sPath);
    if (pageList.contains(nPage)) {
        return;
    }

    if (nullptr == m_sheet)
        return;

    auto item = addBookMarkItem(nPage);
    if (item) {
        pageList.append(nPage);
        dApp->m_pDBService->setBookMarkList(sPath, pageList);
        m_sheet->setFileChanged(true);
    }

    DocummentProxy *proxy =  m_sheet->getDocProxy();
    if (proxy) {
        int nCurPage = proxy->currentPageNo();
        if (nCurPage == nPage) {  //  是当前页
            proxy->setBookMarkState(nPage, true);
            m_pAddBookMarkBtn->setEnabled(false);
        }
        emit sigSetBookMarkState(1, nPage);
    }
}

/**
 * @brief BookMarkWidget::slotOpenFileOk
 *  打开文件成功，　获取该文件的书签数据
 */
void BookMarkWidget::handleOpenSuccess()
{
    if (nullptr == m_sheet)
        return;

    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }

    m_pBookMarkListWidget->clear();

    QList<int> pageList = dApp->m_pDBService->getBookMarkList(m_sheet->qGetPath());
    if (pageList.size() == 0)
        return;

    clearItemColor();

    DocummentProxy *proxy =  m_sheet->getDocProxy();

    if (proxy) {
        foreach (int iPage, pageList) {
            addBookMarkItem(iPage);
            proxy->setBookMarkState(iPage, true);
        }

        //  第一页 就是书签, 添加书签按钮 不能点
        int nCurPage = proxy->currentPageNo();
        if (pageList.contains(nCurPage)) {      //  当前页 是书签, 按钮不可点
            m_pAddBookMarkBtn->setEnabled(false);

            int nSize = m_pBookMarkListWidget->count();
            for (int iLoop = 0; iLoop < nSize; iLoop++) {
                auto item = m_pBookMarkListWidget->item(iLoop);
                if (item) {
                    auto pItemWidget =
                        qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
                    if (pItemWidget) {
                        if (pItemWidget->nPageIndex() == nCurPage) {
                            pItemWidget->setBSelect(true);

                            m_pBookMarkListWidget->setCurrentItem(item);
                            break;
                        }
                    }
                }
            }
        }
    }

    m_loadBookMarkThread.setBookMarks(pageList.count());
    m_loadBookMarkThread.start();
}


/**
 * @brief BookMarkWidget::slotDocFilePageChanged
 * @param page:当前活动页，页码
 */
void BookMarkWidget::slotDocFilePageChanged(const QString &sPage)
{
    //  先将当前的item 取消绘制
    clearItemColor();

    m_pAddBookMarkBtn->setEnabled(true);

    //  包含了该书签, 之后 才进行绘制
    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto item = m_pBookMarkListWidget->item(iLoop);
        if (item) {
            auto pItemWidget =
                qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
            if (pItemWidget) {
                int nWidgetPage = pItemWidget->nPageIndex();
                if (nWidgetPage == sPage.toInt()) {
                    pItemWidget->setBSelect(true);
                    m_pAddBookMarkBtn->setEnabled(false);
                    m_pBookMarkListWidget->setCurrentItem(item);

                    break;
                }
            }
        }
    }
}

/**
 * @brief BookMarkWidget::slotDeleteBookItem
 * 按页码删除书签
 * @param nPage：要删除的书签页
 */
void BookMarkWidget::slotDeleteBookItem(const QString &sContent)
{
    int nPage = sContent.toInt();

    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto pItem = m_pBookMarkListWidget->item(iLoop);
        if (pItem) {
            auto t_widget =
                qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();
                if (nPageIndex == nPage) {
                    t_widget->deleteLater();
                    t_widget = nullptr;

                    delete pItem;
                    pItem = nullptr;

                    deleteIndexPage(nPageIndex);

                    break;
                }
            }
        }
    }
}

//  删除指定页
void BookMarkWidget::deleteIndexPage(const int &pageIndex)
{
    if (nullptr == m_sheet)
        return;

    QList<int> pageList = dApp->m_pDBService->getBookMarkList(m_strBindPath);
    pageList.removeOne(pageIndex);
    dApp->m_pDBService->setBookMarkList(m_strBindPath, pageList);

    if (m_strBindPath != m_sheet->qGetPath())
        return;

    DocummentProxy *proxy =  m_sheet->getDocProxy();
    if (proxy) {
        m_sheet->setFileChanged(true);
        m_sheet->showTips(tr("The bookmark has been removed"));

        proxy->setBookMarkState(pageIndex, false);

        int nCurPage = proxy->currentPageNo();
        if (nCurPage == pageIndex) {  //  当前页被删了
            m_pAddBookMarkBtn->setEnabled(true);
        }

        emit sigSetBookMarkState(-1, pageIndex);
    }
}

void BookMarkWidget::clearItemColor()
{
    if (m_pBookMarkListWidget == nullptr)
        return;
    auto pCurItem = m_pBookMarkListWidget->currentItem();
    if (pCurItem) {
        auto pItemWidget =
            qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

BookMarkItemWidget *BookMarkWidget::getItemWidget(QListWidgetItem *item)
{
    if (m_pBookMarkListWidget == nullptr) {
        return nullptr;
    }
    auto pWidget = qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (pWidget) {
        return pWidget;
    }
    return nullptr;
}

/**
 * @brief BookMarkWidget::adaptWindowSize
 * 书签缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void BookMarkWidget::adaptWindowSize(const double &scale)
{
    double width = 1.0;
    double height = 1.0;

    //set item size
    width = static_cast<double>(LEFTMINWIDTH) * scale;
    height = static_cast<double>(80) * scale;

    if (m_pBookMarkListWidget) {
        int itemCount = 0;
        itemCount = m_pBookMarkListWidget->count();
        if (itemCount > 0) {
            for (int index = 0; index < itemCount; index++) {
                auto item = m_pBookMarkListWidget->item(index);
                if (item) {
                    auto itemWidget = getItemWidget(item);
                    if (itemWidget) {
                        item->setSizeHint(QSize(static_cast<int>(width), static_cast<int>(height)));
                        itemWidget->adaptWindowSize(scale);
                    }
                }
            }
        }
    }
}

/**
 * @brief BookMarkWidget::updateThumbnail
 * 高亮操作之后要跟换相应的缩略图
 * @param page 页码数，从0开始
 */
void BookMarkWidget::updateThumbnail(const int &page)
{
    if (m_pBookMarkListWidget == nullptr) {
        return;
    }
    int itemNum = 0;
    itemNum = m_pBookMarkListWidget->count();
    if (itemNum <= 0) {
        return;
    }
    QImage image;
    int tW = 48;
    int tH = 68;
    dApp->adaptScreenView(tW, tH);

    if (m_strBindPath != m_sheet->qGetPath())
        return;

    auto dproxy = m_sheet->getDocProxy();
    if (nullptr == dproxy)
        return;

    dproxy->getImage(page, image, tW, tH);
    for (int index = 0; index < itemNum; index++) {
        auto item = m_pBookMarkListWidget->item(index);
        if (item) {
            auto itemWidget = getItemWidget(item);
            if (itemWidget) {
                if (itemWidget->nPageIndex() == page) {
                    if (nullptr == dproxy) {
                        return;
                    }
                    itemWidget->setLabelImage(image);
                    return;
                }
            }
        }
    }
}

QString BookMarkWidget::getBindPath() const
{
    return m_strBindPath;
}

/**
 * @brief BookMarkWidget::slotLoadImage
 * 填充书签缩略图
 * @param page
 * @param image
 */
void BookMarkWidget::slotLoadImage(const int &page, const QImage &image)
{
    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto pItem = m_pBookMarkListWidget->item(iLoop);
        if (pItem) {
            auto t_widget =
                qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();
                if (nPageIndex == page) {
                    t_widget->setLabelImage(image);
                    break;
                }
            }
        }
    }
}

void BookMarkWidget::slotUpdateTheme()
{
    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
    m_pAddBookMarkBtn->setPalette(plt);
}

/**
 * @brief BookMarkWidget::initWidget
 * 初始化书签窗体
 */
void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new CustomListWidget(m_sheet, this);
    m_pBookMarkListWidget->setListType(E_BOOKMARK_WIDGET);
    connect(m_pBookMarkListWidget, SIGNAL(sigListMenuClick(const int &)), SLOT(slotListMenuClick(const int &)));
    connect(m_pBookMarkListWidget, SIGNAL(sigSelectItem(QListWidgetItem *)), SLOT(slotSelectItemBackColor(QListWidgetItem *)));

    m_pAddBookMarkBtn = new DPushButton(this);
    int tW = 170;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pAddBookMarkBtn->setFixedHeight(tH);
    m_pAddBookMarkBtn->setMinimumWidth(tW);
    m_pAddBookMarkBtn->setText(tr("Add bookmark"));
    DFontSizeManager::instance()->bind(m_pAddBookMarkBtn, DFontSizeManager::T6);
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(slotAddBookMark()));

    auto m_pHBoxLayout = new QHBoxLayout;
    m_pHBoxLayout->setContentsMargins(10, 6, 10, 6);
    m_pHBoxLayout->addWidget(m_pAddBookMarkBtn);

    auto m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 10, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);
    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);
    m_pVBoxLayout->addWidget(new DHorizontalLine(this));
    m_pVBoxLayout->addItem(m_pHBoxLayout);
}

/**
 * @brief BookMarkWidget::initConnection
 *初始化connect
 */
void BookMarkWidget::initConnection()
{
    connect(&m_loadBookMarkThread, SIGNAL(sigLoadImage(const int &, const QImage &)),
            SLOT(slotLoadImage(const int &, const QImage &)));
}

/**
 * @brief BookMarkWidget::addBookMarkItem
 * 添加书签
 * @param page：要增加的书签页
 */
QListWidgetItem *BookMarkWidget::addBookMarkItem(const int &page)
{
    CentralDocPage *pMtwe = CentralDocPage::Instance();
    if (pMtwe) {
        DocummentProxy *proxy =  pMtwe->getCurFileAndProxy(m_strBindPath);

        if (nullptr != proxy) {
            QImage t_image;
            double width = 1.0;
            double height = 1.0;
            double scale = 1.0;
            scale = dApp->scale();
            //set item size
            width = static_cast<double>(LEFTMINWIDTH) * scale;
            height = static_cast<double>(80) * scale;
            int tW = 48;
            int tH = 68;
            dApp->adaptScreenView(tW, tH);
            bool rl = proxy->getImage(page, t_image, tW, tH /*42, 62*/);
            if (rl) {
                QImage img = Utils::roundImage(QPixmap::fromImage(t_image), ICON_SMALL);
                auto item = m_pBookMarkListWidget->insertWidgetItem(page);
                tW = LEFTMINWIDTH;
                tH = 80;
                dApp->adaptScreenView(tW, tH);
                tW = static_cast<int>(width);
                tH = static_cast<int>(height);
                item->setSizeHint(QSize(tW, tH));

                auto t_widget = new BookMarkItemWidget(this);
                t_widget->setLabelImage(img);
                t_widget->setLabelPage(page, 1);
                m_pBookMarkListWidget->setItemWidget(item, t_widget);
                t_widget->adaptWindowSize(scale);

                int nCurPage = proxy->currentPageNo();
                if (nCurPage == page) {

                    t_widget->setBSelect(true);

                    m_pBookMarkListWidget->setCurrentItem(item);
                }

                return item;
            }
        }
    }
    return nullptr;
}

/**
 * @brief BookMarkWidget::slotSelectItemBackColor
 * 绘制选中外边框,蓝色
 */
void BookMarkWidget::slotSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    auto curItem = m_pBookMarkListWidget->currentItem();
    if (curItem == item) {
        return;
    }

    clearItemColor();

    m_pBookMarkListWidget->setCurrentItem(item);

    auto pItemWidget =
        qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (pItemWidget) {
        pItemWidget->setBSelect(true);
    }
}

void BookMarkWidget::SlotBookMarkMsg(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_DELETE_BOOKMARK) {
        slotDeleteBookItem(msgContent);
    } else if (msgType == MSG_OPERATION_ADD_BOOKMARK) {
        slotAddBookMark(msgContent);
    }
}

void BookMarkWidget::slotListMenuClick(const int &iType)
{
    if (iType == E_BOOKMARK_DELETE) {
        DeleteItemByKey();
    }
}

/**
 * @brief BookMarkWidget::dealWithData
 * 处理全局消息
 * @param msgType:消息类型
 * @param msgContent:消息内容
 * @return
 */
int BookMarkWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更消息
        slotUpdateTheme();
    } else if (MSG_FILE_PAGE_CHANGE == msgType) { //  文档页变化消息
        slotDocFilePageChanged(msgContent);
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;

}

int BookMarkWidget::qDealWithShortKey(const QString &s)
{
    if (s == KeyStr::g_ctrl_b) {
        slotAddBookMark();
    }

    if (m_pKeyMsgList.contains(s)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}

/**
 * @brief BookMarkWidget::getBookMarkPage
 * @param index(list item 从0开始)
 * @return 返回要填充缩略图的页码，不一定是index
 */
int BookMarkWidget::getBookMarkPage(const int &index)
{
    auto pItem = m_pBookMarkListWidget->item(index);
    if (pItem) {
        auto pItemWidget =
            qobject_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
        if (pItemWidget) {
            int page = pItemWidget->nPageIndex();

            QList<int> pageList = dApp->m_pDBService->getBookMarkList(m_strBindPath);
            if (pageList.contains(page)) {
                return page;
            }
        }
    }

    return -1;
}

/*************************************LoadBookMarkThread**************************************/
/*************************************加载书签列表线程******************************************/

LoadBookMarkThread::LoadBookMarkThread(QObject *parent)
    : QThread(parent)
{
}

/**
 * @brief LoadBookMarkThread::stopThreadRun
 * 线程退出
 */
void LoadBookMarkThread::stopThreadRun()
{
    m_isRunning = false;

    quit();
    wait();  //阻塞等待
}

/**
 * @brief LoadBookMarkThread::run
 * 线程主工作区，加载相应缩略图
 */
void LoadBookMarkThread::run()
{
    CentralDocPage *pMtwe = CentralDocPage::Instance();
    if (pMtwe) {
        DocummentProxy *proxy =  pMtwe->getCurFileAndProxy(m_pBookMarkWidget->getBindPath());
        if (nullptr != proxy) {
            while (m_isRunning) {
                msleep(50);

                if (m_nStartIndex < 0) {
                    m_nStartIndex = 0;
                }
                if (m_nEndIndex >= m_bookMarks) {
                    m_nEndIndex = m_bookMarks - 1;
                }
//                int tW = 48;
//                int tH = 68;
                int tW = 146;
                int tH = 174;
                dApp->adaptScreenView(tW, tH);
                for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {
                    QImage image;
                    int page = -1;

                    if (!m_isRunning) {
                        break;
                    }

                    if (!m_pBookMarkWidget) {
                        break;
                    }

                    page = m_pBookMarkWidget->getBookMarkPage(index);

                    if (page == -1) {
                        continue;
                    }

                    bool bl = proxy->getImage(page, image, tW, tH /*42, 62*/);
                    if (bl) {
                        emit sigLoadImage(page, image);
                    }

                    msleep(50);
                }

                m_nStartIndex += FIRST_LOAD_PAGES;
                m_nEndIndex += FIRST_LOAD_PAGES;
            }
        }
    }
}

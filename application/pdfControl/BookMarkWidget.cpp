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

#include "application.h"
#include "BookMarkItemWidget.h"

#include "controller/DataManager.h"
#include "controller/AppSetting.h"
#include "docview/docummentproxy.h"

BookMarkWidget::BookMarkWidget(DWidget *parent)
    : CustomWidget(QString("BookMarkWidget"), parent)
{
    setFocusPolicy(Qt::ClickFocus);

    initWidget();
    initConnection();
    slotUpdateTheme();

    m_pKeyMsgList = {KeyStr::g_ctrl_b};

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}
/**
 * @brief BookMarkWidget::~BookMarkWidget
 * 等待子线程退出
 */
BookMarkWidget::~BookMarkWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }

    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }
}

/**
 * @brief BookMarkWidget::prevPage
 * 上一页
 */
void BookMarkWidget::prevPage()
{
    if (m_pBookMarkListWidget == nullptr ||
            DataManager::instance()->bThumbnIsShow() == false ||
            DataManager::instance()->CurShowState() != FILE_NORMAL) {
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
        auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (t_widget) {
            notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(t_widget->nPageIndex()));
        }
    }
}

/**
 * @brief BookMarkWidget::nextPage
 * 下一页
 */
void BookMarkWidget::nextPage()
{
    if (DataManager::instance()->bThumbnIsShow() == false ||
            DataManager::instance()->CurShowState() != FILE_NORMAL) {
        return;
    }

    if (m_pBookMarkListWidget->count() <= 0) {
        return;
    }

    int curIndex = m_pBookMarkListWidget->currentRow();
    curIndex++;
    if (curIndex >= m_pBookMarkListWidget->count())
        return;
    auto pCurItem = m_pBookMarkListWidget->item(curIndex);
    if (pCurItem) {
        auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (t_widget) {
            notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(t_widget->nPageIndex()));
        }
    }
}

void BookMarkWidget::DeleteItemByKey()
{
    bool bFocus = this->hasFocus();
    if (bFocus) {
        auto pCurItem = m_pBookMarkListWidget->currentItem();
        if (pCurItem) {
            auto t_widget =
                reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
            if (t_widget) {
                if (t_widget->bSelect()) {
                    int nPageIndex = t_widget->nPageIndex();

                    notifyMsg(MSG_BOOKMARK_DLTITEM, QString::number(nPageIndex));
                }
            }
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

    auto proxy = DocummentProxy::instance();
    if (proxy) {
        int nPage = proxy->currentPageNo();
        sendMsg(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nPage));
    }
}

//  书签状态 添加指定页
void BookMarkWidget::slotAddBookMark(const int &nPage)
{
    QList<int> pageList = dApp->dbM->getBookMarkList();
    if (pageList.contains(nPage)) {
        return;
    }

    auto item = addBookMarkItem(nPage);
    if (item) {
        DataManager::instance()->setBIsUpdate(true);
        pageList.append(nPage);
        dApp->dbM->setBookMarkList(pageList);
    }

    auto dproxy = DocummentProxy::instance();
    if (dproxy) {
        int nCurPage = dproxy->currentPageNo();
        if (nCurPage == nPage) {  //  是当前页
            m_pAddBookMarkBtn->setEnabled(false);
        }
    }
//}
}

/**
 * @brief BookMarkWidget::slotOpenFileOk
 *  打开文件成功，　获取该文件的书签数据
 */
void BookMarkWidget::slotOpenFileOk()
{
    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }

    m_pBookMarkListWidget->clear();

    dApp->dbM->getBookMarks();
    QList<int> pageList = dApp->dbM->getBookMarkList();
    foreach (int iPage, pageList) {
        addBookMarkItem(iPage);
    }

    clearItemColor();

    //  第一页 就是书签, 添加书签按钮 不能点
    if (pageList.contains(0)) {
        m_pAddBookMarkBtn->setEnabled(false);

        int nSize = m_pBookMarkListWidget->count();
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            auto item = m_pBookMarkListWidget->item(iLoop);
            if (item) {
                auto pItemWidget =
                    reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
                if (pItemWidget) {
                    if (pItemWidget->nPageIndex() == 0) {
                        pItemWidget->setBSelect(true);

                        m_pBookMarkListWidget->setCurrentItem(item);
                        break;
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

    int page = sPage.toInt();
    QList<int> pageList = dApp->dbM->getBookMarkList();
    bool bl = pageList.contains(page);

    m_pAddBookMarkBtn->setEnabled(!bl);

    //  包含了该书签, 之后 才进行绘制
    if (bl) {
        int nSize = m_pBookMarkListWidget->count();
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            auto item = m_pBookMarkListWidget->item(iLoop);
            if (item) {
                auto pItemWidget =
                    reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
                if (pItemWidget) {
                    int nWidgetPage = pItemWidget->nPageIndex();

                    auto dproxy = DocummentProxy::instance();
                    if (dproxy) {
                        int nCurPage = dproxy->currentPageNo();
                        if (nWidgetPage == nCurPage) {
                            pItemWidget->setBSelect(bl);

                            m_pBookMarkListWidget->setCurrentItem(item);

                            break;
                        }
                    }
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
void BookMarkWidget::slotDeleteBookItem(const int &nPage)
{
    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto pItem = m_pBookMarkListWidget->item(iLoop);
        if (pItem) {
            auto t_widget =
                reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();
                if (nPageIndex == nPage) {
                    t_widget->deleteLater();
                    t_widget = nullptr;

                    delete pItem;
                    pItem = nullptr;

                    deleteIndexPage(nPageIndex);

                    notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("The bookmark has been removed"));

                    break;
                }
            }
        }
    }
}

//  删除指定页
void BookMarkWidget::deleteIndexPage(const int &pageIndex)
{
    QList<int> pageList = dApp->dbM->getBookMarkList();
    pageList.removeOne(pageIndex);
    dApp->dbM->setBookMarkList(pageList);

    auto dproxy = DocummentProxy::instance();
    if (dproxy) {
        DataManager::instance()->setBIsUpdate(true);
        dproxy->setBookMarkState(pageIndex, false);

        int nCurPage = dproxy->currentPageNo();
        if (nCurPage == pageIndex) {  //  当前页被删了
            m_pAddBookMarkBtn->setEnabled(true);
        }
    }
}

void BookMarkWidget::clearItemColor()
{
    if (m_pBookMarkListWidget == nullptr)
        return;
    auto pCurItem = m_pBookMarkListWidget->currentItem();
    if (pCurItem) {
        auto pItemWidget =
            reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

bool BookMarkWidget::bOperationBK()
{
    int leftShow = 0;
    int widgetIndex = 0;
    leftShow = DataManager::instance()->getShowLeft().toInt();
    widgetIndex = DataManager::instance()->getListIndex().toInt();

    if (leftShow == 1 && widgetIndex == 2) {
        return true;
    }

    return false;
}

/**
 * @brief BookMarkWidget::slotCloseFile
 *关联关闭文件成功槽函数
 */
void BookMarkWidget::slotCloseFile()
{
    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }
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
                reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
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

/**
 * @brief BookMarkWidget::slotDelBkItem
 * delete键删除鼠标选中书签item
 */
//void BookMarkWidget::slotDelBkItem()
//{
////    qDebug() << __FUNCTION__ << "  111111111111111 ";
//    //  按Del键删除, 当前显示的List 必须是 自己 才可以进行删除
//    bool bFocus = this->hasFocus();
//    if (bFocus) {
//        auto pCurItem = m_pBookMarkListWidget->currentItem();
//        if (pCurItem) {
//            auto t_widget =
//                reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
//            if (t_widget) {
//                if (t_widget->bSelect()) {
//                    int nPageIndex = t_widget->nPageIndex();

//                    notifyMsg(MSG_BOOKMARK_DLTITEM, QString::number(nPageIndex));

////                    t_widget->deleteLater();
////                    t_widget = nullptr;

////                    delete pCurItem;
////                    pCurItem = nullptr;

////                    deleteIndexPage(nPageIndex);
////                    notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("The bookmark has been removed"));
//                }
//            }
//        }
//    }
//}

void BookMarkWidget::slotUpdateTheme()
{
    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
    m_pAddBookMarkBtn->setPalette(plt);
    //  updateWidgetTheme();
}

void BookMarkWidget::slotRightSelectItem(QString page)
{
    if (m_pBookMarkListWidget == nullptr) {
        return;
    }

    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto pItem = m_pBookMarkListWidget->item(iLoop);
        if (pItem) {
            auto t_widget =
                reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();
                if (nPageIndex == page.toInt()) {
                    slotSelectItemBackColor(pItem);
                }
            }
        }
    }
}

/**
 * @brief BookMarkWidget::initWidget
 * 初始化书签窗体
 */
void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new CustomListWidget(this);

    m_pAddBookMarkBtn = new DPushButton(this);
    m_pAddBookMarkBtn->setFixedHeight(36);
    m_pAddBookMarkBtn->setMinimumWidth(170);
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
    connect(&m_loadBookMarkThread, SIGNAL(sigLoadImage(const int &, const QImage &)), this,
            SLOT(slotLoadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigAddBookMark(const int &)), this, SLOT(slotAddBookMark(const int &)));
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem(const int &)), this,
            SLOT(slotDeleteBookItem(const int &)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
//    connect(this, SIGNAL(sigDelBKItem()), this, SLOT(slotDelBkItem()));
    connect(this, SIGNAL(sigUpdateTheme()), this, SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigFilePageChanged(const QString &)),
            SLOT(slotDocFilePageChanged(const QString &)));
//    connect(this, SIGNAL(sigCtrlBAddBookMark()), SLOT(slotAddBookMark()));
    connect(this, SIGNAL(sigRightSelectItem(QString)), SLOT(slotRightSelectItem(QString)));
    connect(m_pBookMarkListWidget, SIGNAL(sigSelectItem(QListWidgetItem *)),
            SLOT(slotSelectItemBackColor(QListWidgetItem *)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)),
            this, SLOT(slotDealWithShurtKey(const QString &)));
}

/**
 * @brief BookMarkWidget::addBookMarkItem
 * 添加书签
 * @param page：要增加的书签页
 */
QListWidgetItem *BookMarkWidget::addBookMarkItem(const int &page)
{
    auto dproxy = DocummentProxy::instance();
    if (nullptr == dproxy) {
        return nullptr;
    }
    QImage t_image;
    bool rl = dproxy->getImage(page, t_image, 48, 68 /*42, 62*/);
    if (rl) {
        QImage img = Utils::roundImage(QPixmap::fromImage(t_image), ICON_SMALL);
        auto item = m_pBookMarkListWidget->insertWidgetItem(page);
        item->setSizeHint(QSize(LEFTMINWIDTH, 80));

        auto t_widget = new BookMarkItemWidget(this);
//        connect(t_widget, SIGNAL(sigDeleleteItem(const int &)), SLOT(slotDeleteBookItem(const int &)));
        t_widget->setLabelImage(img);
        t_widget->setLabelPage(page, 1);

        m_pBookMarkListWidget->setItemWidget(item, t_widget);

        int nCurPage = dproxy->currentPageNo();
        if (nCurPage == page) {

            t_widget->setBSelect(true);

            m_pBookMarkListWidget->setCurrentItem(item);
        }
        dproxy->setBookMarkState(page, true);

        return item;
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
        reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (pItemWidget) {
        pItemWidget->setBSelect(true);
    }
}

/**
 * @brief BookMarkWidget::slotDealWithShurtKey
 * 处理键盘事件
 * @param msgContent    事件内容
 */
void BookMarkWidget::slotDealWithShurtKey(const QString &msgContent)
{
    if (msgContent == KeyStr::g_ctrl_b) {
        slotAddBookMark();
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
    if (MSG_BOOKMARK_DLTITEM == msgType || MSG_OPERATION_DELETE_BOOKMARK == msgType) {   //  删除书签消息
        emit sigDeleteBookItem(msgContent.toInt());
    } /*else if (MSG_BOOKMARK_DLTITEM == msgType || MSG_OPERATION_DELETE_BOOKMARK == msgType) {   //  删除书签消息
        emit sigDeleteBookItem(msgContent.toInt());
    }*/ else if (MSG_OPERATION_ADD_BOOKMARK == msgType || MSG_OPERATION_TEXT_ADD_BOOKMARK == msgType) {  //  增加书签消息
        emit sigAddBookMark(msgContent.toInt());
    } else if (MSG_OPERATION_RIGHT_SELECT_BOOKMARK == msgType) {
        emit sigRightSelectItem(msgContent);
    } else  if (MSG_OPERATION_OPEN_FILE_OK == msgType) {  //  打开 文件通知消息
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {                 //  关闭 文件通知消息
        emit sigCloseFile();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更消息
        emit sigUpdateTheme();
    } else if (MSG_FILE_PAGE_CHANGE == msgType) {  //  文档页变化消息
        emit sigFilePageChanged(msgContent);
    } else if (MSG_NOTIFY_KEY_MSG == msgType) {  //  按键通知消息
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }
    return 0;
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
            reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
        if (pItemWidget) {
            int page = pItemWidget->nPageIndex();
            QList<int> pageList = dApp->dbM->getBookMarkList();
            if (pageList.contains(page)) {
                return page;
            }
        }
    }

    return -1;
}

bool BookMarkWidget::hasClickFoucs()
{
    if (m_pBookMarkListWidget) {
        return m_pBookMarkListWidget->hasFocus();
    }

    return false;
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
    DocummentProxy *_proxy = nullptr;
    while (m_isRunning) {
        if (!_proxy) {
            _proxy = DocummentProxy::instance();
        }
        msleep(50);

        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_bookMarks) {
            m_nEndIndex = m_bookMarks - 1;
        }

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

            bool bl = _proxy->getImage(page, image, 48, 68 /*42, 62*/);
            if (bl) {
                emit sigLoadImage(page, image);
            }

            msleep(50);
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex += FIRST_LOAD_PAGES;
    }
}

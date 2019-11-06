#include "BookMarkWidget.h"
#include "controller/DataManager.h"

BookMarkWidget::BookMarkWidget(CustomWidget *parent) :
    CustomWidget(QString("BookMarkWidget"), parent)
{
    initWidget();

    initConnection();
}
/**
 * @brief BookMarkWidget::~BookMarkWidget
 * 等待子线程退出
 */
BookMarkWidget::~BookMarkWidget()
{
    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }
}

/**
 * @brief BookMarkWidget::slotAddBookMark
 * @brief 点击按钮添加 当前页 为 书签
 */
void BookMarkWidget::slotAddBookMark()
{
    auto proxy = DocummentProxy::instance();
    if (proxy) {
        //  书签, 添加当前页
        int nPage = proxy->currentPageNo();
        bool bRes = proxy->setBookMarkState(nPage, true);
        if (bRes) {
            slotAddBookMark(nPage);
            m_pAddBookMarkBtn->setEnabled(false);
        }
    }
}

//  书签状态 添加指定页
void BookMarkWidget::slotAddBookMark(const int &nPage)
{
    QList<int> pageList = dApp->dbM->getBookMarkList();
    if (pageList.contains(nPage)) {
        return;
    }

    pageList.append(nPage);
    dApp->dbM->setBookMarkList(pageList);

    auto item = addBookMarkItem(nPage);
    if (item) {
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
        if (pItemWidget) {
            int nWidgetPage = pItemWidget->nPageIndex();
            if (nWidgetPage == nPage) {
                pItemWidget->setBSelect(true);

                m_pBookMarkListWidget->setCurrentItem(item);
            }
        }
        m_pAddBookMarkBtn->setEnabled(false);

        DataManager::instance()->setBIsUpdate(true);
    }
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
    disconnect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)));
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)), Qt::QueuedConnection);

    m_pBookMarkListWidget->clear();

    dApp->dbM->getBookMarks();
    QList<int> pageList = dApp->dbM->getBookMarkList();
    foreach (int iPage, pageList) {
        addBookMarkItem(iPage);
    }

    //  第一页 就是书签, 添加书签按钮 不能点
    if (pageList.contains(0)) {
        m_pAddBookMarkBtn->setEnabled(false);

        int nSize = m_pBookMarkListWidget->count();
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            auto item = m_pBookMarkListWidget->item(iLoop);
            if (item) {
                auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
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
void BookMarkWidget::slotDocFilePageChanged(int page)
{
    //  先将当前的item 取消绘制
    auto pCurItem = m_pBookMarkListWidget->currentItem();
    if (pCurItem) {
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }

    QList<int> pageList = dApp->dbM->getBookMarkList();
    bool bl = pageList.contains(page);

    m_pAddBookMarkBtn->setEnabled(!bl);

    //  包含了该书签, 之后 才进行绘制
    if (bl) {
        int nSize = m_pBookMarkListWidget->count();
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            auto item = m_pBookMarkListWidget->item(iLoop);
            if (item) {
                auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
                if (pItemWidget) {
                    int nWidgetPage = pItemWidget->nPageIndex();
                    if (nWidgetPage == page) {
                        pItemWidget->setBSelect(bl);

                        m_pBookMarkListWidget->setCurrentItem(item);

                        break;
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
            auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();
                if (nPageIndex == nPage) {
                    t_widget->deleteLater();
                    t_widget = nullptr;

                    delete  pItem;
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
    QList<int> pageList = dApp->dbM->getBookMarkList();
    pageList.removeOne(pageIndex);
    dApp->dbM->setBookMarkList(pageList);

    auto dproxy = DocummentProxy::instance();
    if (dproxy) {
        dproxy->setBookMarkState(pageIndex, false);

        DataManager::instance()->setBIsUpdate(true);
        m_pAddBookMarkBtn->setEnabled(true);
    }
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
            auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
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
 * @brief BookMarkWidget::slotDelNoteItem
 * delete键删除鼠标选中书签item
 */
void BookMarkWidget::slotDelBkItem()
{
    //  按Del键删除, 当前显示的List 必须是 自己 才可以进行删除
    bool bFocus = this->hasFocus();
    if (bFocus) {
        auto pCurItem = m_pBookMarkListWidget->currentItem();
        if (pCurItem) {
            auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pCurItem));
            if (t_widget) {
                int nPageIndex = t_widget->nPageIndex();

                t_widget->deleteLater();
                t_widget = nullptr;

                delete  pCurItem;
                pCurItem = nullptr;

                deleteIndexPage(nPageIndex);
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
    m_pBookMarkListWidget = new CustomListWidget;
    m_pBookMarkListWidget->setSpacing(0);

    m_pAddBookMarkBtn = new DPushButton(this);
    m_pAddBookMarkBtn->setFixedHeight(36);
    m_pAddBookMarkBtn->setMinimumWidth(170);
    m_pAddBookMarkBtn->setText(tr("add bookmark"));
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(slotAddBookMark()));

    auto m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);
    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);
    m_pVBoxLayout->addSpacing(4);
    m_pVBoxLayout->addWidget(m_pAddBookMarkBtn);
}

/**
 * @brief BookMarkWidget::initConnection
 *初始化connect
 */
void BookMarkWidget::initConnection()
{
    connect(&m_loadBookMarkThread, SIGNAL(sigLoadImage(const int &, const QImage &)), this, SLOT(slotLoadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigAddBookMark(const int &)), this, SLOT(slotAddBookMark(const int &)));
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem(const int &)), this, SLOT(slotDeleteBookItem(const int &)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(this, SIGNAL(sigDelBKItem()), this, SLOT(slotDelBkItem()));
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
    bool rl = dproxy->getImage(page, t_image, 28, 48);
    if (rl) {
        auto t_widget = new BookMarkItemWidget;
        connect(t_widget, SIGNAL(sigDeleleteItem(const int &)), this, SLOT(slotDeleteBookItem(const int &)));
        t_widget->setLabelImage(t_image);
        t_widget->setLabelPage(page, 1);
        t_widget->setMinimumSize(QSize(240, 80));

        auto item = new QListWidgetItem(m_pBookMarkListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(240, 80));

        m_pBookMarkListWidget->addItem(item);
        m_pBookMarkListWidget->setItemWidget(item, t_widget);

        dproxy->setBookMarkState(page, true);

        return item;
    }
    return nullptr;
}

/**
 * @brief BookMarkWidget::setSelectItemBackColor
 * 绘制选中外边框,蓝色
 */
void BookMarkWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (pItemWidget) {
        pItemWidget->setBSelect(true);
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
    //  删除书签消息
    if (MSG_BOOKMARK_DLTITEM == msgType || MSG_OPERATION_DELETE_BOOKMARK == msgType) {
        emit sigDeleteBookItem(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    }

    //  增加书签消息
    if (MSG_BOOKMARK_ADDITEM == msgType || MSG_OPERATION_ADD_BOOKMARK == msgType || MSG_OPERATION_TEXT_ADD_BOOKMARK == msgType) {
        emit sigAddBookMark(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    }

    //  打开 文件通知消息
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    }

    //  关闭w文件通知消息
    if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (msgContent == KeyStr::g_del) {
            emit sigDelBKItem();
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
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
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


/*************************************LoadBookMarkThread**************************************/
/*************************************加载书签列表线程******************************************/

LoadBookMarkThread::LoadBookMarkThread(QObject *parent)
    : QThread (parent)
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
//    terminate();    //终止线程
    wait();         //阻塞等待
}

/**
 * @brief LoadBookMarkThread::run
 * 线程主工作区，加载相应缩略图
 */
void LoadBookMarkThread::run()
{
    while (m_isRunning) {

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

            bool bl = DocummentProxy::instance()->getImage(page, image, 28, 50);
            if (bl) {
                emit sigLoadImage(page, image);
            }

            msleep(10);
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex += FIRST_LOAD_PAGES;

        msleep(30);
    }
}

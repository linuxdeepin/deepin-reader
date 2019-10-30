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
        }
    }
}

//  书签状态 添加指定页
void BookMarkWidget::slotAddBookMark(const int &nPage)
{
    if (m_pAllPageList.contains(nPage)) {
        return;
    }

    m_pAllPageList.append(nPage);

    operateDb();

    addBookMarkItem(nPage);

    slotDocFilePageChanged(nPage);

    DataManager::instance()->setBIsUpdate(true);
}

/**
 * @brief BookMarkWidget::slotOpenFileOk
 *  打开文件成功，　获取该文件的书签数据
 */

void BookMarkWidget::slotOpenFileOk()
{
    disconnect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)));
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)), Qt::QueuedConnection);

    m_pAllPageList.clear();
    if (m_pBookMarkListWidget) {
        m_pBookMarkListWidget->clear();
    }

    QString sAllPages = dApp->dbM->getBookMarks();
    QStringList sPageList = sAllPages.split(",", QString::SkipEmptyParts);
    foreach (QString s, sPageList) {
        int nPage = s.toInt();

        m_pAllPageList.append(nPage);

        addBookMarkItem(nPage);
    }

    m_loadBookMarkThread.setBookMarks(m_pAllPageList.count());
    m_loadBookMarkThread.start();

    slotDocFilePageChanged(0);
}

/**
 * @brief BookMarkWidget::slotDocFilePageChanged
 * @param page:当前活动页，页码
 */
void BookMarkWidget::slotDocFilePageChanged(int page)
{
    bool bl = m_pAllPageList.contains(page);

    m_pAddBookMarkBtn->setEnabled(!bl);
    sendMsg(MSG_BOOKMARK_STATE, QString("%1").arg(bl));

    if (m_pIndexItem) {
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pIndexItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }

    int nSize = m_pBookMarkListWidget->count();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        auto item = m_pBookMarkListWidget->item(iLoop);
        if (item) {
            auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
            if (pItemWidget) {
                int nWidgetPage = pItemWidget->nPageIndex();
                if (nWidgetPage == page) {
                    pItemWidget->setBSelect(bl);

                    m_pIndexItem = item;

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

                    m_pAllPageList.removeOne(nPage);

                    slotDocFilePageChanged(nPage);

                    operateDb();

                    DataManager::instance()->setBIsUpdate(true);

                    break;
                }
            }
        }
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
    if (m_pIndexItem) {
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pIndexItem));
        if (pItemWidget) {
            if (pItemWidget->nPageIndex() == page) {
                pItemWidget->setLabelImage(image);
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
    if (m_pIndexItem) {
        auto t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pIndexItem));
        if (t_widget) {
            int nPageIndex = t_widget->nPageIndex();

            t_widget->deleteLater();
            t_widget = nullptr;

            delete  m_pIndexItem;
            m_pIndexItem = nullptr;

            m_pAllPageList.removeOne(nPageIndex);

            slotDocFilePageChanged(nPageIndex);

            operateDb();

            DataManager::instance()->setBIsUpdate(true);
        }
    }
}

void BookMarkWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item != nullptr && m_pIndexItem != item) {
        setSelectItemBackColor(item);
        m_pIndexItem = item;
    }
}

/**
 * @brief BookMarkWidget::initWidget
 * 初始化书签窗体
 */
void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new CustomListWidget;

    m_pAddBookMarkBtn = new DPushButton(this);
    m_pAddBookMarkBtn->setMinimumSize(QSize(250, 50));
    m_pAddBookMarkBtn->setText(tr("add bookmark"));
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(slotAddBookMark()));

    connect(this, SIGNAL(sigAddBookMark(const int &)), this, SLOT(slotAddBookMark(const int &)));

    auto m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);
    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);
    m_pVBoxLayout->addWidget(m_pAddBookMarkBtn);
}

/**
 * @brief BookMarkWidget::initConnection
 *初始化connect
 */
void BookMarkWidget::initConnection()
{
    connect(&m_loadBookMarkThread, SIGNAL(sigLoadImage(const int &, const QImage &)), this, SLOT(slotLoadImage(const int &, const QImage &)));
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem(const int &)), this, SLOT(slotDeleteBookItem(const int &)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(this, SIGNAL(sigDelBKItem()), this, SLOT(slotDelBkItem()));
    connect(m_pBookMarkListWidget, SIGNAL(sigSelectItem(QListWidgetItem *)), this, SLOT(slotSelectItem(QListWidgetItem *)));
}

/**
 * @brief BookMarkWidget::addBookMarkItem
 * 添加书签
 * @param page：要增加的书签页
 */
void BookMarkWidget::addBookMarkItem(const int &page)
{
    auto dproxy = DocummentProxy::instance();
    if (nullptr == dproxy) {
        return;
    }
    QImage t_image;
    bool rl = dproxy->getImage(page, t_image, 28, 48);
    if (rl) {
        auto t_widget = new BookMarkItemWidget;
        t_widget->setLabelImage(t_image);
        t_widget->setLabelPage(page, 1);
        t_widget->setMinimumSize(QSize(190, 80));

        auto item = new QListWidgetItem(m_pBookMarkListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(190, 80));

        m_pBookMarkListWidget->addItem(item);
        m_pBookMarkListWidget->setItemWidget(item, t_widget);

        dproxy->setBookMarkState(page, true);
    }
}

void BookMarkWidget::operateDb()
{
    if (m_pAllPageList.size() == 0) {
        dApp->dbM->deleteBookMark();
    } else if (m_pAllPageList.size() == 1) {
        dApp->dbM->insertBookMark(QString::number(m_pAllPageList.at(0)));
    } else {
        QString sPage = "";
        foreach (int i, m_pAllPageList) {
            sPage += QString::number(i) + ",";
        }

        dApp->dbM->updateBookMark(sPage);
    }
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

    //  先将之前的选中 取消
    if (m_pIndexItem) {
        auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pIndexItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }

    m_pIndexItem = item;

    auto pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pIndexItem));
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
        if (msgContent == QString("Del")) {
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
            if (m_pAllPageList.contains(page)) {
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

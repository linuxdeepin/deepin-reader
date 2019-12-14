#include "ThumbnailWidget.h"
#include "controller/DBManager.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"

ThumbnailWidget::ThumbnailWidget(CustomWidget *parent)
    : CustomWidget(QString("ThumbnailWidget"), parent)
{
    m_ThreadLoadImage.setThumbnail(this);

    //    setMinimumWidth(LEFTMINWIDTH - 5);
    //    setMaximumWidth(LEFTMAXWIDTH - 5);

    initWidget();

    initConnection();
}

ThumbnailWidget::~ThumbnailWidget()
{
    // 等待子线程退出
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    if (m_threadRotateImage.isRunning()) {
        m_threadRotateImage.stopThreadRun();
    }
}

// 处理消息事件
int ThumbnailWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (MSG_FILE_PAGE_CHANGE == msgType) {
        emit sigFilePageChanged(msgContent);
    } else if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (msgContent == KeyStr::g_up || msgContent == KeyStr::g_pgup ||
            msgContent == KeyStr::g_left) {
            //            qDebug() << __FUNCTION__ << "                   " << 1;
            emit sigJumpToPrevPage();
        } else if (msgContent == KeyStr::g_down || msgContent == KeyStr::g_pgdown ||
                   msgContent == KeyStr::g_right) {
            emit sigJumpToNextPage();
        }
    } else if (msgType == MSG_FILE_ROTATE) {  //  文档旋转了
        emit sigSetRotate(msgContent.toInt());
    }
    return 0;
}

// 初始化界面s
void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new CustomListWidget(this);
    m_pThumbnailListWidget->setSpacing(3);

    m_pPageWidget = new PagingWidget(this);
    auto hLine = new DHorizontalLine(this);

    auto m_pvBoxLayout = new QVBoxLayout(this);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    m_pvBoxLayout->addWidget(m_pPageWidget);
    m_pvBoxLayout->addWidget(hLine);

    m_pvBoxLayout->setContentsMargins(0, 2, 0, 8);
    m_pvBoxLayout->setSpacing(6);
    this->setLayout(m_pvBoxLayout);
}

//  画 选中项的背景颜色
void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    //  先清空之前的选中颜色
    auto curItem = m_pThumbnailListWidget->item(m_nCurrentPage);
    if (curItem) {
        auto itemWidget =
            reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(curItem));
        if (itemWidget) {
            itemWidget->setBSelect(false);
        }
    }

    if (item) {
        //        m_pThumbnailListWidget->setCurrentItem(item);
        m_nCurrentPage = m_pThumbnailListWidget->row(item);
        m_pThumbnailListWidget->setCurrentRow(m_nCurrentPage, QItemSelectionModel::NoUpdate);
        auto pWidget =
            reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
        if (pWidget) {
            pWidget->setBSelect(true);
        }

        int nJumpPage = pWidget->nPageIndex();
        m_pPageWidget->setCurrentPageValue(nJumpPage);
    }
}

//  添加项
void ThumbnailWidget::addThumbnailItem(const int &iIndex)
{
    auto widget = new ThumbnailItemWidget();
    widget->setLabelPage(iIndex);

    auto item = new QListWidgetItem(m_pThumbnailListWidget);
    item->setFlags(Qt::NoItemFlags);

    item->setSizeHint(QSize(LEFTMINWIDTH, 212));

    m_pThumbnailListWidget->addItem(item);
    m_pThumbnailListWidget->setItemWidget(item, widget);
}

void ThumbnailWidget::initConnection()
{
    connect(&m_ThreadLoadImage, SIGNAL(signal_loadImage(const int &, const QImage &)),
            m_pThumbnailListWidget, SLOT(slot_loadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigFilePageChanged(const QString &)),
            SLOT(slotDocFilePageChanged(const QString &)));
    connect(this, &ThumbnailWidget::sigJumpToPrevPage, this, &ThumbnailWidget::slotJumpToPrevPage);
    connect(this, &ThumbnailWidget::sigJumpToNextPage, this, &ThumbnailWidget::slotJumpToNextPage);
    connect(this, SIGNAL(sigSetRotate(int)), this, SLOT(slotSetRotate(int)));
    connect(&m_threadRotateImage, SIGNAL(sigRotateImage(int)), this,
            SLOT(slotRotateThumbnail(int)));
}

/**
 * @brief ThumbnailWidget::jumpToSpecifiedPage
 * 跳转到指定页
 * @param page
 */
void ThumbnailWidget::jumpToSpecifiedPage(const int &page)
{
    //  跳转的页码 必须 大于0, 且 小于 总页码数
    int nPageSize = DocummentFileHelper::instance()->getPageSNum();
    if (page < 0 || page == nPageSize) {
        return;
    }

    DocummentFileHelper::instance()->pageJump(page);
}

//  文件  当前页变化, 获取与 文档页  对应的 item, 设置 选中该item, 绘制item
void ThumbnailWidget::slotDocFilePageChanged(const QString &sPage)
{
    int page = sPage.toInt();
    auto curPageItem = m_pThumbnailListWidget->item(page);
    if (curPageItem) {
        setSelectItemBackColor(curPageItem);
    }
}

// 关联关闭文件槽函数
void ThumbnailWidget::slotCloseFile()
{
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
}

void ThumbnailWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

/**
 * @brief ThumbnailWidget::slotJumpToPrevPage
 *上一页
 */
void ThumbnailWidget::slotJumpToPrevPage()
{
    if (DataManager::instance()->CurShowState() != FILE_NORMAL) {
        bool bstart = false;
        if (nullptr != DocummentProxy::instance() &&
            DocummentProxy::instance()->getAutoPlaySlideStatu()) {
            DocummentProxy::instance()->setAutoPlaySlide(false);
            bstart = true;
        }
        int nCurPage = DocummentFileHelper::instance()->currentPageNo();
        nCurPage--;
        jumpToSpecifiedPage(nCurPage);
        if (bstart && nullptr != DocummentProxy::instance()) {
            DocummentProxy::instance()->setAutoPlaySlide(true);
            bstart = false;
        }
        return;
    }
    if (DataManager::instance()->currentWidget() != WIDGET_THUMBNAIL ||
        (DataManager::instance()->bThumbnIsShow() == false)) {
        return;
    }
    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage--;
    jumpToSpecifiedPage(nCurPage);
}

/**
 * @brief ThumbnailWidget::slotJumpToNextPage
 *下一页
 */
void ThumbnailWidget::slotJumpToNextPage()
{
    if (DataManager::instance()->CurShowState() != FILE_NORMAL) {
        bool bstart = false;
        if (nullptr != DocummentProxy::instance() &&
            DocummentProxy::instance()->getAutoPlaySlideStatu()) {
            DocummentProxy::instance()->setAutoPlaySlide(false);
            bstart = true;
        }
        int nCurPage = DocummentFileHelper::instance()->currentPageNo();
        nCurPage++;
        jumpToSpecifiedPage(nCurPage);
        if (bstart && nullptr != DocummentProxy::instance()) {
            DocummentProxy::instance()->setAutoPlaySlide(true);
            bstart = false;
        }
        return;
    }
    if (DataManager::instance()->currentWidget() != WIDGET_THUMBNAIL ||
        (DataManager::instance()->bThumbnIsShow() == false)) {
        return;
    }

    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage++;
    jumpToSpecifiedPage(nCurPage);
}

void ThumbnailWidget::slotSetRotate(int angle)
{
    m_nRotate = angle;
    if (m_threadRotateImage.isRunning()) {
        m_threadRotateImage.stopThreadRun();
    }
    m_threadRotateImage.setPages(m_totalPages);
    m_threadRotateImage.setLoadOver();
    m_threadRotateImage.start();
}

void ThumbnailWidget::slotRotateThumbnail(int index)
{
    if (m_pThumbnailListWidget) {
        if (index >= m_pThumbnailListWidget->count()) {
            return;
        }
        auto item = m_pThumbnailListWidget->item(index);
        auto pWidget =
            reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
        if (pWidget) {
            pWidget->rotateThumbnail(m_nRotate);
            //            qDebug() << __FUNCTION__ << "  rotate angle:" << m_nRotate;
        }
    }
}

// 初始化缩略图列表list，无缩略图
void ThumbnailWidget::fillContantToList()
{
    for (int idex = 0; idex < m_totalPages; ++idex) {
        addThumbnailItem(idex);
    }

    if (m_totalPages > 0) {
        showItemBookMark();
        auto item = m_pThumbnailListWidget->item(0);
        if (item) {
            //            m_pThumbnailListWidget->setCurrentItem(item);
            m_nCurrentPage = 0;
            auto pWidget =
                reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
            if (pWidget) {
                pWidget->setBSelect(true);
            }
        }
    }

    m_isLoading = false;
}

void ThumbnailWidget::showItemBookMark(int ipage)
{
    if (ipage >= 0) {
        auto pWidget = reinterpret_cast<ThumbnailItemWidget *>(
            m_pThumbnailListWidget->itemWidget(m_pThumbnailListWidget->item(ipage)));
        pWidget->slotBookMarkShowStatus(true);
    } else {
        DBManager::instance()->getBookMarks();
        QList<int> pageList = DBManager::instance()->getBookMarkList();
        foreach (int index, pageList) {
            auto pWidget = reinterpret_cast<ThumbnailItemWidget *>(
                m_pThumbnailListWidget->itemWidget(m_pThumbnailListWidget->item(index)));
            pWidget->slotBookMarkShowStatus(true);
        }
    }
}

// 关联成功打开文件槽函数
void ThumbnailWidget::slotOpenFileOk()
{
    m_isLoading = true;

    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    int pages = DocummentFileHelper::instance()->getPageSNum();

    m_totalPages = pages;
    m_pPageWidget->setTotalPages(m_totalPages);

    m_pThumbnailListWidget->clear();
    fillContantToList();

    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.setStartAndEndIndex();
    }
    m_ThreadLoadImage.setPages(m_totalPages);
    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

/*******************************ThreadLoadImage*************************************************/

ThreadLoadImage::ThreadLoadImage(QObject *parent)
    : QThread(parent)
{
}

void ThreadLoadImage::stopThreadRun()
{
    m_isLoaded = false;
    quit();
    //    terminate();    //终止线程
    wait();  //阻塞等待
}

// 加载缩略图线程
void ThreadLoadImage::run()
{
    while (m_isLoaded) {
        if (!m_pThumbnailWidget) {
            continue;
        }

        if (m_pThumbnailWidget->isLoading()) {
            continue;
        }

        if (m_nStartPage < 0) {
            m_nStartPage = 0;
        }
        if (m_nEndPage >= m_pages) {
            //            m_isLoaded = true;
            m_nEndPage = m_pages - 1;
        }

        auto dproxy = DocummentFileHelper::instance();
        if (nullptr == dproxy) {
            break;
        }

        for (int page = m_nStartPage; page <= m_nEndPage; page++) {
            if (!m_isLoaded)
                break;
            QImage image;
            bool bl = dproxy->getImage(page, image, 146, 174 /*138, 166*/);
            if (bl) {
                //                QImage img = Utils::roundImage(QPixmap::fromImage(image),
                //                ICON_BIG);
                emit signal_loadImage(page, image);
            }
        }
        m_nStartPage += FIRST_LOAD_PAGES;
        m_nEndPage += FIRST_LOAD_PAGES;

        msleep(50);
    }
}

/************************旋转缩略图线程****************************/

ThreadRotateImage::ThreadRotateImage(QObject *parent) {}

void ThreadRotateImage::stopThreadRun()
{
    m_bLoading = false;
    quit();
    wait();  //阻塞等待
}

void ThreadRotateImage::run()
{
    m_nFirstIndex = 0;
    m_nEndIndex = FIRST_LOAD_PAGES - 1;

    while (m_bLoading) {
        if (m_nFirstIndex < 0) {
            m_nFirstIndex = 0;
        }
        if (m_nEndIndex >= m_nPages) {
            m_nEndIndex = m_nPages - 1;
        }

        for (int index = m_nFirstIndex; index <= m_nEndIndex; index++) {
            emit sigRotateImage(index);
            msleep(30);
        }

        if (m_nEndIndex >= m_nPages) {
            m_bLoading = false;
            break;
        }
        m_nFirstIndex += FIRST_LOAD_PAGES;
        m_nEndIndex += FIRST_LOAD_PAGES;

        msleep(50);
    }

    m_nFirstIndex = 0;
    m_nEndIndex = FIRST_LOAD_PAGES - 1;
}

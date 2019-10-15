#include "ThumbnailWidget.h"
#include <QStringListModel>
#include <QStandardItemModel>


ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(QString("ThumbnailWidget"), parent)
{
//    m_ThreadLoadImage.setParent(this);
    m_ThreadLoadImage.setThumbnail(this);

    initWidget();

    connect(&m_ThreadLoadImage, SIGNAL(signal_loadImage(const int &, const QImage &)),
            m_pThumbnailListWidget, SLOT(slot_loadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
}

ThumbnailWidget::~ThumbnailWidget()
{
    // 等待子线程退出
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
}

// 处理消息事件
int ThumbnailWidget::dealWithData(const int &msgType, const QString &)
{
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    return 0;
}

// 初始化界面
void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new CustomListWidget(this);

    m_pPageWidget = new PagingWidget(this);

    auto m_pvBoxLayout = new QVBoxLayout(this);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    m_pvBoxLayout->addWidget(m_pPageWidget);

    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);
    this->setLayout(m_pvBoxLayout);
}

//  画 选中项的背景颜色
void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    //  先清空之前的选中颜色
    if (m_pOldThumbnailItemWidget) {
        m_pOldThumbnailItemWidget->setBSelect(false);
    }

    if (item) {
        m_pOldThumbnailItemWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
        if (m_pOldThumbnailItemWidget) {
            m_pOldThumbnailItemWidget->setBSelect(true);
        }

        int nJumpPage = m_pOldThumbnailItemWidget->nPageIndex();
        m_pPageWidget->setCurrentPageValue(nJumpPage);
    }
}

//  添加项
void ThumbnailWidget::addThumbnailItem(const int &iIndex)
{
    ThumbnailItemWidget *widget = new ThumbnailItemWidget;
    widget->setLabelPage(iIndex);
//    widget->setMinimumSize(QSize(250, 250));

    QListWidgetItem *item = new QListWidgetItem(m_pThumbnailListWidget);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(250, 250));

    m_pThumbnailListWidget->addItem(item);
    m_pThumbnailListWidget->setItemWidget(item, widget);
}

//  文件  当前页变化, 获取与 文档页  对应的 item, 设置 选中该item, 绘制item
void ThumbnailWidget::slotFileViewToListPage(const int &page)
{
    QListWidgetItem *curPageItem = m_pThumbnailListWidget->item(page);
    if (curPageItem) {
        m_pThumbnailListWidget->setCurrentRow(page, QItemSelectionModel::NoUpdate);

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

// 初始化缩略图列表list，无缩略图
void ThumbnailWidget::fillContantToList()
{
    for (int idex = 0; idex < m_totalPages; ++idex) {
        addThumbnailItem(idex);
    }

    if (m_totalPages > 0) {
        m_pThumbnailListWidget->setCurrentRow(0, QItemSelectionModel::NoUpdate);

        QListWidgetItem *item = m_pThumbnailListWidget->item(0);
        if (item) {
            setSelectItemBackColor(item);
        }
    }

    m_isLoading = false;
}

// 关联成功打开文件槽函数
void ThumbnailWidget::slotOpenFileOk()
{
    m_isLoading = true;

    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    disconnect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotFileViewToListPage(int)));
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotFileViewToListPage(int)), Qt::QueuedConnection);

    int pages = DocummentProxy::instance()->getPageSNum();

    m_totalPages = pages;

    if (m_pPageWidget) {
        m_pPageWidget->setTotalPages(m_totalPages);
    }
    int counter = m_pThumbnailListWidget->count();
    for (int index = 0; index < counter; index++) {
        QListWidgetItem *item = m_pThumbnailListWidget->takeItem(0);
        delete item;
    }
    m_pThumbnailListWidget->clear();
    fillContantToList();

    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.setStartAndEndIndex();
    }
    m_ThreadLoadImage.setPages(m_totalPages);
    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

// 启动加载缩略图线程
void ThumbnailWidget::slotLoadThumbnailImage()
{
    if (m_ThreadLoadImage.endPage() == ( m_totalPages - 1)) {
//        m_loadImageTimer.stop();
//        m_ThreadLoadImage.setIsLoaded(false);
    }
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.start();
    }
}

/*******************************ThreadLoadImage*************************************************/

ThreadLoadImage::ThreadLoadImage(QObject *parent)
    : QThread (parent)
{

}

void ThreadLoadImage::stopThreadRun()
{
    m_isLoaded = false;
    quit();
//    terminate();    //终止线程
    wait();         //阻塞等待
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

        for (int page = m_nStartPage; page <= m_nEndPage; page++) {
            if (!m_isLoaded)
                break;
            DocummentProxy *dproxy = DocummentProxy::instance();
            if (nullptr == dproxy) {
                break;
            }
            QImage image;
            bool bl = dproxy->getImage(page, image, 113 * 1.5, 143 * 1.5);

            if (bl) {
                emit signal_loadImage(page, image);
            }

            msleep(10);
        }
        m_nStartPage += FIRST_LOAD_PAGES;
        m_nEndPage += FIRST_LOAD_PAGES;

        msleep(30);
    }
}

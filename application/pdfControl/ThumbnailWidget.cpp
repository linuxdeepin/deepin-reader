#include "ThumbnailWidget.h"

ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(QString("ThumbnailWidget"), parent)
{
    m_ThreadLoadImage.setParent(this);
    m_ThreadLoadImage.setThumbnail(this);

    initWidget();

    connect(&m_ThreadLoadImage, SIGNAL(signal_loadImage(int, QImage)), this, SLOT(slot_loadImage(int, QImage)));

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));

//    connect(m_pThumbnailListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

ThumbnailWidget::~ThumbnailWidget()
{
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
}

int ThumbnailWidget::dealWithData(const int &msgType, const QString &)
{
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    return 0;
}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new CustomListWidget(this);

    m_pPageWidget = new PagingWidget;

    auto m_pvBoxLayout = new QVBoxLayout;
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
    widget->setMinimumSize(QSize(250, 250));

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

void ThumbnailWidget::slotCloseFile()
{
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
}

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
    m_pThumbnailListWidget->clear();
    fillContantToList();

    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.setStartAndEndIndex();
    }
    m_ThreadLoadImage.setPages(pages);
    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

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


void ThumbnailWidget::slot_loadImage(int page, QImage image)
{
    m_pThumbnailListWidget->setItemImage(page, image);
}

/*******************************ThreadLoadImage*************************************************/

ThreadLoadImage::ThreadLoadImage(QObject *parent)
    : QThread (parent)
{

}

void ThreadLoadImage::stopThreadRun()
{
    m_isLoaded = false;

//    terminate();    //终止线程
    wait();         //阻塞等待
}

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
            m_isLoaded = true;
            m_nEndPage = m_pages - 1;
        }

        for (int page = m_nStartPage; page <= m_nEndPage; page++) {
            if (!m_isLoaded)
                break;
            QImage image;
            bool bl = DocummentProxy::instance()->getImage(page, image, 113, 143);

            if (bl) {
                emit signal_loadImage(page, image);
            }
        }
        m_nStartPage += FIRST_LOAD_PAGES;
        m_nEndPage += FIRST_LOAD_PAGES;

        msleep(50);
    }
}

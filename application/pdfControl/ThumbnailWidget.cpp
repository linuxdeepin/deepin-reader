#include "ThumbnailWidget.h"
#include <QStringListModel>
#include <QStandardItemModel>
#include "frame/DocummentFileHelper.h"

ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(QString("ThumbnailWidget"), parent)
{
    m_ThreadLoadImage.setThumbnail(this);

    initWidget();

    connect(&m_ThreadLoadImage, SIGNAL(signal_loadImage(const int &, const QImage &)),
            m_pThumbnailListWidget, SLOT(slot_loadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigFilePageChanged(const QString &)), SLOT(slotDocFilePageChanged(const QString &)));
}

ThumbnailWidget::~ThumbnailWidget()
{
    // 等待子线程退出
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
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
    }

    return 0;
}

// 初始化界面
void ThumbnailWidget::initWidget()
{
    DPalette plt = DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(QPalette::Background, plt.color(QPalette::Base));
    setAutoFillBackground(true);
    setPalette(plt);

    m_pThumbnailListWidget = new CustomListWidget(this);
    m_pThumbnailListWidget->setSpacing(3);

    m_pPageWidget = new PagingWidget(this);

    auto m_pvBoxLayout = new QVBoxLayout(this);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    m_pvBoxLayout->addWidget(m_pPageWidget);

    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(6);
    this->setLayout(m_pvBoxLayout);
}

//  画 选中项的背景颜色
void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    //  先清空之前的选中颜色
    auto curItem = m_pThumbnailListWidget->currentItem();
    if (curItem) {
        auto itemWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(curItem));
        if (itemWidget) {
            itemWidget->setBSelect(false);
        }
    }

    if (item) {
        m_pThumbnailListWidget->setCurrentItem(item);
        auto pWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
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
    auto widget = new ThumbnailItemWidget;
    widget->setLabelPage(iIndex);

    auto item = new QListWidgetItem(m_pThumbnailListWidget);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(240, 212));

    m_pThumbnailListWidget->addItem(item);
    m_pThumbnailListWidget->setItemWidget(item, widget);
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
    DPalette plt = DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(QPalette::Background, plt.color(QPalette::Base));
    setAutoFillBackground(true);
    setPalette(plt);
}

// 初始化缩略图列表list，无缩略图
void ThumbnailWidget::fillContantToList()
{
    for (int idex = 0; idex < m_totalPages; ++idex) {
        addThumbnailItem(idex);
    }

    if (m_totalPages > 0) {
        auto item = m_pThumbnailListWidget->item(0);
        if (item) {
            m_pThumbnailListWidget->setCurrentItem(item);

            auto pWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
            if (pWidget) {
                pWidget->setBSelect(true);
            }
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

    int pages = DocummentFileHelper::instance()->getPageSNum();

    m_totalPages = pages;

    if (m_pPageWidget) {
        m_pPageWidget->setTotalPages(m_totalPages);
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
    //    if (m_ThreadLoadImage.endPage() == ( m_totalPages - 1)) {
    //        m_loadImageTimer.stop();
    //        m_ThreadLoadImage.setIsLoaded(false);
    //    }
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
            auto dproxy = DocummentFileHelper::instance();
            if (nullptr == dproxy) {
                break;
            }
            QImage image;
            bool bl = dproxy->getImage(page, image, 132, 160/*134, 152*/);
            if (bl) {
                emit signal_loadImage(page, image);
            }
        }
        m_nStartPage += FIRST_LOAD_PAGES;
        m_nEndPage += FIRST_LOAD_PAGES;

        msleep(50);
    }
}

#include "ThumbnailWidget.h"

ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(QString("ThumbnailWidget"), parent)
{
    m_ThreadLoadImage.setParent(this);
    m_ThreadLoadImage.setThumbnail(this);

    m_pvBoxLayout = new QVBoxLayout;
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);

    initWidget();

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigJumpIndexPage(int)), this, SLOT(slotJumpIndexPage(int)));
    connect(m_pThumbnailListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
}

ThumbnailWidget::~ThumbnailWidget()
{
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    if (m_pThumbnailListWidget) {
        m_pThumbnailListWidget->clear();
    }
}

int ThumbnailWidget::dealWithData(const int &msgType, const QString &msgContant)
{
    if (MSG_THUMBNAIL_JUMPTOPAGE == msgType) {
        int row = msgContant.toInt();

        emit sigJumpIndexPage(row);

        return ConstantMsg::g_effective_res;
    } else if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    return 0;
}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new DListWidget(this);
    m_pThumbnailListWidget->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pThumbnailListWidget->setResizeMode(QListWidget::Adjust);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    //m_pThumbnailListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pPageWidget = new PagingWidget;
    m_pvBoxLayout->addWidget(m_pPageWidget);

    this->setLayout(m_pvBoxLayout);
}

void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    QPalette p;

    if (m_pSonWidgetPageLabel && m_pSonWidgetContantLabel && item) {
        p.setColor(QPalette::Text, QColor(0, 0, 0));

        m_pSonWidgetPageLabel->setPalette(p);

        m_pSonWidgetContantLabel->setFrameShape (QFrame::NoFrame);
        m_pSonWidgetContantLabel->setPalette(p);
        m_pSonWidgetContantLabel->setLineWidth(1);
    }

    if (item) {
        DLabel *t_pageLab = nullptr;
        DLabel *t_contantLab = nullptr;
        ThumbnailItemWidget *t_ItemWidget = nullptr;

        t_ItemWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));

        p.setColor(QPalette::Text, QColor(30, 144, 255));

        t_contantLab = t_ItemWidget->getContantLabel();
        t_contantLab->setLineWidth(3);
        t_contantLab->setPalette(p);

        t_pageLab = t_ItemWidget->getPageLabel();
        t_pageLab->setPalette(p);

        m_pSonWidgetContantLabel = t_contantLab;
        m_pSonWidgetPageLabel = t_pageLab;
        m_pThumbnailItemWidget = t_ItemWidget;
    }
}

void ThumbnailWidget::setCurrentRow(const int &row)
{
    slotFileViewToListPage(row);

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::addThumbnailItem(const int &idex)
{
    ThumbnailItemWidget *widget = new ThumbnailItemWidget;
    widget->setPageLabelText(QString("%1").arg(idex + 1));
    widget->setMinimumSize(QSize(250, 250));

    QListWidgetItem *item = new QListWidgetItem(m_pThumbnailListWidget);
    item->setFlags(Qt::NoItemFlags);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 250));

    m_pThumbnailListWidget->addItem(item);
    m_pThumbnailListWidget->setItemWidget(item, widget);
}

void ThumbnailWidget::slotFileViewToListPage(int page)
{
    if (this->preRowVal() == page) {
        return;
    }

    this->setPreRowVal(page);
    m_pThumbnailListWidget->setCurrentRow(page, QItemSelectionModel::NoUpdate);
    QListWidgetItem *item = m_pThumbnailListWidget->item(page);
    setSelectItemBackColor(item);

    if (m_pPageWidget) {
        m_pPageWidget->setPageValue(page);
    }
}

void ThumbnailWidget::slotCloseFile()
{
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
}

bool ThumbnailWidget::fillContantToList()
{
    for (int idex = 0; idex < totalPages(); ++idex) {

        addThumbnailItem(idex);
    }

    m_isLoading = false;

    return true;
}

void ThumbnailWidget::loadImage(const int &page, QImage &image)
{
    ThumbnailItemWidget *t_ItemWidget = nullptr;
    QListWidgetItem *item = nullptr;
    item = m_pThumbnailListWidget->item(page);
    if (item) {
        t_ItemWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
        if (t_ItemWidget) {
//            qDebug() << tr("loading       page:%1   image").arg(page + 1);
            t_ItemWidget->setContantLabelPixmap(image);
        }
    }
}

void ThumbnailWidget::slotShowSelectItem(QListWidgetItem *item)
{
    int row = m_pThumbnailListWidget->row(item);

    if (this->preRowVal() == row) {
        return;
    }

    this->setPreRowVal(row);

    setSelectItemBackColor(item);

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::slotOpenFileOk()
{
    m_isLoading = true;

    m_pSonWidgetContantLabel = nullptr;
    m_pSonWidgetPageLabel = nullptr;
    m_pThumbnailItemWidget = nullptr;

    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotFileViewToListPage(int)), Qt::QueuedConnection);

    int pages = DocummentProxy::instance()->getPageSNum();

    if (pages < FIRSTPAGES) {
        return;
    }

    if (m_pPageWidget) {
        m_pPageWidget->setTotalPages(pages);
    }

    if (m_pThumbnailListWidget) {
        m_pThumbnailListWidget->clear();
    }

    setTotalPages(pages);

    fillContantToList();

    if (!m_ThreadLoadImage.isRunning()) {

        m_ThreadLoadImage.setStartAndEndIndex();
    }
    m_ThreadLoadImage.setPages(pages);
    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

void ThumbnailWidget::slotJumpIndexPage(int index)
{
    setCurrentRow(index);
}

void ThumbnailWidget::slotLoadThumbnailImage()
{
    if (m_ThreadLoadImage.endPage() == (totalPages() - 1)) {
//        m_loadImageTimer.stop();
//        m_ThreadLoadImage.setIsLoaded(false);
    }
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.start();
    }
}

/*******************************ThreadLoadImage*************************************************/

ThreadLoadImage::ThreadLoadImage()
{

}

void ThreadLoadImage::stopThreadRun()
{
    m_isLoaded = false;

    terminate();    //终止线程
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
                m_pThumbnailWidget->loadImage(page, image);
            }
        }
        m_nStartPage += FIRST_LOAD_PAGES;
        m_nEndPage += FIRST_LOAD_PAGES;

        msleep(50);
    }
}

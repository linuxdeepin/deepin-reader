#include "ThumbnailWidget.h"

ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget("ThumbnailWidget", parent)
{
    m_pvBoxLayout = new QVBoxLayout;
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);

    initWidget();

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigJumpIndexPage(int)), this, SLOT(slotJumpIndexPage(int)));
    connect(m_pThumbnailListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
    connect(&m_loadImageTimer, SIGNAL(timeout()), this, SLOT(loadThumbnailImage()));
}

int ThumbnailWidget::dealWithData(const int &msgType, const QString &msgContant)
{
    if (MSG_THUMBNAIL_JUMPTOPAGE == msgType) {
        int row = msgContant.toInt();

        emit sigJumpIndexPage(row);

        return ConstantMsg::g_effective_res;
    } else if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
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
    //m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
        //t_contantLab->setFrameShape (QFrame::Box);
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
    if (this->preRowVal() == row) {
        return;
    }

    this->setPreRowVal(row);
    m_pThumbnailListWidget->setCurrentRow(row, QItemSelectionModel::NoUpdate);
    QListWidgetItem *item = m_pThumbnailListWidget->item(row);
    setSelectItemBackColor(item);

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::addThumbnailItem(const QImage &image, const int &idex)
{
    QListWidgetItem *item = new QListWidgetItem;
    ThumbnailItemWidget *widget = new ThumbnailItemWidget;

    //widget->setContantLabelPixmap(image);
    widget->setPageLabelText(tr("%1").arg(idex + 1));
    widget->setMinimumSize(QSize(250, 250));

    item->setFlags(Qt::NoItemFlags);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 250));

    m_pThumbnailListWidget->addItem(item);
    m_pThumbnailListWidget->setItemWidget(item, widget);
}

bool ThumbnailWidget::fillContantToList()
{
    for (int idex = 0; idex < totalPages(); ++idex) {
        QImage image;
        //DocummentProxy::instance()->getImage(idex, image, 113, 143);

        addThumbnailItem(image, idex);
    }

    return true;
}

void ThumbnailWidget::slotShowSelectItem(QListWidgetItem *item)
{
    int row = m_pThumbnailListWidget->row(item);

    if (this->preRowVal() == row) {
        return;
    }

    this->setPreRowVal(row);

    setSelectItemBackColor(item);

    sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(row));

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::slotOpenFileOk()
{
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotJumpIndexPage(int)), Qt::QueuedConnection);

    int pages = DocummentProxy::instance()->getPageSNum();

    if (pages < FIRSTPAGES) {
        return;
    }

    if (m_pPageWidget)
        m_pPageWidget->setTotalPages(pages);

    if (m_pThumbnailListWidget)
        m_pThumbnailListWidget->clear();

    m_ThreadLoadImage.setPages(pages);
    m_ThreadLoadImage.start();

    setTotalPages(pages);

    fillContantToList();
    m_loadImageTimer.start(1);
}

void ThumbnailWidget::slotJumpIndexPage(int index)
{
    setCurrentRow(index);
}

void ThumbnailWidget::loadThumbnailImage()
{
    if (m_ThreadLoadImage.isLoaded()) {

//        const int nPageSpace = 20;
//        static int nStartPage = 0;
//        static int nEndPage = 19;

//        if (nEndPage > totalPages()) {
//            m_loadImageTimer.stop();
//            nEndPage = totalPages();
//        }

        for (int idex = 0; idex < totalPages(); ++idex) {
            QImage image;

            ThumbnailItemWidget *t_ItemWidget = nullptr;
            QListWidgetItem *item = nullptr;
            item = m_pThumbnailListWidget->item(idex);
            if (item) {
                t_ItemWidget = reinterpret_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
                if (t_ItemWidget) {
                    m_ThreadLoadImage.getPageImage(idex, image);
                    t_ItemWidget->setContantLabelPixmap(image);
                }
            }
        }

//        nStartPage += nPageSpace;
//        nEndPage += nPageSpace;
        m_loadImageTimer.stop();
    }
}

/*******************************ThreadLoadImage*************************************************/

ThreadLoadImage::ThreadLoadImage()
{

}

void ThreadLoadImage::getPageImage(const int &page, QImage &image)
{
    if (m_imageMap.contains(page)) {
        image = m_imageMap[page];
    }
}

void ThreadLoadImage::run()
{
    m_isLoaded = false;
    for (int page = 0; page < m_pages; page++) {
        QImage image;
        bool bl = DocummentProxy::instance()->getImage(page, image, 113, 143);

        if (bl) {
            m_imageMap[page] = image;
        }
    }

    m_isLoaded = true;
    qDebug() << tr("ThreadLoadImage::run");
}

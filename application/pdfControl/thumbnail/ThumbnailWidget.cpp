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
#include "ThumbnailWidget.h"

#include "docview/docummentproxy.h"
#include "DocSheet.h"
#include "application.h"

ThumbnailWidget::ThumbnailWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    m_ThreadLoadImage.setThumbnail(this);

    initWidget();

    initConnection();
}

ThumbnailWidget::~ThumbnailWidget()
{
    // 等待子线程退出
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
        m_ThreadLoadImage.clearList();
    }
}

void ThumbnailWidget::handleOpenSuccess()
{
    slotOpenFileOk(m_sheet->filePath());
    m_pPageWidget->handleOpenSuccess();
}

void ThumbnailWidget::handleRotate(int rotate)
{
    m_nRotate = rotate;
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

void ThumbnailWidget::handlePage(int page)
{
    m_page = page;
    auto curPageItem = m_pThumbnailListWidget->item(page);
    if (curPageItem) {
        auto curItem = m_pThumbnailListWidget->currentItem();
        if (curItem == curPageItem)
            return;

        setSelectItemBackColor(curPageItem);
    }

    m_pPageWidget->setPage(page);
}

bool ThumbnailWidget::isLoading()
{
    return m_isLoading;
}

// 初始化界面
void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new CustomListWidget(m_sheet, this);
    m_pThumbnailListWidget->setSpacing(8);
    connect(m_pThumbnailListWidget, SIGNAL(sigValueChanged(const int &)), SLOT(slotLoadThumbnail(const int &)));

    m_pPageWidget = new PagingWidget(m_sheet, this);

    auto m_pvBoxLayout = new QVBoxLayout(this);
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);

    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    m_pvBoxLayout->addWidget(new DHorizontalLine(this));
    m_pvBoxLayout->addWidget(m_pPageWidget);

    this->setLayout(m_pvBoxLayout);
}

//  画 选中项的背景颜色
void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    //  先清空之前的选中颜色
    auto curItem = m_pThumbnailListWidget->currentItem();
    if (curItem) {
        SetSelectItemColor(curItem, false);
    }

    if (item) {
        SetSelectItemColor(item, true);
    }
}

//  添加项
void ThumbnailWidget::addThumbnailItem(const int &iIndex)
{
    auto widget = new ThumbnailItemWidget(this);

    widget->setLabelPage(iIndex);

    auto item = new QListWidgetItem(m_pThumbnailListWidget);

    item->setFlags(Qt::NoItemFlags);

    item->setSizeHint(QSize(200, 200));

    m_pThumbnailListWidget->addItem(item);

    m_pThumbnailListWidget->setItemWidget(item, widget);

    widget->adaptWindowSize(dApp->scale());
}

void ThumbnailWidget::initConnection()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &ThumbnailWidget::slotUpdateTheme);

    connect(&m_ThreadLoadImage, SIGNAL(sigLoadImage(const int &, const QImage &)), this, SLOT(slotLoadImage(const int &, const QImage &)));
    connect(&m_ThreadLoadImage, SIGNAL(sigRotateImage(const int &)), SLOT(slotRotateThumbnail(const int &)));
}

void ThumbnailWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

void ThumbnailWidget::slotRotateThumbnail(const int &index)
{
    if (m_pThumbnailListWidget) {
        if (index >= m_pThumbnailListWidget->count()) {
            return;
        }
        auto item = m_pThumbnailListWidget->item(index);
        if (item) {
            auto pWidget = getItemWidget(item);
            if (pWidget) {
                pWidget->rotateThumbnail(m_nRotate);
            }
        }
    }
}

/**
 * @brief ThumbnailWidget::slotLoadThumbnail
 * 根据纵向滚动条来加载缩略图
 * value 为当前滚动条移动的数值,最小是0，最大是滚动条的最大值
 */
void ThumbnailWidget::slotLoadThumbnail(const int &value)
{
    if (m_nValuePreIndex < 1 || value < 1)
        return;

    int indexPage = value / m_nValuePreIndex;

    if (indexPage < 0 || indexPage > m_totalPages || m_ThreadLoadImage.inLoading(indexPage)) {
        return;
    }

    int loadStart = (indexPage - (FIRST_LOAD_PAGES / 2)) < 0 ? 0 : (indexPage - (FIRST_LOAD_PAGES / 2));
    int loadEnd = (indexPage + (FIRST_LOAD_PAGES / 2)) > m_totalPages ? m_totalPages : (indexPage + (FIRST_LOAD_PAGES / 2));
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }
    m_ThreadLoadImage.setStartAndEndIndex(loadStart, loadEnd);
    m_ThreadLoadImage.setIsLoaded(true);
    m_ThreadLoadImage.start();
}

void ThumbnailWidget::slotLoadImage(const int &row, const QImage &image)
{
    if (!m_pThumbnailListWidget) {
        return;
    }

    auto item = m_pThumbnailListWidget->item(row);
    if (item) {
        auto pWidget = getItemWidget(item);

        if (pWidget) {
            pWidget->rotateThumbnail(m_nRotate);
            pWidget->setLabelImage(image);
        }
    }
}

void ThumbnailWidget::SlotSetBookMarkState(const int &iType, const int &iPage)
{
    bool bStatus = false;
    if (iType == -1) {
    } else {
        bStatus = true;
    }

    auto item = m_pThumbnailListWidget->item(iPage);
    if (item) {
        auto pWidget = getItemWidget(item);
        if (pWidget) {
            pWidget->qSetBookMarkShowStatus(bStatus);
        }
    }
}

// 初始化缩略图列表list，无缩略图
void ThumbnailWidget::fillContantToList()
{
    if (m_totalPages > 0) {
        for (int idex = 0; idex < m_totalPages; ++idex) {
            addThumbnailItem(idex);
        }

        showItemBookMark();
        auto item = m_pThumbnailListWidget->item(0);
        if (item) {
            SetSelectItemColor(item, true);
        }
        /**
         * @brief listCount
         * 计算每个item的大小
         */
        int listCount = m_pThumbnailListWidget->count();
        m_nValuePreIndex = m_pThumbnailListWidget->verticalScrollBar()->maximum() / listCount;
    }

    m_isLoading = false;
}

void ThumbnailWidget::showItemBookMark()
{
    QList<int> pageList = dApp->m_pDBService->getBookMarkList(m_sheet->filePath());
    foreach (int index, pageList) {
        auto pWidget = getItemWidget(m_pThumbnailListWidget->item(index));
        if (pWidget) {
            pWidget->qSetBookMarkShowStatus(true);
        }
    }
}

/**
 * @brief ThumbnailWidget::prevPage
 * 上一页
 */
void ThumbnailWidget::prevPage()
{
    if (m_sheet.isNull())
        return;

    m_sheet->pagePrev();
}

/**
 * @brief ThumbnailWidget::nextPage
 下一页
 */
void ThumbnailWidget::nextPage()
{
    if (m_sheet.isNull())
        return;

    m_sheet->pageNext();
}

/**
 * brief ThumbnailWidget::adaptWindowSize
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void ThumbnailWidget::adaptWindowSize(const double &scale)
{
    double width = static_cast<double>(LEFTMINWIDTH) * scale;
    double height = static_cast<double>(212) * scale;

    if (m_pThumbnailListWidget) {
        int itemCount = 0;
        itemCount = m_pThumbnailListWidget->count();
        if (itemCount > 0) {
            for (int index = 0; index < itemCount; index++) {
                auto item = m_pThumbnailListWidget->item(index);
                if (item) {
                    auto itemWidget = getItemWidget(item);
                    if (itemWidget) {
                        itemWidget->adaptWindowSize(scale);
                        item->setSizeHint(QSize(static_cast<int>(width), static_cast<int>(height)));
                    }
                }
            }
        }

        m_pThumbnailListWidget->scrollTo(m_pThumbnailListWidget->currentIndex());
    }

}

/**
 * @brief ThumbnailWidget::updateThumbnail
 * 高亮操作之后要跟换相应的缩略图
 * @param page 页码数，从0开始
 */
void ThumbnailWidget::updateThumbnail(const int &page)
{
    if (m_pThumbnailListWidget == nullptr) {
        return;
    }
    int itemNum = m_pThumbnailListWidget->count();
    if (itemNum <= 0) {
        return;
    }
    for (int index = 0; index < itemNum; index++) {
        auto item = m_pThumbnailListWidget->item(index);
        if (item) {
            auto itemWidget = getItemWidget(item);
            if (itemWidget) {
                if (itemWidget->nPageIndex() == page) {
                    auto dproxy = m_sheet->getDocProxy();
                    if (nullptr == dproxy) {
                        return;
                    }
                    QImage image;
                    dproxy->getImageMax(page, image, 174);
                    itemWidget->setLabelImage(image);
                    return;
                }
            }
        }
    }
}

// 关联成功打开文件槽函数
void ThumbnailWidget::slotOpenFileOk(const QString &sPath)
{
    DocummentProxy *_proxy = m_sheet->getDocProxy();
    if (_proxy) {
        m_isLoading = true;
        m_ThreadLoadImage.setIsLoaded(false);
        if (m_ThreadLoadImage.isRunning()) {
            m_ThreadLoadImage.stopThreadRun();
        }

        m_totalPages = _proxy->getPageSNum();

        m_pThumbnailListWidget->clear();
        m_nValuePreIndex = 0;
        fillContantToList();

        m_nRotate = m_sheet->getOper(Rotate).toInt();

        if (m_nRotate < 0) {
            m_nRotate = qAbs(m_nRotate);
        }
        m_nRotate %= 360;

        int currentPage = _proxy->currentPageNo();

        m_ThreadLoadImage.setPages(m_totalPages);
        m_ThreadLoadImage.setProxy(_proxy);
        m_ThreadLoadImage.setFilePath(sPath);
        if (!m_ThreadLoadImage.isRunning()) {
            m_ThreadLoadImage.clearList();;
            m_ThreadLoadImage.setStartAndEndIndex(currentPage - (FIRST_LOAD_PAGES / 2), currentPage + (FIRST_LOAD_PAGES / 2));
        }
        m_ThreadLoadImage.setIsLoaded(true);
        m_ThreadLoadImage.start();
        handlePage(currentPage);
    }
}

void ThumbnailWidget::SetSelectItemColor(QListWidgetItem *item, const bool &bSelect)
{
    auto pWidget = getItemWidget(item);
    if (pWidget) {
        pWidget->setBSelect(bSelect);
    }

    if (bSelect) {
        m_pThumbnailListWidget->setCurrentItem(item);
    }
}

ThumbnailItemWidget *ThumbnailWidget::getItemWidget(QListWidgetItem *item)
{
    auto pWidget = qobject_cast<ThumbnailItemWidget *>(m_pThumbnailListWidget->itemWidget(item));
    if (pWidget) {
        return pWidget;
    }
    return nullptr;
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
    wait();  //阻塞等待
}

bool ThreadLoadImage::inLoading(int index)
{
    if (index >=  m_nStartPage && index < m_nEndPage) {
        return true;
    }

    return false;
}

void ThreadLoadImage::setStartAndEndIndex(int startIndex, int endIndex)
{
    m_nStartPage = startIndex;  // 加载图片起始页码
    m_nEndPage = endIndex;   // 加载图片结束页码

    if (m_nStartPage < 0) {
        m_nStartPage = 0;
    }
    if (m_nEndPage >= m_pages) {
        m_nEndPage = m_pages - 1;
    }
}

// 加载缩略图线程
void ThreadLoadImage::run()
{
    do {
        msleep(50);

        if (!m_pThumbnailWidget) {
            break;
        }

        if (m_pThumbnailWidget->isLoading()) {
            break;
        }

        if (m_nStartPage < 0) {
            m_nStartPage = 0;
        }
        if (m_nEndPage >= m_pages) {
            m_nEndPage = m_pages - 1;
        }

        if (nullptr == m_proxy) {
            break;
        }

        for (int page = m_nStartPage; page <= m_nEndPage; page++) {
            if (!m_isLoaded)
                break;

            if (m_listLoad.contains(page)) {
                emit sigRotateImage(page);
                msleep(50);
                continue;
            }
            QImage image;

            bool bl = m_proxy->getImageMax(page, image, 174);

            if (bl) {
                m_listLoad.append(page);
                emit sigLoadImage(page, image);
                msleep(50);
            }
        }

    } while (0);
}

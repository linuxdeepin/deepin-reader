#include "CustomListWidget.h"
#include "CustomItemWidget.h"
#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"

CustomListWidget::CustomListWidget(DWidget *parent)
    : DListWidget(parent)
{
    this->setMinimumWidth(LEFTMINWIDTH);
    this->setMaximumWidth(LEFTMAXWIDTH);
    resize(LEFTMINWIDTH, this->height());

    setSpacing(0);
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListWidget::Adjust);
    setViewMode(QListView::ListMode);
    setSelectionMode(QAbstractItemView::SingleSelection);
    sortItems(Qt::AscendingOrder);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(slotShowSelectItem(QListWidgetItem *)));
}

//  插入 实现排序
QListWidgetItem *CustomListWidget::insertWidgetItem(const int &iData)
{
    int iInsertIndex = 0;

    //  计算 新增的书签  插入位置
    int nCount = this->count();
    while (nCount > 0) {
        nCount--;

        auto nextItem = this->item(nCount);
        if (nextItem) {
            int nextItemData = nextItem->data(Qt::UserRole + 1).toInt();
            if (nextItemData < iData) {
                nCount++;
                break;
            }
        }
    }

    iInsertIndex = nCount;
    auto item = new QListWidgetItem;
    item->setData(Qt::UserRole + 1, iData);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(230, 80));

    this->insertItem(iInsertIndex, item);

    return item;
}

/**
 * @brief CustomListWidget::setItemImage
 * 填充缩略图
 * @param row
 * @param image
 */
void CustomListWidget::slot_loadImage(const int &row, const QImage &image)
{
    auto item = this->item(row);
    if (item) {
        auto t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
        if (t_ItemWidget) {
            t_ItemWidget->setLabelImage(image);
        }
    }
}

/**
 * @brief CustomListWidget::slotShowSelectItem
 * 鼠标点击当前缩略图item，右边fileview中同步到当前页
 * @param item
 */
void CustomListWidget::slotShowSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }
    auto t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
    if (t_ItemWidget) {
        int nJumpPage = t_ItemWidget->nPageIndex();
        int nCurPage = DocummentFileHelper::instance()->currentPageNo();
        if (nCurPage != nJumpPage) {
            //  页跳转
            DocummentFileHelper::instance()->pageJump(nJumpPage);
        }
        emit sigSelectItem(item);
    }
}

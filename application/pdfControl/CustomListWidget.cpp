#include "CustomListWidget.h"
#include "CustomItemWidget.h"
#include "docview/docummentproxy.h"

CustomListWidget::CustomListWidget(DWidget *parent)
    : DListWidget (parent)
{
    setSpacing(0);
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListWidget::Adjust);
    setViewMode(QListView::ListMode);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
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
    auto t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
    if (t_ItemWidget) {
        int nJumpPage = t_ItemWidget->nPageIndex();
        int nCurPage = DocummentProxy::instance()->currentPageNo();
        if (nCurPage != nJumpPage) {
            //  页跳转
            DocummentProxy::instance()->pageJump(nJumpPage);
        }
        emit sigSelectItem(item);
    }
}


#include "CustomListWidget.h"
#include "CustomItemWidget.h"
#include "docview/docummentproxy.h"

CustomListWidget::CustomListWidget(DWidget *parent)
    : DListWidget (parent)
{
    setFocusPolicy(Qt::NoFocus);
    setSpacing(5);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

void CustomListWidget::setItemImage(const int &row, QImage &image)
{
    QListWidgetItem *item = this->item(row);
    if (item) {
        CustomItemWidget *t_ItemWidget = reinterpret_cast<CustomItemWidget *>(this->itemWidget(item));
        if (t_ItemWidget) {
            t_ItemWidget->setLabelImage(image);
        }
    }
}

//  单击 跳转
void CustomListWidget::slotShowSelectItem(QListWidgetItem *item)
{
    int nJumpPage = item->data(Qt::UserRole + 1).toInt();
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    if (nCurPage != nJumpPage) {
        //  页跳转
        DocummentProxy::instance()->pageJump(nJumpPage);
    }
}


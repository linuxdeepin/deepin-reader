#include "CustomItemWidget.h"

CustomItemWidget::CustomItemWidget(const QString &name, CustomWidget *parent)
    : CustomWidget (name, parent)
{

}

/**
 * @brief CustomItemWidget::setLabelImage
 * 给新label填充缩略图
 * @param image
 */
void CustomItemWidget::setLabelImage(const QImage &image)
{
    if (m_pPicture != nullptr) {
        m_pPicture->setPixmap(QPixmap::fromImage(image));
    }
}

/**
 * @brief CustomItemWidget::setLabelPage
 * 设置页码标签内容
 * @param value
 * @param nShowPage
 */
void CustomItemWidget::setLabelPage(const int &value,  const int &nShowPage )
{
    m_nPageIndex = value;

    if (m_pPageNumber) {
        int nnPage = value + 1;
        if (nShowPage == 1) {
            QString sPageText = tr("page") + QString("%1").arg(nnPage);
            m_pPageNumber->setText(sPageText);
        } else {
            m_pPageNumber->setText(QString("%1").arg(nnPage));
        }
    }
}

/**
 * @brief CustomItemWidget::nPageIndex
 * 获取当前注释缩略图index
 * @return
 */
int CustomItemWidget::nPageIndex() const
{
    return m_nPageIndex;
}

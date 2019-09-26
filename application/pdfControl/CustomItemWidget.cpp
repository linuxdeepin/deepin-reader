#include "CustomItemWidget.h"

CustomItemWidget::CustomItemWidget(const QString &name, CustomWidget *parent)
    : CustomWidget (name, parent)
{

}

void CustomItemWidget::setLabelImage(const QImage &image)
{
    if (m_pPicture != nullptr) {
        m_pPicture->setPixmap(QPixmap::fromImage(image));
    }
}

#include "CustomItemWidget.h"
#include "translator/PdfControl.h"

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

void CustomItemWidget::setLabelPage(const int &value,  const int &nShowPage )
{
    m_nPageIndex = value;
    if (m_pPage) {
        int nnPage = value + 1;
        if (nShowPage == 1) {
            QString sPageText = PdfControl::PAGE_PREF + QString("%1").arg(nnPage);
            m_pPage->setText(sPageText);
        } else {
            m_pPage->setText(QString("%1").arg(nnPage));
        }
    }
}

int CustomItemWidget::nPageIndex() const
{
    return m_nPageIndex;
}

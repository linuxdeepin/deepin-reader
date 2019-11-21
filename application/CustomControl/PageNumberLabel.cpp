#include "PageNumberLabel.h"

PageNumberLabel::PageNumberLabel(DWidget *parent)
    : DLabel (parent)
{

}

void PageNumberLabel::setSelect(const bool &select)
{
//    m_bSelect = select;
    if (select) {
        setForegroundRole(DPalette::Highlight);
    } else {
        setForegroundRole(DPalette::TextTitle);
    }
    update();
}

//void PageNumberLabel::paintEvent(QPaintEvent *e)
//{
////    QPalette pPage(this->palette());
////    QColor color;
//    //  涉及到 主题颜色
//    if (m_bSelect) {
////        color = QColor(QString("#0081FF"));
////        pPage.setColor(QPalette::Text, pPage.highlight().color());
//        setForegroundRole(DPalette::Highlight);
//    } else {
////        color = QColor::fromRgbF(0, 0, 0, 0.08);
////        pPage.setColor(QPalette::Text, QColor(QString("#303030")));
//        setForegroundRole(DPalette::TextTitle);
//    }

//    DLabel::paintEvent(e);
////    this->setPalette(pPage);
//}

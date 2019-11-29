#include "PageNumberLabel.h"

PageNumberLabel::PageNumberLabel(DWidget *parent)
    : DLabel(parent)
{

}

void PageNumberLabel::setSelect(const bool &select)
{
    if (select) {
        setForegroundRole(DPalette::Highlight);
    } else {
        setForegroundRole(DPalette::TextTitle);
    }
    update();
}

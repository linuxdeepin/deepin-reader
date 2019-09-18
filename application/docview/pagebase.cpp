#include "pagebase.h"

PageBase::PageBase(DWidget *parent)
    : DLabel(parent)
{
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
}

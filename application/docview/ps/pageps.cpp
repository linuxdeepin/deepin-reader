#include "pageps.h"
#include "docview/publicfunc.h"
#include "docummentps.h"
#include <QQueue>
#include <QDebug>

PagePS::PagePS(QWidget *parent)
    : PageBase(new PagePSPrivate(this), parent)
{
}

void PagePS::setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno)
{
    Q_D(PagePS);
    d->setPage(page, renderContext, pageno);
}

bool PagePS::getSlideImage(QImage &image, double &width, double &height)
{
    Q_D(PagePS);
    return d->getSlideImage(image, width, height);
}

bool PagePS::getImage(QImage &image, double width, double height)
{
    Q_D(PagePS);
    return d->getImage(image, width, height);
}

PageInterface *PagePS::getInterFace()
{
    Q_D(PagePS);
    return d;
}


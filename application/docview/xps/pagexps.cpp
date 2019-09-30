#include "pagexps.h"
#include "docview/publicfunc.h"
#include <QQueue>
#include <QDebug>

PageXPS::PageXPS(QWidget *parent)
    : PageBase(new PageXPSPrivate(this), parent)
{
}

void PageXPS::setPage(XpsPage *page, int pageno)
{
    Q_D(PageXPS);
    d->setPage(page, pageno);
}

bool PageXPS::getSlideImage(QImage &image, double &width, double &height)
{

    Q_D(PageXPS);
    return d->getSlideImage(image, width, height);
}

bool PageXPS::getImage(QImage &image, double width, double height)
{
    Q_D(PageXPS);
    return d->getImage(image, width, height);
}

PageInterface *PageXPS::getInterFace()
{
    Q_D(PageXPS);
    return d;
}


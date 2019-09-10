#include <QRectF>

#include "documentlayout.h"
#include "pageitem.h"
#include "viewsettings.h"

const qreal viewportPadding = 6.0f;
ViewSettings* DocumentLayout::s_settings=0;

DocumentLayout::DocumentLayout()
{
    if(s_settings == 0)
    {
        s_settings = ViewSettings::instance();
    }
}

DocumentLayout *DocumentLayout::fromLayoutMode(LayoutMode layoutMode)
{
    switch(layoutMode)
    {
    default:
    case SinglePageMode: return new SinglePageLayout;
        //    case TwoPagesMode: return new TwoPagesLayout;
        //    case TwoPagesWithCoverPageMode: return new TwoPagesWithCoverPageLayout;
        //    case MultiplePagesMode: return new MultiplePagesLayout;
    }
}

bool DocumentLayout::isCurrentPage(const QRectF &visibleRect, const QRectF &pageRect) const
{
    // Works with vertically scrolling layouts, i.e. all currently implemented layouts.
    const qreal pageVisibleHeight = pageRect.intersected(visibleRect).height();
    const qreal pageTopOffset = pageRect.top() - visibleRect.top();

    if(visibleRect.height() > 2.0f * pageRect.height()) // Are more than two pages visible?
    {
        const qreal halfPageHeight = 0.5f * pageRect.height();

        return pageVisibleHeight >= halfPageHeight && pageTopOffset < halfPageHeight && pageTopOffset >= -halfPageHeight;
    }
    else
    {
        return pageVisibleHeight >= 0.5f * visibleRect.height();
    }
}

qreal DocumentLayout::visibleHeight(int viewportHeight) const
{
    const qreal pageSpacing = s_settings->documentView().pageSpacing();;

    return viewportHeight - 2.0f * pageSpacing;
}


int SinglePageLayout::currentPage(int page) const
{
    return page;
}

int SinglePageLayout::previousPage(int page) const
{
    return qMax(page - 1, 1);
}

int SinglePageLayout::nextPage(int page, int count) const
{
    return qMin(page + 1, count);
}

QPair<int, int> SinglePageLayout::prefetchRange(int page, int count) const
{
    const int prefetchDistance =s_settings->documentView().prefetchDistance();;

    return qMakePair(qMax(page - prefetchDistance / 2, 1),
                     qMin(page + prefetchDistance, count));
}

int SinglePageLayout::leftIndex(int index) const
{
    return index;
}

int SinglePageLayout::rightIndex(int index, int count) const
{
    Q_UNUSED(count);

    return index;
}

qreal SinglePageLayout::visibleWidth(int viewportWidth) const
{
    const qreal pageSpacing = s_settings->documentView().pageSpacing();

    return viewportWidth - viewportPadding - 2.0f * pageSpacing;
}

void SinglePageLayout::prepareLayout(const QVector<PageItem *> &pageItems, bool rightToLeft, qreal &left, qreal &right, qreal &height)
{
    const qreal pageSpacing = s_settings->documentView().pageSpacing();
    qreal pageHeight = 0.0f;

    for(int index = 0; index < pageItems.count(); ++index)
    {
        PageItem* page = pageItems.at(index);
        const QRectF boundingRect = page->boundingRect();

        page->setPos(-boundingRect.left() - 0.5f * boundingRect.width(), height - boundingRect.top());

        pageHeight = boundingRect.height();

        left = qMin(left, -0.5f * boundingRect.width() - pageSpacing);
        right = qMax(right, 0.5f * boundingRect.width() + pageSpacing);
        height += pageHeight + pageSpacing;
    }
}

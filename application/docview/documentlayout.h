#ifndef DOCUMENTLAYOUT_H
#define DOCUMENTLAYOUT_H

#include <QMap>
#include <QPair>

#include "global.h"
class QRectF;
class PageItem;
class ViewSettings;

struct DocumentLayout
{
    DocumentLayout();
    virtual ~DocumentLayout() {}

    static DocumentLayout* fromLayoutMode(LayoutMode layoutMode);

    virtual LayoutMode layoutMode() const = 0;

    virtual int currentPage(int page) const = 0;
    virtual int previousPage(int page) const = 0;
    virtual int nextPage(int page, int count) const = 0;

    bool isCurrentPage(const QRectF& visibleRect, const QRectF& pageRect) const;

    virtual QPair< int, int > prefetchRange(int page, int count) const = 0;

    virtual int leftIndex(int index) const = 0;
    virtual int rightIndex(int index, int count) const = 0;

    virtual qreal visibleWidth(int viewportWidth) const = 0;
    qreal visibleHeight(int viewportHeight) const;

    virtual void prepareLayout(const QVector< PageItem* >& pageItems, bool rightToLeft,
                               qreal& left, qreal& right, qreal& height) = 0;

protected:
    static ViewSettings* s_settings;

};

struct SinglePageLayout : public DocumentLayout
{
    LayoutMode layoutMode() const { return SinglePageMode; }

    int currentPage(int page) const;
    int previousPage(int page) const;
    int nextPage(int page, int count) const;

    QPair< int, int > prefetchRange(int page, int count) const;

    int leftIndex(int index) const;
    int rightIndex(int index, int count) const;

    qreal visibleWidth(int viewportWidth) const;

    void prepareLayout(const QVector< PageItem* >& pageItems, bool rightToLeft,
                       qreal& left, qreal& right, qreal& height);

};

#endif // DOCUMENTLAYOUT_H

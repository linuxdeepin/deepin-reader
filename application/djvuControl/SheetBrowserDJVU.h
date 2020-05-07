#ifndef SHEETBROWSERDJVU_H
#define SHEETBROWSERDJVU_H

#include <QGraphicsView>
#include "document/model.h"
#include "global.h"

class SheetBrowserDJVUItem;
class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowserDJVU(QWidget *parent = nullptr);

    ~SheetBrowserDJVU();

    bool openFilePath(const QString &);

    void loadPages(Dr::ScaleMode mode, double scale, Dr::Rotation rotation, Dr::MouseShape mouseShape, int currentPage);

    void setScale(Dr::ScaleMode mode, double scale, Dr::Rotation rotation);

    void setMouseShape(Dr::MouseShape mouseShape);

    void setCurrentPage(int page);

signals:
    void sigScrollPage(int page);   //滚动到某页

private slots:
    void onVerticalScrollBarValueChanged(int value);

    void onScroll();

private:
    deepin_reader::Document *m_document = nullptr;
    QList<SheetBrowserDJVUItem *> m_items;

    Dr::Rotation m_rotion = Dr::RotateBy0;
    Dr::ScaleMode m_mode = Dr::ScaleFactorMode;
    Dr::MouseShape m_mouseShape = Dr::MouseShapeNormal;
    double m_scale = 100.0;

    int m_maxWidth = 0;
    int m_maxHeight = 0;

    int scrollValue = 0;
};

#endif // SHEETBROWSERDJVU_H

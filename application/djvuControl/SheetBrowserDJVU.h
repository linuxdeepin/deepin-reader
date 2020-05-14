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

    void loadPages(Dr::ScaleMode mode, double scaleFactor, Dr::Rotation rotation, Dr::MouseShape mouseShape, int currentPage);

    void setScale(Dr::ScaleMode mode, double scaleFactor, Dr::Rotation rotation);

    void setRotation(Dr::Rotation rotation);

    void setLayoutMode(Dr::LayoutMode mode);

    void setMouseShape(Dr::MouseShape mouseShape);

    void deform();      //根据当前参数进行变换

    bool hasLoaded();

    int allPages();

    int currentPage();

    void setCurrentPage(int page);

    bool getImageMax(int index, QImage &image, double max);

protected:
    void showEvent(QShowEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

signals:
    void sigPageChanged(int page);

    void sigScaleChanged(Dr::ScaleMode mode, qreal scaleFactor);

    void sigNeedZoomin();

    void sigNeedZoomout();

private slots:
    void onVerticalScrollBarValueChanged(int value);

private:
    void wheelEvent(QWheelEvent *event);

private:
    deepin_reader::Document *m_document = nullptr;
    QList<SheetBrowserDJVUItem *> m_items;

    Dr::LayoutMode m_layoutMode = Dr::SinglePageMode;
    Dr::Rotation m_rotion       = Dr::RotateBy0;
    Dr::ScaleMode m_scaleMode   = Dr::ScaleFactorMode;
    Dr::MouseShape m_mouseShape = Dr::MouseShapeNormal;
    qreal m_scaleFactor         = 1;
    int m_curPage = 0;
    int m_maxWidth = 0;     //最大一页的宽度
    int m_maxHeight = 0;    //最大一页的高度
    bool m_hasLoaded = false;//是否已经加载过每页的信息
    int m_initPage = 1;     //用于刚显示跳转的页数
};

#endif // SHEETBROWSERDJVU_H

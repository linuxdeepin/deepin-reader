#ifndef SHEETBROWSERDJVU_H
#define SHEETBROWSERDJVU_H

#include <QGraphicsView>
#include "document/model.h"
#include "global.h"

class DocOperation;
class SheetBrowserDJVUItem;
class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowserDJVU(QWidget *parent = nullptr);

    ~SheetBrowserDJVU();

    bool openFilePath(const QString &);

    void loadPages(DocOperation &operation);

    void loadMouseShape(DocOperation &operation);

    void deform(DocOperation &operation);      //根据当前参数进行变换

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

    void sigWheelUp();

    void sigWheelDown();

    void sigSizeChanged();

private slots:
    void onVerticalScrollBarValueChanged(int value);

private:
    void wheelEvent(QWheelEvent *event);

private:
    deepin_reader::Document *m_document = nullptr;
    QList<SheetBrowserDJVUItem *> m_items;

    int m_maxWidth = 0;     //最大一页的宽度
    int m_maxHeight = 0;    //最大一页的高度
    bool m_hasLoaded = false;//是否已经加载过每页的信息
    int m_initPage = 1;     //用于刚显示跳转的页数
};

#endif // SHEETBROWSERDJVU_H

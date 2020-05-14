#ifndef DOCSHEETDJVU_H
#define DOCSHEETDJVU_H

#include "DocSheet.h"

class SheetBrowserDJVU;
class DocSheetDJVU : public DocSheet
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetDJVU)

public:
    explicit DocSheetDJVU(QString filePath, QWidget *parent = nullptr);

    ~DocSheetDJVU()override;

    bool openFileExec()override;

    int pagesNumber()override;

    int currentPage()override;      //从1开始

    int currentIndex()override;     //从0开始

    void jumpToPage(int page)override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void zoomin()override;  //放大一级

    void zoomout() override;

    void rotateLeft()override;

    void rotateRight()override;

    void setLayoutMode(Dr::LayoutMode mode)override;

    void setScaleFactor(qreal scaleFactor)override;

    void setMouseShape(Dr::MouseShape shape)override;

    void setScaleMode(Dr::ScaleMode mode)override;

    bool getImageMax(int index, QImage &image, double max)override;

    bool fileChanged()override;

private slots:
    void onBrowserPageChanged(int page);

    void onBrowserScaleChanged(Dr::ScaleMode mode, qreal scaleFactor);

private:
    SheetBrowserDJVU *m_browser = nullptr;
};

#endif // DOCSHEETDJVU_H

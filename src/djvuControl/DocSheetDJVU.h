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

    virtual ~DocSheetDJVU()override;

    bool openFileExec()override;

    void setBookMark(int index, int state);

    bool fileChanged()override;

    bool saveData()override;

    bool saveAsData(QString filePath)override;

    int pagesNumber()override;

    int currentPage()override;      //从1开始

    int currentIndex()override;     //从0开始

    void jumpToPage(int page)override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void rotateLeft()override;

    void rotateRight()override;

    void setLayoutMode(Dr::LayoutMode mode)override;

    void setScaleFactor(qreal scaleFactor)override;

    void setMouseShape(Dr::MouseShape shape)override;

    void setScaleMode(Dr::ScaleMode mode)override;

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) override;

    bool getImageMax(int index, QImage &image, double max)override;

    void openMagnifier()override;

    void closeMagnifier()override;

    bool magnifierOpened()override;

    void docBasicInfo(stFileInfo &info)override;

    QString filter()override;

private slots:
    void onBrowserPageChanged(int page);

    void onBrowserSizeChanged();

    void onBrowserWheelUp();

    void onBrowserWheelDown();

    void onBrowserPageFirst();

    void onBrowserPagePrev();

    void onBrowserPageNext();

    void onBrowserPageLast();

private:
    SheetBrowserDJVU *m_browser = nullptr;
    QSet<int> m_initBookmarks;
};

#endif // DOCSHEETDJVU_H

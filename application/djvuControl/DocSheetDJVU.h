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

    void pageJump(int page)override;

    void pageFirst()override;

    void pageLast()override;

    void pageNext()override;

    void pagePrev()override;

    void zoomin()override;  //放大一级

    void zoomout() override;

    void setScaleFactor(qreal scaleFactor)override;

    bool openFileExec()override;

    void setMouseShape(Dr::MouseShape shape)override;

    void setScaleMode(Dr::ScaleMode mode)override;

private:
    SheetSidebar     *m_sidebar = nullptr;
    SheetBrowserDJVU *m_browser = nullptr;
};

#endif // DOCSHEETDJVU_H

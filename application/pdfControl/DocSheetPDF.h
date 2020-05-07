#ifndef DOCSHEETPDF_H
#define DOCSHEETPDF_H

#include <DSplitter>
#include <QMap>

#include "FileDataModel.h"
#include "pdfControl/docview/commonstruct.h"
#include "ModuleHeader.h"
#include "DocSheet.h"

class SpinnerWidget;
class SheetBrowserPDF;
class SheetSidebarPDF;
class DocummentProxy;
class SheetBrowserArea;
class FindWidget;

class DocSheetPDF : public DocSheet
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetPDF)

public:
    explicit DocSheetPDF(Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheetPDF() override;

    void openFile(const QString &filePath)override;

    void pageJump(int page)override;

    void pageFirst()override;

    void pageLast()override;

    void pageNext()override;

    void pagePrev()override;

    void zoomin()override;//放大一级

    void zoomout()override;

    void setDoubleShow(bool isShow)override;

    void setRotateLeft()override;

    void setRotateRight()override;

    void setFileChanged(bool hasChanged)override;

    void setMouseDefault()override;//默认工具

    void setMouseHand()override;//手型工具

    void setScale(double scale)override;

    void setFit(int fit)override;

    void setBookMark(int page, int state)override;

    void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT)override;

    bool isMouseHand()override;

    bool isDoubleShow()override;

    QString filePath()override;

    QString filter()override;

    bool getFileChanged()override;

    bool saveData()override;

    bool saveAsData(QString filePath)override;

    QVariant getOper(int type)override;

    void setOper(const int &, const QVariant &)override;

    void saveOper()override;

    DocummentProxy *getDocProxy()override;//在文档打开成功之前为空

    void OnOpenSliderShow()override;

    void exitSliderShow()override;

    void ShowFindWidget()override;

    void handleOpenSuccess()override;

    void setSidebarVisible(bool isVisible)override;

    void onTitleShortCut(QString)override;

private slots:
    void SlotFileOpenResult(const QString &, const bool &);

    void onShowTips(const QString &tips, int);

    void onFileChanged();

    void onSplitterMoved(int, int);

    void onFindOperation(int, QString);

    void onFindContentComming(const stSearchRes &);

    void onFindFinished();

    void onRotate(int rotate);

    void onAnntationMsg(const int &, const QString &);

private:
    SheetSidebarPDF *m_sidebar = nullptr;
    SheetBrowserPDF *m_browser = nullptr;
    QStackedWidget *m_pRightWidget = nullptr;
    SpinnerWidget  *m_pSpinnerWidget = nullptr;
    FindWidget     *m_pFindWidget = nullptr;
    bool            m_bOldState = false;
};

#endif // DOCSHEETPDF_H

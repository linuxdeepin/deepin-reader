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
class SheetSidebar;
class DocummentProxy;
class SheetBrowserArea;
class FindWidget;

class DocSheetPDF : public DocSheet
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetPDF)

public:
    explicit DocSheetPDF(QString filePath, Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheetPDF() override;

    void openFile()override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void zoomin()override;//放大一级

    void zoomout()override;

    void setDoubleShow(bool isShow)override;

    void rotateLeft()override;

    void rotateRight()override;

    void setFileChanged(bool hasChanged)override;

    void setMouseDefault()override;//默认工具

    void setMouseHand()override;//手型工具

    void setScale(double scale)override;

    void setFit(int fit)override;

    void setBookMark(int page, int state)override;

    void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT)override;

    bool isMouseHand()override;

    bool isDoubleShow()override;

    QString filter()override;

    bool getFileChanged()override;

    bool saveData()override;

    bool saveAsData(QString filePath)override;

    QVariant getOper(int type)override;

    void setOper(const int &, const QVariant &)override;

    void saveOper()override;

    void exitSliderShow()override;

    void handleSearch()override;

    void handleOpenSuccess()override;

    void setSidebarVisible(bool isVisible)override;

    void copySelectedText()override;                    //复制选中文字

    void highlightSelectedText()override;                   //高亮选中文字

    void addSelectedTextHightlightAnnotation()override;       //对选中文字添加高亮注释

    void openSideBar()override;

    void print()override;

    virtual Outline outline() override;

    virtual int pagesNumber() override;

    virtual int currentIndex() override;
    virtual int  pointInWhichPage(QPoint pos) override;
    virtual int label2pagenum(QString label) override;

    virtual bool isOpen() override;
    virtual bool haslabel() override;
    virtual bool closeMagnifier() override;
    virtual bool showSlideModel() override;
    virtual bool exitSlideModel() override;
    virtual bool getAutoPlaySlideStatu() override;
    virtual bool setBookMarkState(int page, bool state) override;
    virtual bool getImage(int pagenum, QImage &image, double width, double height) override;
    virtual bool getImageMax(int pagenum, QImage &image, double max) override;

    virtual void jumpToHighLight(const QString &uuid, int ipage) override;
    virtual void docBasicInfo(stFileInfo &info) override;
    virtual void setAutoPlaySlide(bool autoplay, int timemsec = 3000) override;
    virtual void getAllAnnotation(QList<stHighlightContent> &listres) override;
    virtual void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0) override;

    virtual double getMaxZoomratio() override;

    virtual QString pagenum2label(int index) override;
    virtual QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note) override;

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
    SheetSidebar *m_sidebar = nullptr;
    SheetBrowserPDF *m_browser = nullptr;
    QStackedWidget *m_pRightWidget = nullptr;
    SpinnerWidget  *m_pSpinnerWidget = nullptr;
    FindWidget     *m_pFindWidget = nullptr;
    bool            m_bOldState = false;
};

#endif // DOCSHEETPDF_H

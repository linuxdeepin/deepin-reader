#ifndef DOCSHEETPDF_H
#define DOCSHEETPDF_H

#include <DSplitter>
#include <QMap>

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

    void jumpToPage(int page)override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void rotateLeft()override;

    void rotateRight()override;

    void setFileChanged(bool hasChanged)override;

    void setBookMark(int page, int state)override;

    void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT)override;

    void setLayoutMode(Dr::LayoutMode mode)override;

    void setMouseShape(Dr::MouseShape shape) override;

    void setScaleMode(Dr::ScaleMode mode) override;

    void setScaleFactor(qreal scaleFactor) override;

    bool isDoubleShow()override;

    QString filter()override;

    bool fileChanged()override;

    bool saveData()override;

    bool saveAsData(QString filePath)override;

    void exitSliderShow()override;

    void handleSearch()override;

    void copySelectedText()override;                    //复制选中文字

    void highlightSelectedText()override;                   //高亮选中文字

    void addSelectedTextHightlightAnnotation()override;       //对选中文字添加高亮注释

    Outline outline() override;

    int pagesNumber() override;
    int currentIndex() override;
    int pointInWhichPage(QPoint pos) override;
    int label2pagenum(QString label) override;

    bool isOpen() override;
    bool haslabel() override;
    bool closeMagnifier() override;
    bool showSlideModel() override;
    bool exitSlideModel() override;
    bool getAutoPlaySlideStatu() override;
    bool getImage(int index, QImage &image, double width, double height) override;
    bool getImageMax(int index, QImage &image, double max) override;

    void jumpToHighLight(const QString &uuid, int ipage) override;
    void docBasicInfo(stFileInfo &info) override;
    void setAutoPlaySlide(bool autoplay, int timemsec = 3000) override;
    void getAllAnnotation(QList<stHighlightContent> &listres) override;
    void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0) override;

    double getMaxZoomratio() override;

    QString pagenum2label(int index) override;
    QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note) override;

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
    SheetBrowserPDF *m_browser = nullptr;
    QStackedWidget *m_pRightWidget = nullptr;
    SpinnerWidget  *m_pSpinnerWidget = nullptr;
    FindWidget     *m_pFindWidget = nullptr;
    bool            m_bOldState = false;
};

#endif // DOCSHEETPDF_H

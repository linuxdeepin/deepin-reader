#ifndef DOCSHEET_H
#define DOCSHEET_H

#include <DSplitter>
#include <QMap>

#include "FileDataModel.h"
#include "global.h"
#include "pdfControl/docview/commonstruct.h"
#include "ModuleHeader.h"

class SpinnerWidget;
class SheetBrowserPDF;
class SheetSidebar;
class DocummentProxy;
class SheetBrowserArea;
class FindWidget;
struct DocOperation {
    Dr::LayoutMode layoutMode   = Dr::SinglePageMode;
    Dr::MouseShape mouseShape   = Dr::MouseShapeNormal;
    Dr::ScaleMode scaleMode     = Dr::ScaleFactorMode;
    Dr::Rotation rotation       = Dr::RotateBy0;
    qreal scaleFactor           = 1.0;
    bool sidebarVisible         = false;
    int  sidebarIndex           = 0;
    int  currentPage            = 1;
};

class DocSheet : public Dtk::Widget::DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheet)
    friend class Database;
    friend class SheetSideBar;
public:
    explicit DocSheet(Dr::FileType type, QString filePath, Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheet() override;

    virtual bool isOpen();

    virtual void openFile();

    virtual bool openFileExec();

    virtual void jumpToIndex(int index);

    virtual void jumpToPage(int page);

    virtual void jumpToFirstPage();

    virtual void jumpToLastPage();

    virtual void jumpToNextPage();

    virtual void jumpToPrevPage();

    virtual void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0);

    virtual void jumpToHighLight(const QString &uuid, int ipage);

    virtual void zoomin();  //放大一级

    virtual void zoomout();

    virtual void setDoubleShow(bool isShow);

    virtual void setRotateLeft();

    virtual void setRotateRight();

    virtual void setFileChanged(bool hasChanged);

    virtual void setMouseDefault();     //默认工具

    virtual void setMouseHand();        //手型工具

    virtual void setMouseShape(Dr::MouseShape shape);

    virtual void setScale(double scale);

    virtual void setScaleFactor(qreal scaleFactor);     //base is 1

    virtual void setFit(int fit);

    virtual void setScaleMode(Dr::ScaleMode mode);

    virtual void setBookMark(int page, int state);

    virtual void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT);

    virtual bool isMouseHand();

    virtual bool isDoubleShow();

    virtual QString filter();

    virtual bool getFileChanged();

    virtual bool saveData();

    virtual bool saveAsData(QString filePath);

    virtual QVariant getOper(int type);

    virtual void setOper(const int &, const QVariant &);

    virtual void saveOper();

    virtual void exitSliderShow();

    virtual void handleSearch();

    virtual void handleOpenSuccess();

    virtual void setSidebarVisible(bool isVisible);

    virtual void copySelectedText();                    //复制选中文字

    virtual void highlightSelectedText();                   //高亮选中文字

    virtual void addSelectedTextHightlightAnnotation();       //对选中文字添加高亮注释

    virtual void openSideBar();

    virtual void print();

    virtual int pagesNumber();

    virtual int currentPage();      //从1开始

    virtual int currentIndex();     //从0开始

    //replace docProxy api 2020.05.11
    virtual Outline outline();

    virtual int pointInWhichPage(QPoint pos);
    virtual int label2pagenum(QString label);

    virtual bool haslabel();
    virtual bool closeMagnifier();
    virtual bool showSlideModel();
    virtual bool exitSlideModel();
    virtual bool getAutoPlaySlideStatu();
    virtual bool setBookMarkState(int page, bool state);
    virtual bool getImage(int pagenum, QImage &image, double width, double height);
    virtual bool getImageMax(int pagenum, QImage &image, double max);


    virtual void docBasicInfo(stFileInfo &info);
    virtual void setAutoPlaySlide(bool autoplay, int timemsec = 3000);
    virtual void getAllAnnotation(QList<stHighlightContent> &listres);

    virtual double getMaxZoomratio();

    virtual QString pagenum2label(int index);
    virtual QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note);

    QString filePath();

    QList<qreal> scaleFactorList();

    Dr::FileType type();

    void showTips(const QString &tips, int iconIndex = 0);

    void setCurrentState(int state);

    int getCurrentState();

    void showControl();

    void quitSlide();

    void openMagnifer();

    void quitMagnifer();

    DocOperation getOperation();

protected:
    DocOperation &operation();

signals:
    void sigOpenFileResult(const QString &, const bool &);

    void sigFileChanged(DocSheet *);    //被修改了 书签 笔记

    void sigOpened(DocSheet *, bool);

    void sigFindOperation(const int &);

    void signalAutoplaytoend();

private:
    QString      m_filePath;
    Dr::FileType m_type;
    DocOperation m_operation;
    QString      m_uuid;
public:
    static QUuid     getUuid(DocSheet *);
    static DocSheet *getSheet(QString uuid);
    static QMap<QString, DocSheet *> g_map;
};

#endif // DOCSHEET_H

#ifndef DOCSHEET_H
#define DOCSHEET_H

#include <DSplitter>
#include <QMap>

#include "global.h"
#include "pdfControl/docview/commonstruct.h"
#include "ModuleHeader.h"

class SheetSidebar;
class SlideWidget;
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
    friend class SheetSidebar;
public:
    explicit DocSheet(Dr::FileType type, QString filePath, Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheet();

    virtual void initOperationData(const DocOperation &opera);

    virtual bool isOpen();

    virtual void openFile();

    virtual bool openFileExec();

    virtual int pagesNumber();

    virtual int currentPage();      //从1开始

    virtual int currentIndex();     //从0开始

    virtual void jumpToIndex(int index);

    virtual void jumpToPage(int page);

    virtual void jumpToFirstPage();

    virtual void jumpToLastPage();

    virtual void jumpToNextPage();

    virtual void jumpToPrevPage();

    virtual Outline outline();

    virtual void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0);

    virtual void jumpToHighLight(const QString &uuid, int ipage);

    virtual void rotateLeft();

    virtual void rotateRight();

    virtual void setBookMark(int index, int state);

    bool hasBookMark(int index);

    virtual void setLayoutMode(Dr::LayoutMode mode);

    virtual void setScaleMode(Dr::ScaleMode mode);

    virtual void setScaleFactor(qreal scaleFactor);     //base is 1 ;设置后自动取消自适应

    virtual void setMouseShape(Dr::MouseShape shape);

    virtual void openMagnifier();

    virtual bool closeMagnifier();

    virtual bool fileChanged();

    virtual bool saveData();

    virtual bool saveAsData(QString filePath);

    virtual void handleSearch();

    virtual void copySelectedText();                    //复制选中文字

    virtual void highlightSelectedText();                   //高亮选中文字

    virtual void addSelectedTextHightlightAnnotation();       //对选中文字添加高亮注释

    virtual bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

    virtual bool getImageMax(int index, QImage &image, double max);

    virtual QString filter();

    virtual QSet<int> getBookMarkList() const;

    void print();

    DocOperation operation();

    Dr::FileType type();

    QString filePath();

    QList<qreal> scaleFactorList();

    void zoomin();  //放大一级

    void zoomout();

    void setSidebarVisible(bool isVisible);

    void handleOpenSuccess();

    void showTips(const QString &tips, int iconIndex = 0);

    void setCurrentState(int state);

    int  currentState();

    void openSlideWidget();

    void closeSlideWidget();

    void handleSlideKeyPressEvent(const QString &sKey);

protected:
    DocOperation  m_operation;
    QSet<int>     m_bookmarks;

    SlideWidget  *m_slideWidget = nullptr;
    SheetSidebar *m_sidebar = nullptr;

signals:
    void sigFileChanged(DocSheet *);    //被修改了 书签 笔记

    void sigOpened(DocSheet *, bool);

private:
    QString         m_filePath;

    Dr::FileType    m_type;

    QString         m_uuid;

public:
    static bool existFileChanged();

    static QUuid getUuid(DocSheet *);

    static DocSheet *getSheet(QString uuid);

    static QMap<QString, DocSheet *> g_map;

    //===========以上是改版后的,优先使用(pdf看情况，如果未实现则不用) ,以下则逐步替换和删除

public:
    virtual int label2pagenum(QString label);

    virtual bool haslabel();

    virtual void docBasicInfo(stFileInfo &info);

    virtual void getAllAnnotation(QList<stHighlightContent> &listres);

    virtual double getMaxZoomratio();

    virtual QString pagenum2label(int index);

    virtual QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note);

signals:
    void sigFindOperation(const int &);

};

#endif // DOCSHEET_H

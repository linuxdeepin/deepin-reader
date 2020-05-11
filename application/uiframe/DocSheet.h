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
    bool openThumbnail          = false;
    int  leftIndex              = 0;
    int  currentPage            = 1;
};

class DocSheet : public Dtk::Widget::DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheet)

public:
    explicit DocSheet(Dr::FileType type, QString filePath, Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheet() override;

    virtual void openFile();

    virtual bool openFileExec();

    virtual void pageJump(int page);

    virtual void pageFirst();

    virtual void pageLast();

    virtual void pageNext();

    virtual void pagePrev();

    virtual void zoomin();  //放大一级

    virtual void zoomout();

    virtual void setDoubleShow(bool isShow);

    virtual void setRotateLeft();

    virtual void setRotateRight();

    virtual void setFileChanged(bool hasChanged);

    virtual void setMouseDefault();     //默认工具

    virtual void setMouseHand();        //手型工具

    virtual void setScale(double scale);

    virtual void setScaleFactor(qreal scaleFactor);

    virtual void setFit(int fit);

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

    virtual DocummentProxy *getDocProxy();      //在文档打开成功之前为空

    virtual void OnOpenSliderShow();

    virtual void exitSliderShow();

    virtual void handleSearch();

    virtual void handleOpenSuccess();

    virtual void setSidebarVisible(bool isVisible);

    virtual void copySelectedText();                    //复制选中文字

    virtual void highlightSelectedText();                   //高亮选中文字

    virtual void addSelectedTextHightlightAnnotation();       //对选中文字添加高亮注释

    virtual void openSideBar();

    virtual void print();

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

    DocOperation &operation();

signals:
    void sigOpenFileResult(const QString &, const bool &);

    void sigFileChanged(DocSheet *);    //被修改了 书签 笔记

    void sigScaleChanged(DocSheet *);

    void sigTotateChanged();

    void sigOpened(DocSheet *, bool);

    void sigFindOperation(const int &);

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

#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include "docview/commonstruct.h"
#include "searchtask.h"
#include <DWidget>
#include <DScrollArea>
#include <DScrollBar>
#include <DGuiApplicationHelper>
#include <QList>
#include <QVBoxLayout>
#include <QPoint>
#include <QColor>
#include <QVideoWidget>
#include <QThread>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};
#include <QtDebug>

class SearchTask;
class DocummentBase;
class DocummentBasePrivate;
//class ThreadLoadWords : public QThread
//{
//public:
//    ThreadLoadWords();
//    void setDoc(DocummentBase *doc);
//    void setRestart();

//protected:
//    virtual void run();

//private:
//    DocummentBase *m_doc;
//    bool restart;
//};

class MagnifierWidget: public DWidget
{
    Q_OBJECT
public:
    MagnifierWidget(DWidget *parent = nullptr);
    void setPixmap(QPixmap pixmap);
    void setPoint(QPoint point);
    int getMagnifierRadius();
    double getMagnifierScale();
    int getMagnifierRingWidth();
    void setMagnifierRadius(int radius);
    void setMagnifierScale(double scale);
    void setMagnifierRingWidth(int ringWidth);
    void setMagnifierColor(QColor color);
    void stopShow();
    void startShow();
    bool showState();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QColor m_magnifiercolor;
    int m_magnifierradius;
    int m_magnifierringwidth;
    QPoint m_magnifierpoint;
    QPixmap m_magnifierpixmap;
    double m_magnifierscale;
    bool bStartShow;
};

class SlidWidget: public DWidget
{
    Q_OBJECT
public:
    SlidWidget(DWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};


class DocummentBasePrivate: public QObject
{
    Q_OBJECT
public:
    DocummentBasePrivate(DocummentBase *parent): q_ptr(parent),
        m_pages(),
        m_widgets(),
        m_pagecountsearch()
    {
        m_widget = nullptr;
        m_vboxLayout = nullptr;
        m_magnifierwidget = nullptr;
        m_slidewidget = nullptr;
        pslidelabel = nullptr;
        pslideanimationlabel = nullptr;
        pblankwidget = nullptr;
        m_bModified = false;
        m_bslidemodel = false;
        m_slidepageno = -1;
        m_currentpageno = 0;
        m_scale = 1.0;
        m_rotate = RotateType_0;
        donotneedreloaddoc = false;
        m_magnifierpage = -1;
        m_viewmode = ViewMode_SinglePage;
        m_lastmagnifierpagenum = -1;
        animationfirst = nullptr;
        animationsecond = nullptr;
        animationgroup = nullptr;
        m_bsearchfirst = true;
        m_findcurpage = -1;
        m_imagewidth = 0.1;
        m_imageheight = 0.1;
        m_cursearch = 1;
        bcloseing = false;
        m_searchTask = nullptr;
        bfindnext=true;
        m_bScanningcopy=false;
        showslidwaittimer = nullptr;

    }

    ~DocummentBasePrivate()
    {
        if (!animationfirst) {
            animationfirst->deleteLater();
            animationfirst = nullptr;
        }
        if (!animationsecond) {
            animationsecond->deleteLater();
            animationsecond = nullptr;
        }
        if (!animationgroup) {
            animationgroup->deleteLater();
            animationgroup = nullptr;
        }
        if (m_searchTask) {
            delete m_searchTask;
            m_searchTask = nullptr;
        }
        if (m_magnifierwidget) {
            m_magnifierwidget->deleteLater();
            m_magnifierwidget = nullptr;

        }
        if (m_slidewidget) {
            m_slidewidget->deleteLater();
            m_slidewidget = nullptr;
        }
    }

    QVector<PageBase *> m_pages;
    QList<DWidget *>m_widgets;
    DWidget *m_widget;
    QVBoxLayout *m_vboxLayout;
    MagnifierWidget *m_magnifierwidget;
    SlidWidget *m_slidewidget;
    DLabel *pslideanimationlabel;
    DLabel *pslidelabel;
    DWidget *pblankwidget;
    ViewMode_EM m_viewmode;
    int m_lastmagnifierpagenum;
    int m_magnifierpage;
    int m_slidepageno;
    int m_currentpageno;
    double m_scale;
    mutable bool m_bModified;
    bool m_bslidemodel;
    //    ThreadLoadWords m_threadloadwords;
    RotateType_EM m_rotate;
    bool donotneedreloaddoc;
    //    bool m_wordsbload;
    QPoint m_magnifierpoint;
    QPropertyAnimation *animationfirst;
    QPropertyAnimation *animationsecond;
    QParallelAnimationGroup *animationgroup;
    SearchTask *m_searchTask;
    int m_findcurpage;
    QMap<int, int> m_pagecountsearch; //搜索结果页对应当前页个数
    int m_cursearch;
    bool m_bsearchfirst;
    double m_imagewidth;
    double m_imageheight;
    bool bcloseing;
    bool bfindnext;//上一次搜索结果是向前翻还是向后翻
    bool m_bScanningcopy;//当前打开的是否为扫描件
    QTimer *showslidwaittimer;


signals:
    void signal_docummentLoaded();
protected slots:
    virtual void loadDocumment(QString filepath) {}
protected:
    DocummentBase *q_ptr;
    Q_DECLARE_PUBLIC(DocummentBase)
};

class DocummentBase: public DScrollArea
{
    Q_OBJECT
public:
    DocummentBase(DocummentBasePrivate *ptr = nullptr, DWidget *parent = nullptr);
    ~DocummentBase();
    virtual bool loadDocumment(QString filepath) = 0;
    virtual bool bDocummentExist() = 0;
    virtual bool getImage(int pagenum, QImage &image, double width, double height) = 0;
    virtual bool save(const QString &filePath, bool withChanges)
    {
        //        qDebug() << "do nothing";
        return false;
    }
    virtual bool saveas(const QString &filePath, bool withChanges)
    {
        return false;
    }
    virtual QString removeAnnotation(const QPoint &startpos) {}

    virtual void removeAnnotation(const QString &struuid,int ipage=-1) {}
    virtual QString addAnnotation(const QPoint &startpos,const QPoint &endpos,QColor color = Qt::yellow) {}
    virtual void search(const QString &strtext, QColor color = Qt::yellow) {}
    virtual void getAllAnnotation(QList<stHighlightContent> &listres) {}
    virtual void clearSearch() {}
    virtual void docBasicInfo(stFileInfo &info) {}
    virtual void title(QString &title) {}
    virtual void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) {}
    virtual void getAnnotationText(const QString &struuid, QString &strtext, int ipage = -1) {}
    virtual bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
    {
        return false;
    }
    virtual void jumpToHighLight(const QString &uuid, int ipage) {};
    void stopLoadPageThread();
    bool openFile(QString filepath);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
    QPoint global2RelativePoint(QPoint globalpoint);
    bool showMagnifier(QPoint point);
    int getCurrentPageNo();
    int currentPageNo();
    Page::Link *mouseBeOverLink(QPoint point);
    bool getSelectTextString(QString &st);
    bool loadPages();
    double adaptWidthAndShow(double width);
    double adaptHeightAndShow(double height);
    void findNext();
    void findPrev();
    //    bool loadWords();
    int getPageSNum();
    bool exitSlideModel();
    QVector<PageBase *> *getPages();
    PageBase *getPage(int index);
    void magnifierClear();
    void pageMove(double mvx, double mvy);
    //    bool isWordsBeLoad();
    bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3);
    bool showSlideModel();
    void cacularValueXY(int &xvalue, int &yvalue, int curpage, bool bsearch = true, QRectF rect = QRectF());
    int pointInWhichPage(QPoint &qpoint);


signals:
    void signal_pageChange(int);
    void signal_searchRes(stSearchRes);
    void signal_searchover();
    void signal_loadDocumment(QString);

protected slots:
    void slot_vScrollBarValueChanged(int value);
    void slot_hScrollBarValueChanged(int value);
    void slot_MagnifierPixmapCacheLoaded(int pageno);
    void slot_searchValueAdd(stSearchRes res);
    void slot_searchover();
    void slot_docummentLoaded();
    bool pageJump(int pagenum);
    bool mouseSelectText(QPoint start, QPoint stop);
    void scaleAndShow(double scale, RotateType_EM rotate);
    bool setViewModeAndShow(ViewMode_EM viewmode);
    void showSlideModelTimerOut();
protected:
    void showSinglePage();
    void showFacingPage();
    void initConnect();





    DocummentBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentBase)

};

#endif // DOCUMMENTBASE_H

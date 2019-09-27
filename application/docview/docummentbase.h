#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include "docview/commonstruct.h"
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

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};
#include <QtDebug>

class DocummentBase;
//class ThreadLoadDoc : public QThread
//{
//    Q_OBJECT
//public:
//    ThreadLoadDoc();
//    void setDoc(DocummentBase *doc);
//    void setRestart();

//protected:
//    virtual void run();

//private:
//    DocummentBase *m_doc;
//    bool restart;
//};
class DocummentBase;
class DocummentBasePrivate;
class ThreadLoadWords : public QThread
{
public:
    ThreadLoadWords();
    void setDoc(DocummentBase *doc);
    void setRestart();

protected:
    virtual void run();

private:
    DocummentBase *m_doc;
    bool restart;
};

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


class DocummentBasePrivate: public QObject
{
    Q_OBJECT
public:
    DocummentBasePrivate(DocummentBase *parent): q_ptr(parent)
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
        m_currentpageno = -1;
        m_scale = 1.0;
        m_rotate = RotateType_0;
        donotneedreloaddoc = false;
        m_wordsbload = false;
        m_magnifierpage = false;
        m_currentpageno = 0;
        m_viewmode = ViewMode_SinglePage;
        m_lastmagnifierpagenum = -1;
    }

    ~DocummentBasePrivate()
    {
        if (m_threadloadwords.isRunning()) {
            m_threadloadwords.requestInterruption();
            m_threadloadwords.quit();
            m_threadloadwords.wait();
        }
//        if (m_magnifierwidget) {
//            delete m_magnifierwidget;
//            m_magnifierwidget = nullptr;
//        }
//        if (m_slidewidget) {
//            delete m_slidewidget;
//            m_slidewidget = nullptr;
//        }
    }

    QList<PageBase *> m_pages;
    QList<DWidget *>m_widgets;
    DWidget *m_widget;
    QVBoxLayout *m_vboxLayout;
    MagnifierWidget *m_magnifierwidget;
    DWidget *m_slidewidget;
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
//    ThreadLoadDoc m_threadloaddoc;
    ThreadLoadWords m_threadloadwords;
    RotateType_EM m_rotate;
    bool donotneedreloaddoc;
    bool m_wordsbload;
    QPoint m_magnifierpoint;
//    virtual bool loadDocumment(QString filepath) = 0;

signals:
    void signal_docummentLoaded();
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
    virtual bool loadDocumment(QString filepath)
    {
        return false;
    }
    virtual bool bDocummentExist()
    {
        return false;
    }
    virtual bool getImage(int pagenum, QImage &image, double width, double height)
    {
        return false;
    }
    virtual bool save(const QString &filePath, bool withChanges)
    {
        qDebug() << "do nothing";
        return false;
    }
    virtual bool saveas(const QString &filePath, bool withChanges)
    {
        return false;
    }
    virtual QString removeAnnotation(const QPoint &startpos) {}
    virtual void removeAnnotation(const QString &struuid) {}
    virtual QString addAnnotation(const QPoint &starpos, const QPoint &endpos, QColor color = Qt::yellow) {}
    virtual void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) {}
    virtual void clearSearch() {}
    virtual void docBasicInfo(stFileInfo &info) {}
    virtual void findNext() {}
    virtual void findPrev() {}
    virtual void title(QString &title) {}
    virtual void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) {}
    virtual void getAnnotationText(const QString &struuid, QString &strtext, int ipage = -1) {}

    void stopLoadPageThread();
    bool openFile(QString filepath);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    bool mouseSelectText(QPoint start, QPoint stop);
    void mouseSelectTextClear();
    void scaleAndShow(double scale, RotateType_EM rotate);
    bool mouseBeOverText(QPoint point);
    QPoint global2RelativePoint(QPoint globalpoint);
    bool showMagnifier(QPoint point);
    bool setViewModeAndShow(ViewMode_EM viewmode);
    int currentPageNo();
    bool pageJump(int pagenum);
    Page::Link *mouseBeOverLink(QPoint point);
    bool getSelectTextString(QString &st);
    bool showSlideModel();
    bool loadPages();
    double adaptWidthAndShow(double width);
    double adaptHeightAndShow(double height);
    bool loadWords();
    int getPageSNum();
    bool exitSlideModel();
    QList<PageBase *> *getPages();
    PageBase *getPage(int index);
    void magnifierClear();
    void pageMove(double mvx, double mvy);
    bool isWordsBeLoad();
    bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3);

signals:
    void signal_pageChange(int);
//    void signal_loadDocumment(QString);
protected slots:
    void slot_vScrollBarValueChanged(int value);
    void slot_hScrollBarValueChanged(int value);
    void slot_MagnifierPixmapCacheLoaded(int pageno);
    void slot_docummentLoaded();
protected:
    int pointInWhichPage(QPoint &qpoint);
    void showSinglePage();
    void showFacingPage();
    void initConnect();
    QScopedPointer<DocummentBasePrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentBase)
};

#endif // DOCUMMENTBASE_H

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
class ThreadLoadDoc : public QThread
{
public:
    ThreadLoadDoc();
    void setDoc(DocummentBase *doc);
    void setRestart();

protected:
    virtual void run();

private:
    DocummentBase *m_doc;
    bool restart;
};

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
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QColor m_magnifiercolor;
    int m_magnifierradius;
    int m_magnifierringwidth;
    QPoint m_magnifierpoint;
    QPixmap m_magnifierpixmap;
    double m_magnifierscale;
};

class DocummentBase: public DScrollArea
{
    Q_OBJECT
public:
    DocummentBase(DWidget *parent = nullptr);
    virtual bool openFile(QString filepath)
    {
        return false;
    }
    virtual bool bDocummentExist()
    {
        return false;
    }
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    bool mouseSelectText(QPoint start, QPoint stop);
    void mouseSelectTextClear();
    void scaleAndShow(double scale, RotateType_EM rotate);
    bool mouseBeOverText(QPoint point);

    virtual bool getImage(int pagenum, QImage &image, double width, double height)
    {
        return false;
    }

    QPoint global2RelativePoint(QPoint globalpoint);

    bool showMagnifier(QPoint point);

    virtual bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3)
    {
        if (!m_magnifierwidget) {
            return false;
        }
        m_magnifierwidget->setMagnifierRadius(magnifierradius);
        m_magnifierwidget->setMagnifierScale(magnifierscale);
        m_magnifierwidget->setMagnifierRingWidth(magnifierringwidth);
        m_magnifierwidget->setMagnifierColor(magnifiercolor);
        return true;
    }

    int getPageSNum()
    {
        return m_pages.size();
    }

    bool setViewModeAndShow(ViewMode_EM viewmode);

    virtual bool save(const QString &filePath, bool withChanges)
    {
        qDebug() << "do nothing";
        return false;
    }

    virtual QString removeAnnotation(const QPoint &startpos) {}

    virtual void removeAnnotation(const QString &struuid) {}

    virtual QString addAnnotation(const QPoint &starpos, const QPoint &endpos, QColor color = Qt::yellow) {}


    virtual void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) {}
    int currentPageNo();

    bool pageJump(int pagenum);

    virtual void clearSearch() {}


    Page::Link *mouseBeOverLink(QPoint point);

    bool getSelectTextString(QString &st);

    bool showSlideModel();

    virtual bool loadPages()
    {
        return false;
    }

    virtual bool loadWords()
    {
        return false;
    }

    bool exitSlideModel()
    {
        m_slidewidget->hide();
        this->show();
        m_bslidemodel = false;
        m_slidepageno = -1;
        return true;
    }

    virtual void docBasicInfo(stFileInfo &info) {}

    virtual void findNext() {}

    virtual void findPrev() {}

    QList<PageBase *> *getPages()
    {
        return &m_pages;
    }
    PageBase *getPage(int index)
    {
        if (m_pages.size() > index)
            return (PageBase *)m_pages.at(index);
        return nullptr;
    }

    void magnifierClear()
    {
        if (m_magnifierwidget) {
            m_magnifierwidget->setPixmap(QPixmap());
            m_magnifierwidget->hide();
        }
    }

    void pageMove(double mvx, double mvy)
    {
        DScrollBar *scrollBar_X = horizontalScrollBar();
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + mvx);
        DScrollBar *scrollBar_Y = verticalScrollBar();
        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + mvy);
    }
    virtual void title(QString &title) {}

    virtual void setAnnotationText(int ipage,const QString& struuid,const QString& strtext){}
    virtual void getAnnotationText(const QString& struuid,QString& strtext){}


signals:
    void signal_pageChange(int);
protected slots:
    void slot_vScrollBarValueChanged(int value);
    void slot_hScrollBarValueChanged(int value);
protected:
    int pointInWhichPage(QPoint &qpoint);
    void showSinglePage();
    void showFacingPage();
    QList<PageBase *> m_pages;
    QList<DWidget *>m_widgets;
    DWidget m_widget;
    QVBoxLayout m_vboxLayout;
    ViewMode_EM m_viewmode;
    mutable bool m_bModified;
    MagnifierWidget *m_magnifierwidget;
    int m_lastmagnifierpagenum;
    DWidget *m_slidewidget;
    bool m_bslidemodel;
    DLabel *pslidelabel;
    int m_slidepageno;
    DLabel *pslideanimationlabel;
    ThreadLoadDoc m_threadloaddoc;
    ThreadLoadWords m_threadloadwords;
    RotateType_EM m_rotate;
    int m_currentpageno;
    double m_scale;
    bool donotneedreloaddoc;
    DWidget *pblankwidget;
};

#endif // DOCUMMENTBASE_H

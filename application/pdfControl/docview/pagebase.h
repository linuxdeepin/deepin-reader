#ifndef PAGEBASE_H
#define PAGEBASE_H

#include <QObject>
#include <DLabel>
#include <DSpinner>
#include <DGuiApplicationHelper>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QRunnable>
#include "commonstruct.h"
#include "widgets/bookmarkbutton.h"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
#define ICONANNOTE_WIDTH    20

class PageBase;
class PageInterface;
class PageBasePrivate;

class ThreadRenderImage : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThreadRenderImage();
    void setPage(PageInterface *page, double width, double height);
    void setRestart();
    bool isRunning();
    void setRunningTrue();
    void setQuit();

protected:
    virtual void run();

signals:
    void signal_RenderFinish(QImage);
private:
    PageInterface *m_page;
    bool restart;
    double m_width;
    double m_height;
    bool b_running;
    bool m_bquit;
};
class ThreadLoadMagnifierCache : public QThread
{
    Q_OBJECT
public:
    ThreadLoadMagnifierCache();
    void setPage(PageInterface *page, double width, double height);
    void setRestart();

protected:
    virtual void run();
signals:
    void signal_loadMagnifierPixmapCache(QImage image, double width, double height);

private:
    PageInterface *m_page;
    bool restart;
    double m_width;
    double m_height;
};
class PageInterface
{
public:
    virtual bool getImage(QImage &image, double width, double height) = 0;
    virtual bool getSlideImage(QImage &image, double &width, double &height) = 0;
    virtual bool loadData() = 0;
};
class PageBasePrivate: public QObject
{
    Q_OBJECT
public:
    PageBasePrivate(PageBase *parent): q_ptr(parent),
        paintrects(),
        m_links(),
        m_words(),
        m_highlights()
    {
        m_imagewidth = 0.01;
        m_imageheight = 0.01;
        m_paintercolor = QColor(72, 118, 255, 100);
        m_cursearchfocuscolor = QColor("#3B9401");
        m_cursearchfocuscolor.setAlpha(100);
        m_pencolor = QColor(72, 118, 255, 0);
        m_searchcolor = QColor("#EEDE00");
        m_searchcolor.setAlpha(100);
        m_penwidth = 0;
        m_selecttextstartword = -1;
        m_selecttextendword = -1;
        m_rotate = RotateType_0;
        m_magnifierwidth = 0;
        m_magnifierheight = 0;
        m_pageno = -1;
        m_icurhightlight = 0;
        m_bcursearchshow = false;
        m_scale = 1.0;
        havereander = false;
        pixelratiof = 1;
        bookmarkbtn = nullptr;
        m_spinner = nullptr;
        m_bquit = false;
        connect(&loadmagnifiercachethread, SIGNAL(signal_loadMagnifierPixmapCache(QImage, double, double)), this, SIGNAL(signal_loadMagnifierPixmapCache(QImage, double, double)));
        connect(&threadreander, SIGNAL(signal_RenderFinish(QImage)), this, SIGNAL(signal_RenderFinish(QImage)));
    }

    ~PageBasePrivate()
    {
//        qDebug() << "~PageBasePrivate";
        qDeleteAll(m_links);
        m_links.clear();
        if (loadmagnifiercachethread.isRunning()) {
            loadmagnifiercachethread.quit();
            loadmagnifiercachethread.wait();
        }
//        if (threadreander.isRunning()) {
//            threadreander.requestInterruption();
//            threadreander.quit();
//            threadreander.wait();
//        }
        threadreander.setAutoDelete(true);
    }

    QColor m_paintercolor;
    QColor m_pencolor;
    int m_penwidth;
    QList<QRect> paintrects;
    QList< Page::Link * > m_links;
    QList<stWord> m_words;
    int m_selecttextstartword;
    int m_selecttextendword;
    double m_imagewidth;
    double m_imageheight;
    QPixmap m_magnifierpixmap;
    RotateType_EM m_rotate;
    double m_scale;
    ThreadLoadMagnifierCache loadmagnifiercachethread;
    ThreadRenderImage threadreander;
    double m_magnifierwidth;
    double m_magnifierheight;
    int m_pageno;
    bool m_bcursearchshow;//当前页是否是搜索结果当前显示页
    mutable QList<QRectF> m_highlights;
    int m_icurhightlight;
    QColor m_searchcolor;
    QColor m_cursearchfocuscolor;
    bool havereander;
    qreal pixelratiof;
    BookMarkButton *bookmarkbtn;
    DSpinner *m_spinner;
    // QList<ICONANNOTATION> m_iconannotationlist;//注释图标列表
//    QMutex m_mutexlockgetimage;
    QPixmap m_pixmapshow;//当前页文档图片
    bool m_bquit;
    PageBase *q_ptr;
    Q_DECLARE_PUBLIC(PageBase)
signals:
    void signal_loadMagnifierPixmapCache(QImage image, double width, double height);
    void signal_RenderFinish(QImage);
};

class PageBase: public DLabel
{
    Q_OBJECT
public:
    PageBase(PageBasePrivate *ptr = nullptr, DWidget *parent = nullptr);
    ~PageBase();
    virtual bool getImage(QImage &image, double width, double height) = 0;
    virtual bool getSlideImage(QImage &image, double &width, double &height) = 0;
    virtual PageInterface *getInterFace() = 0;
    virtual stSearchRes search(const QString &text, bool matchCase, bool wholeWords)
    {
        Q_UNUSED(text);
        Q_UNUSED(matchCase);
        Q_UNUSED(wholeWords);
        stSearchRes res;
        return res;
    }
    QRectF setCurHightLight(int index)
    {
        Q_D(PageBase);
        QRectF rect;
        if (d->m_highlights.size() > 0 && d->m_icurhightlight < d->m_highlights.size()) {
            d->m_icurhightlight = index - 1;
            rect = d->m_highlights.at(d->m_icurhightlight);
        }
        return  rect;
    }
    double getOriginalImageWidth()
    {
        Q_D(PageBase);
        return d->m_imagewidth;
    }
    double getOriginalImageHeight()
    {
        Q_D(PageBase);
        return d->m_imageheight;
    }
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100),
                            QColor pencolor = QColor(72, 118, 255, 0),
                            int penwidth = 0);
    void clearPageTextSelections();
    bool pageTextSelections(const QPoint start, const QPoint end);
    bool ifMouseMoveOverText(const QPoint point);
    bool getMagnifierPixmap(QPixmap &pixmap, QPointF point, int radius, double width, double height);
    bool clearMagnifierPixmap();
    void loadMagnifierCacheThreadStart(double width, double height);
    void setScaleAndRotate(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    Page::Link *ifMouseMoveOverLink(const QPoint point);
    bool getSelectTextString(QString &st);
    void clearSelectText();
    void selectAllText();
    QRectF translateRect(QRectF &rect, double scale, RotateType_EM rotate);
    QPointF translatepoint(QPointF pt, double scale, RotateType_EM rotate);
    void clearHighlightRects();
    void setCurSearchShow(bool bshow)
    {
        Q_D(PageBase);
        d->m_bcursearchshow = bshow;
    }
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    void stopThread();
    void quitThread();
    void waitThread();
    void clearImage();
    bool setBookMarkState(bool state);
    virtual bool getrectimage(QImage &, double, double, double, QPoint &) {return  false;}

signals:
    void signal_MagnifierPixmapCacheLoaded(int);
    void sigBookMarkButtonClicked(int page, bool state);
    void signal_update();
protected slots:
    void slot_loadMagnifierPixmapCache(QImage image, double width, double height);
    void slot_RenderFinish(QImage);
protected:
    void paintEvent(QPaintEvent *event) override;
protected:
    void getImagePoint(QPointF &point);
    void setSelectTextRects();

    PageBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageBase)

};

#endif // PAGEBASE_H

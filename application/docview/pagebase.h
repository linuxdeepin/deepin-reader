#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <DObject>
#include <DLabel>
#include <DSpinner>
#include <DGuiApplicationHelper>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include "commonstruct.h"
#include "bookmarkbutton.h"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

namespace Page {
enum LinkType_EM {
    LinkType_NULL = 0,
    LinkType_Goto,
    LinkType_GotoOtherFile,
    LinkType_Browse,
    LinkType_Execute
};

struct Link {
    QRectF boundary;

    int page;
    qreal left;
    qreal top;

    QString urlOrFileName;

    LinkType_EM type;

    Link() : boundary(), page(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(), type(LinkType_NULL) {}

    Link(const QRectF &boundingRect, int page, qreal left = qQNaN(), qreal top = qQNaN()) : boundary(boundingRect), page(page), left(left), top(top), urlOrFileName(), type(LinkType_Goto) {}

    Link(const QRectF &boundingRect, const QString &url, LinkType_EM type = LinkType_Browse) : boundary(boundingRect), page(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(url), type(type) {}

    Link(const QRectF &boundingRect, const QString &fileName, int page) : boundary(boundingRect), page(page), left(qQNaN()), top(qQNaN()), urlOrFileName(fileName), type(LinkType_GotoOtherFile) {}

};
}
struct stWord {
    QString s;
    QRectF rect;
};
enum RotateType_EM {
    RotateType_Normal = 0,
    RotateType_0,
    RotateType_90,
    RotateType_180,
    RotateType_270
};

class PageBase;
class PageInterface;
class PageBasePrivate;
class ThreadRenderImage : public QThread
{
    Q_OBJECT
public:
    ThreadRenderImage();
    void setPage(PageInterface *page, double width, double height);
    void setRestart();

protected:
    virtual void run();

signals:
    void signal_RenderFinish(QImage);
private:
    PageInterface *m_page;
    bool restart;
    double m_width;
    double m_height;
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
        m_pencolor = QColor(72, 118, 255, 0);
        m_searchcolor = Qt::yellow;
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
        if (threadreander.isRunning()) {
            threadreander.requestInterruption();
            threadreander.quit();
            threadreander.wait();
        }
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
    bool havereander;
    qreal pixelratiof;
    BookMarkButton *bookmarkbtn;
    DSpinner *m_spinner;
//    QMutex m_mutexlockgetimage;

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
    PageBase(PageBasePrivate *ptr = nullptr, DWidget *parent = 0);
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
        qDebug() << "PageBase setCurHightLight " << d->m_highlights.size();
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
    QRectF translateRect(QRectF &rect, double scale, RotateType_EM rotate);
    void clearHighlightRects();
    void setCurSearchShow(bool bshow)
    {
        Q_D(PageBase);
        d->m_bcursearchshow = bshow;
    }
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    void stopThread();
    void waitThread();
    void clearImage();
    bool setBookMarkState(bool state);

signals:
    void signal_MagnifierPixmapCacheLoaded(int);
    void signal_bookMarkStateChange(int page, bool state);
    void signal_update();
protected slots:
    void slot_loadMagnifierPixmapCache(QImage image, double width, double height);
    void slot_RenderFinish(QImage);
protected:
    void paintEvent(QPaintEvent *event) override;
protected:
    void getImagePoint(QPointF &point);

    PageBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageBase)

};

#endif // PAGEBASE_H

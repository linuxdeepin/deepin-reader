#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <DObject>
#include <DLabel>
#include <DGuiApplicationHelper>
#include <QThread>
#include "commonstruct.h"

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
class ThreadLoadMagnifierCache : public QThread
{
public:
    ThreadLoadMagnifierCache();
    void setPage(PageBase *page, double width, double height);
    void setRestart();

protected:
    virtual void run();

private:
    PageBase *m_page;
    bool restart;
    double m_width;
    double m_height;
};

class PageBase: public DLabel
{
    Q_OBJECT
public:
    PageBase(DWidget *parent = nullptr);
    ~PageBase();
    virtual bool getImage(QImage &image, double width, double height)
    {
        Q_UNUSED(image);
        Q_UNUSED(width);
        Q_UNUSED(height);
        return false;
    }
    virtual bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal)
    {
        Q_UNUSED(scale);
        Q_UNUSED(rotate);
        return false;
    }
    virtual bool getSlideImage(QImage &image, double &width, double &height)
    {
        Q_UNUSED(image);
        Q_UNUSED(width);
        Q_UNUSED(height);
        return false;
    }
    virtual bool loadWords()
    {
        return false;
    }
    virtual bool loadLinks()
    {
        return false;
    }
    virtual stSearchRes search(const QString& text, bool matchCase, bool wholeWords)const
    { Q_UNUSED(text); Q_UNUSED(matchCase); Q_UNUSED(wholeWords); stSearchRes res; return res; }
    QRectF setCurHightLight(int index)
    {
        QRectF rect;
        if(m_highlights.size()>0&&m_icurhightlight<m_highlights.size())
        {
             m_icurhightlight=index-1;
             rect=m_highlights.at(m_icurhightlight);
        }

        return  rect;
    }
    double getOriginalImageWidth()
    {
        return m_imagewidth;
    }
    double getOriginalImageHeight()
    {
        return m_imageheight;
    }
    void loadMagnifierPixmapCache(double width, double height);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100),
                            QColor pencolor = QColor(72, 118, 255, 0),
                            int penwidth = 0);
    void clearPageTextSelections();
    bool pageTextSelections(const QPoint start, const QPoint end);
    bool ifMouseMoveOverText(const QPoint point);
    bool getMagnifierPixmap(QPixmap &pixmap, QPoint point, int radius, double width, double height);
    bool clearMagnifierPixmap();
    void loadMagnifierCacheThreadStart(double width, double height);
    void setScaleAndRotate(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    Page::Link *ifMouseMoveOverLink(const QPoint point);
    bool getSelectTextString(QString &st);
    QRectF translateRect(QRectF& rect,double scale, RotateType_EM rotate);
    void clearHighlightRects();
    void setCurSearchShow(bool bshow){m_bcursearchshow=bshow;}
//    void setReSize(double scale = 1, RotateType_EM rotate = RotateType_Normal);
signals:
    void signal_MagnifierPixmapCacheLoaded(int);
protected:
    void paintEvent(QPaintEvent *event) override;
protected:
    void getImagePoint(QPoint &point);

    QColor m_paintercolor;
    QColor m_pencolor;
    QColor m_searchcolor;
    int m_penwidth;
    QList<QRect> paintrects;
    mutable QList<QRectF> m_highlights;
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
    double m_magnifierwidth;
    double m_magnifierheight;
    int m_pageno;
    int m_icurhightlight;//当前页搜索高亮获取"焦点"的下标
    bool m_bcursearchshow;//当前页是否是搜索结果当前显示页

};

#endif // PAGEBASE_H

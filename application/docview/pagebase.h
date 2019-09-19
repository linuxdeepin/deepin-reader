#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <DObject>
#include <DLabel>
#include <DGuiApplicationHelper>
#include <QThread>

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
    PageBase(DWidget *parent = 0);
    virtual bool getImage(QImage &image, double width, double height)
    {
        return false;
    }
    virtual bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal)
    {
        return false;
    }
    virtual bool getSlideImage(QImage &image, double &width, double &height)
    {
        return false;
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
protected:
    void paintEvent(QPaintEvent *event) override;
protected:
    void getImagePoint(QPoint &point);
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
};

#endif // PAGEBASE_H

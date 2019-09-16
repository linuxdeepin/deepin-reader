#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <QThread>
#include <poppler-qt5.h>

struct stWord {
    QString s;
    QRectF rect;
};
class PagePdf;
class ThreadLoadMagnifierCache : public QThread
{
public:
    ThreadLoadMagnifierCache();
    void setPage(PagePdf *page, double width, double height);
    void setRestart();

protected:
    virtual void run();

private:
    PagePdf *m_page;
    bool restart;
    double m_width;
    double m_height;
};

class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = 0);
    bool ifMouseMoveOverText(const QPoint point);
    bool pageTextSelections(const QPoint start, const QPoint end);
    void clearPageTextSelections();
    void appendWord(stWord word);
    void setPage(Poppler::Page *page);
    void setScaleAndRotate(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    bool getImage(QImage &image, double width, double height);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    void setImageWidth(double width);
    void setImageHeight(double height);
    bool getMagnifierPixmap(QPixmap &pixmap, QPoint point, int radius, double width, double height);
    bool clearMagnifierPixmap();
    //Annotation
    QString addAnnotation(QPoint screenPos);
    QString removeAnnotation(const QPoint& pos);
    void removeAnnotation(const QString& struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
    bool loadLinks();
    Page::Link *ifMouseMoveOverLink(const QPoint point);
    void loadMagnifierPixmapCache(double width, double height);
    void loadMagnifierCacheThreadStart(double width, double height);

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    void getImagePoint(QPoint &point);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
    QList<QRect> paintrects;
    QList<stWord> m_words;
    double m_imagewidth;
    double m_imageheight;
    Poppler::Page *m_page;
    QColor m_paintercolor;
    QColor m_pencolor;
    int m_penwidth;
    QPixmap m_magnifierpixmap;
    QList< Page::Link * > m_links;
    ThreadLoadMagnifierCache loadmagnifiercachethread;
};

#endif // PAGEPDF_H

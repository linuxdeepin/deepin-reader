#ifndef PAGEDJVU_H
#define PAGEDJVU_H
#include "../pagebase.h"
#include <poppler-qt5.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>
#include <QImage>
#include <QThread>

struct stWord {
    QString s;
    QRectF rect;
};
class DocummentDJVU;
class PageDJVU: public PageBase
{
    Q_OBJECT
public:
    PageDJVU(QWidget *parent = 0);
    bool ifMouseMoveOverText(const QPoint point);
    bool pageTextSelections(const QPoint start, const QPoint end);
    void clearPageTextSelections();
//    void appendWord(stWord word);
    void setPage(int pageno);
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
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
    bool loadWords();
    bool loadLinks();
    Page::Link *ifMouseMoveOverLink(const QPoint point);
    void loadMagnifierPixmapCache(double width, double height) override;
    void loadMagnifierCacheThreadStart(double width, double height);
    bool getSelectTextString(QString &st);
    bool getSlideImage(QImage &image, double &width, double &height);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    DocummentDJVU *m_parent;
    int m_pageno;
    int m_resolution;
    void removeAnnotation(Poppler::Annotation *annotation);
    void getImagePoint(QPoint &point);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
    QList<QRect> paintrects;
    QList<stWord> m_words;
    double m_imagewidth;
    double m_imageheight;
//    Poppler::Page *m_page;
    QColor m_paintercolor;
    QColor m_pencolor;
    int m_penwidth;
    QPixmap m_magnifierpixmap;
    QList< Page::Link * > m_links;
    int m_selecttextstartword;
    int m_selecttextendword;
};

#endif // PAGEDJVU_H

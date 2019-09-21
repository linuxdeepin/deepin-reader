#ifndef PAGETIFF_H
#define PAGETIFF_H
#include "../pagebase.h"
#include <tiff.h>
#include <tiffio.h>


class PageTiff: public PageBase
{
    Q_OBJECT
public:
    PageTiff(QWidget *parent = nullptr);
   bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
//    bool getImage(QImage &image, double width, double height) override;
//    bool getSlideImage(QImage &image, double &width, double &height) override;
//    void appendWord(stWord word);
    void setPage(int pageno,TIFF* document);
//    bool loadLinks();
//    //Annotation
//    QString addAnnotation(QPoint screenPos);
//    QString removeAnnotation(const QPoint &pos);
//    void removeAnnotation(const QString &struuid);
//    bool annotationClicked(const QPoint &pos, QString &strtext);
//private:
//    void removeAnnotation(Poppler::Annotation *annotation);
//    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
//    Poppler::Page *m_page;

    TIFF* m_document;
    int m_pageno;
    uint32 m_width;
    uint32 m_height;
    QImage m_image;
};

#endif // PAGETIFF_H

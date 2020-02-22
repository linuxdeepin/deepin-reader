#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <QDebug>
#include <QtConcurrent>
#include <QThread>
#include <QPaintDevice>
#include <poppler-qt5.h>
#include <unistd.h>
#include <QObject>

//class PagePdf;
class PagePdfPrivate;
class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = nullptr);
    ~PagePdf() override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(Poppler::Page *page, int pageno);
    //Annotation
    QString addAnnotation(const QColor &color);
    QString addAnnotation(const QColor &color, const QRect &rect);
    QString removeAnnotation(const QPoint &pos,AnnoteType_Em type);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    bool iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    QString addTextAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type);
    Poppler::Page *GetPage();
    stSearchRes search(const QString &text, bool matchCase, bool wholeWords) override;
    void changeAnnotationColor(const QString uuid, const QColor &color);
    void deletePage();
    void freshPage(Poppler::Page *page);

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QColor &color);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePdf)
};

#endif // PAGEPDF_H

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
#include <QSet>

//class PagePdf;
class PagePdfPrivate;
class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = nullptr);
    ~PagePdf() override;
    bool getImage(QImage &image, double width, double height) override;
    PageInterface *getInterFace() override;
    void setPage(Poppler::Page *page, int pageno);

    //Annotation
    QString addAnnotation(const QColor &color);
    QString addAnnotation(const QColor &color, const QRect &rect);
    QString addTextAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type);      //添加图标注释
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    bool iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid);      //查看当前点击处是否有icon

    Poppler::Page *GetPage();
    stSearchRes search(const QString &text, bool matchCase, bool wholeWords) override;
    void changeAnnotationColor(const QString uuid, const QColor &color);
    void deletePage();
    void freshPage(Poppler::Page *page);
    bool getrectimage(QImage &image, double width, double scalebase, double magnifierscale, QPoint &pt) override;
    bool renderImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;

    QImage thumbnail();

    static bool existIns(PageBase *item);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QString addHighlightAnnotation(const QColor &color);

private:
    static QSet<PageBase *> m_pageItems;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePdf)

    // PageBase interface
protected:
    QPointF globalPoint2Iner(const QPoint point) override;
};

#endif // PAGEPDF_H

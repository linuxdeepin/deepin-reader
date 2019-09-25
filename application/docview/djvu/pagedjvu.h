#ifndef PAGEDJVU_H
#define PAGEDJVU_H
#include "../pagebase.h"
#include <poppler-qt5.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>
#include <QImage>
#include <QThread>

class DocummentDJVU;
class PageDJVU: public PageBase
{
    Q_OBJECT
public:
    PageDJVU(QWidget *parent = 0);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
    void setPage(int pageno);
    //Annotation
    QString addAnnotation(QPoint screenPos);
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
private:
    DocummentDJVU *m_parent;
    int m_resolution;
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
};

#endif // PAGEDJVU_H

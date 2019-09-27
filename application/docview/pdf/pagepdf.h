#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <poppler-qt5.h>
class PagePdf;
class PagePdfPrivate;
class PagePdfPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
public:
    PagePdfPrivate(PagePdf *parent): PageBasePrivate((PageBase *)parent)
    {
        m_page = nullptr;
    }

    ~PagePdfPrivate()
    {
    }
    bool getImage(QImage &image, double width, double height)
    {
        int xres = 72.0, yres = 72.0;
        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        if (!m_page)
            return false;
        image = m_page->renderToImage(xres * scalex, yres * scaley, -1, -1, width, height);
        return true;
    }
    bool getSlideImage(QImage &image, double &width, double &height)
    {
        int xres = 72.0, yres = 72.0;
        double scalex = (width - 20) / m_imagewidth;
        double scaley = (height - 20) / m_imageheight;
        double scale = 1;
        if (scalex > scaley) {
            scale = scaley;
        } else {
            scale = scalex;
        }
        width = m_imagewidth * scale;
        height = m_imageheight * scale;
        if (!m_page)
            return false;
        image = m_page->renderToImage(xres * scale, yres * scale, -1, -1, width, height);
        return true;
    }
    Poppler::Page *m_page;
};
class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = 0);
    ~PagePdf();
//    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
    PageInterface *getInterFace() override;
//    void appendWord(stWord word);
    void setPage(Poppler::Page *page, int pageno);
    //Annotation
    QString addAnnotation(QPoint screenPos);
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
    Poppler::Page *GetPage();
//    void clearThread();
//protected slots:
//    void slot_RenderFinish(QImage);
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePdf)
};

#endif // PAGEPDF_H

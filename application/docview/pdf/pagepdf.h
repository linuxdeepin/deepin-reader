#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <poppler-qt5.h>
class ThreadRenderImage : public QThread
{
    Q_OBJECT
public:
    ThreadRenderImage();
    void setPage(Poppler::Page *page, double xres, double yres, double width, double height);
    void setRestart();

protected:
    virtual void run();

signals:
    void signal_RenderFinish(QImage);
private:
    Poppler::Page *m_page;
    bool restart;
    double m_width;
    double m_height;
    double m_xres;
    double m_yres;
};
class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = 0);
    ~PagePdf();
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
//    void appendWord(stWord word);
    void setPage(Poppler::Page *page, int pageno);
    //Annotation
    QString addAnnotation(QPoint screenPos);
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
    Poppler::Page *GetPage();
    void clearThread();
    stSearchRes search(const QString& text, bool matchCase, bool wholeWords)const override;
protected:
    void paintEvent(QPaintEvent *event) override;
protected slots:
    void slot_RenderFinish(QImage);
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text);
    Poppler::Page *m_page;
    ThreadRenderImage threadreander;
};

#endif // PAGEPDF_H

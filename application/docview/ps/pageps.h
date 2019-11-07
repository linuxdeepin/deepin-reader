#ifndef PAGEPS_H
#define PAGEPS_H
#include "../pagebase.h"
#include <libspectre/spectre-document.h>
#include <QImage>
#include <QThread>

class DocummentPS;

class PagePS;
class PagePSPrivate;

class PagePS: public PageBase
{
    Q_OBJECT
public:
    PagePS(QWidget *parent = nullptr);
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePS)
};


#endif // PAGEPS_H

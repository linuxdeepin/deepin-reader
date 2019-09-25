#ifndef PAGEPS_H
#define PAGEPS_H
#include "../pagebase.h"
#include <libspectre/spectre-document.h>
#include <QImage>
#include <QThread>

class DocummentPS;
class PagePS: public PageBase
{
    Q_OBJECT
public:
    PagePS(QWidget *parent = 0);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
    void setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno);
private:
    SpectrePage *m_page;
    SpectreRenderContext *m_renderContext;
};

#endif // PAGEPS_H

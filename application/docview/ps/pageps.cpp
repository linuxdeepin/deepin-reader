#include "pageps.h"
#include "docview/publicfunc.h"
#include "docummentps.h"
#include <QQueue>
#include <QDebug>

PagePS::PagePS(QWidget *parent)
    : PageBase(new PagePSPrivate(this), parent)
{
}

void PagePS::setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno)
{
    Q_D(PagePS);
    d->setPage(page, renderContext, pageno);
}

//bool PagePS::showImage(double scale, RotateType_EM rotate)
//{
//    if (!m_page)
//        return false;

//    m_scale = scale;
//    m_rotate = rotate;
////    double xscale;
////    double yscale;

////    switch (rotation) {
////    default:
////    case RotateBy0:
////    case RotateBy180:
////        xscale = horizontalResolution / 72.0;
////        yscale = verticalResolution / 72.0;
////        break;
////    case RotateBy90:
////    case RotateBy270:
////        xscale = verticalResolution / 72.0;
////        yscale = horizontalResolution / 72.0;
////        break;
////    }

//    spectre_render_context_set_scale(m_renderContext, scale, scale);

////    switch (rotation) {
////    default:
////    case RotateBy0:
////        spectre_render_context_set_rotation(m_renderContext, 0);
////        break;
////    case RotateBy90:
////        spectre_render_context_set_rotation(m_renderContext, 90);
////        break;
////    case RotateBy180:
////        spectre_render_context_set_rotation(m_renderContext, 180);
////        break;
////    case RotateBy270:
////        spectre_render_context_set_rotation(m_renderContext, 270);
////        break;
////    }

//    int w = m_imagewidth * scale;
//    int h = m_imageheight * scale;

////    spectre_page_get_size(m_page, &w, &h);

////    w = qRound(w * xscale);
////    h = qRound(h * yscale);

////    if (rotation == RotateBy90 || rotation == RotateBy270) {
////        qSwap(w, h);
////    }

//    unsigned char *pageData = 0;
//    int rowLength = 0;

//    spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

//    if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
//        free(pageData);
//        pageData = 0;

//        return false;
//    }

//    QImage auxiliaryImage(pageData, rowLength / 4, h, QImage::Format_RGB32);
//    QImage image(auxiliaryImage.copy(0, 0, w, h));

//    free(pageData);
//    pageData = 0;
//    QPixmap map = QPixmap::fromImage(image);
//    QMatrix leftmatrix;
//    switch (m_rotate) {
//    case RotateType_90:
//        leftmatrix.rotate(90);
//        break;
//    case RotateType_180:
//        leftmatrix.rotate(180);
//        break;
//    case RotateType_270:
//        leftmatrix.rotate(270);
//        break;
//    default:
//        leftmatrix.rotate(0);
//        break;
//    }
//    setPixmap(map.transformed(leftmatrix, Qt::SmoothTransformation));
//    return true;
//}

bool PagePS::getSlideImage(QImage &image, double &width, double &height)
{
    Q_D(PagePS);
    return d->getSlideImage(image, width, height);
}

bool PagePS::getImage(QImage &image, double width, double height)
{
    Q_D(PagePS);
    return d->getImage(image, width, height);
}

PageInterface *PagePS::getInterFace()
{
    Q_D(PagePS);
    return d;
}


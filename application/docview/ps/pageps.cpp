#include "pageps.h"
#include "docview/publicfunc.h"
#include "docummentps.h"
#include <QQueue>
#include <QDebug>

PagePS::PagePS(QWidget *parent)
    : PageBase(parent),
      m_page(nullptr)
{
    //    setFrameShape (QFrame::Box);
    //    setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);");
}

void PagePS::setPage(SpectrePage *page, SpectreRenderContext *renderContext)
{
//    qDebug() << "----setpage";
    m_page = page;
    m_renderContext = renderContext;
    int w, h;
    spectre_page_get_size(m_page, &w, &h);
    m_imagewidth = w;
    m_imageheight = h;
    qDebug() << "----setpage m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight;
}

bool PagePS::showImage(double scale, RotateType_EM rotate)
{
    if (!m_page)
        return false;

    m_scale = scale;
    m_rotate = rotate;
//    double xscale;
//    double yscale;

//    switch (rotation) {
//    default:
//    case RotateBy0:
//    case RotateBy180:
//        xscale = horizontalResolution / 72.0;
//        yscale = verticalResolution / 72.0;
//        break;
//    case RotateBy90:
//    case RotateBy270:
//        xscale = verticalResolution / 72.0;
//        yscale = horizontalResolution / 72.0;
//        break;
//    }

    spectre_render_context_set_scale(m_renderContext, scale, scale);

//    switch (rotation) {
//    default:
//    case RotateBy0:
//        spectre_render_context_set_rotation(m_renderContext, 0);
//        break;
//    case RotateBy90:
//        spectre_render_context_set_rotation(m_renderContext, 90);
//        break;
//    case RotateBy180:
//        spectre_render_context_set_rotation(m_renderContext, 180);
//        break;
//    case RotateBy270:
//        spectre_render_context_set_rotation(m_renderContext, 270);
//        break;
//    }

    int w = m_imagewidth * scale;
    int h = m_imageheight * scale;

//    spectre_page_get_size(m_page, &w, &h);

//    w = qRound(w * xscale);
//    h = qRound(h * yscale);

//    if (rotation == RotateBy90 || rotation == RotateBy270) {
//        qSwap(w, h);
//    }

    unsigned char *pageData = 0;
    int rowLength = 0;

    spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

    if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
        free(pageData);
        pageData = 0;

        return false;
    }

    QImage auxiliaryImage(pageData, rowLength / 4, h, QImage::Format_RGB32);
    QImage image(auxiliaryImage.copy(0, 0, w, h));

    free(pageData);
    pageData = 0;
    QPixmap map = QPixmap::fromImage(image);
    QMatrix leftmatrix;
    switch (m_rotate) {
    case RotateType_90:
        leftmatrix.rotate(90);
        break;
    case RotateType_180:
        leftmatrix.rotate(180);
        break;
    case RotateType_270:
        leftmatrix.rotate(270);
        break;
    default:
        leftmatrix.rotate(0);
        break;
    }
    setPixmap(map.transformed(leftmatrix, Qt::SmoothTransformation));
    return true;
}

bool PagePS::getSlideImage(QImage &image, double &width, double &height)
{
    if (!m_page)
        return false;

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
    spectre_render_context_set_scale(m_renderContext, scale, scale);
    unsigned char *pageData = 0;
    int rowLength = 0;

    spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

    if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
        free(pageData);
        pageData = 0;

        return false;
    }

    QImage auxiliaryImage(pageData, rowLength / 4, height, QImage::Format_RGB32);
    image = QImage(auxiliaryImage.copy(0, 0, width, height));

    free(pageData);
    pageData = 0;
    return true;
}

bool PagePS::getImage(QImage &image, double width, double height)
{
    if (!m_page)
        return false;


    double scalex = width / m_imagewidth;
    double scaley = height / m_imageheight;
    spectre_render_context_set_scale(m_renderContext, scalex, scaley);
    unsigned char *pageData = 0;
    int rowLength = 0;

    spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

    if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
        free(pageData);
        pageData = 0;

        return false;
    }

    QImage auxiliaryImage(pageData, rowLength / 4, height, QImage::Format_RGB32);
    image = QImage(auxiliaryImage.copy(0, 0, width, height));

    free(pageData);
    pageData = 0;
    return true;
}

bool PagePS::loadWords()
{
    if (!m_page)
        return false;
//    int res = 72.0;
//    miniexp_t r;
////    while ( ( r = ddjvu_document_get_pagetext( m_parent->getDocument(), m_pageno, nullptr ) ) == miniexp_dummy )
////        handle_ddjvu_messages( m_parent->getContext(), true );

//    if ( r == miniexp_nil )
//        return false;
//    m_words.clear();
////    QList<KDjVu::TextEntity> ret;

//    int height = m_imageheight;

//    QQueue<miniexp_t> queue;
//    queue.enqueue( r );

//    while ( !queue.isEmpty() ) {
//        miniexp_t cur = queue.dequeue();

//        if ( miniexp_listp( cur )
//                && ( miniexp_length( cur ) > 0 )
//                && miniexp_symbolp( miniexp_nth( 0, cur ) ) ) {
//            int size = miniexp_length( cur );
//            QString sym = QString::fromUtf8( miniexp_to_name( miniexp_nth( 0, cur ) ) );
//            if ( sym == "word" ) {
//                if ( size >= 6 ) {
//                    int xmin = miniexp_to_int( miniexp_nth( 1, cur ) ) * res / m_resolution;
//                    int ymin = miniexp_to_int( miniexp_nth( 2, cur ) ) * res / m_resolution;
//                    int xmax = miniexp_to_int( miniexp_nth( 3, cur ) ) * res / m_resolution;
//                    int ymax = miniexp_to_int( miniexp_nth( 4, cur ) ) * res / m_resolution;
//                    QRect rect( xmin, height - ymax, xmax - xmin, ymax - ymin );
//                    stWord sword;
//                    sword.rect = rect;
//                    sword.s = QString::fromUtf8( miniexp_to_str( miniexp_nth( 5, cur ) ) );
//                    qDebug() << "djvu word:" << sword.s << " rect:" << rect;
////                    qDebug() << "xmin:" << xmin << " ymin:" << ymin << " xmax:" << xmax << " ymax:" << ymax;
//                    m_words.append( sword );
//                }
//            } else {
//                for ( int i = 5; i < size; ++i )
//                    queue.enqueue( miniexp_nth( i, cur ) );
//            }
//        }
//    }
    return true;
}
bool PagePS::loadLinks()
{
    if (!m_page)
        return false;
//    for (int i = 0; i < m_links.size(); i++) {
//        delete m_links.at(i);
//    }
//    m_links.clear();

//    miniexp_t pageAnnoExp = miniexp_nil;

//    {
//        while (true) {
//            pageAnnoExp = ddjvu_document_get_pageanno(m_parent->getDocument(), m_pageno);

//            if (pageAnnoExp == miniexp_dummy) {
////                clearMessageQueue(m_parent->getContext(), true);
//            } else {
//                break;
//            }
//        }
//    }

//    int res = 72.0;
////    const QList< Page::Link * > links = djvuLoadLinks(pageAnnoExp, QSizeF(m_imagewidth * m_resolution / res, m_imageheight * m_resolution / res), m_pageno, m_parent->getPageByName());
////    {

////        ddjvu_miniexp_release(m_parent->getDocument(), pageAnnoExp);
////    }

////    m_links = links;
//    for (int i = 0; i < m_links.size(); i++) {
////        qDebug() << "m_links i:" << i << " boundary:" << m_links.at(i)->boundary << " width:" << m_links.at(i)->boundary.width()* m_imagewidth << " height:" << m_links.at(i)->boundary.height()* m_imageheight;
//        m_links.at(i)->boundary = QRectF(m_links.at(i)->boundary.x() * m_imagewidth,
//                                         m_links.at(i)->boundary.y() * m_imageheight,
//                                         m_links.at(i)->boundary.width() * m_imagewidth,
//                                         m_links.at(i)->boundary.height() * m_imageheight);
//        qDebug() << "m_links i:" << i << " boundary:" << m_links.at(i)->boundary;
//    }
    return true;
}


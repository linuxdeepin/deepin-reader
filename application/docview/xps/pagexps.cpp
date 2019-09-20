#include "pagexps.h"
#include "docview/publicfunc.h"
#include <QQueue>
#include <QDebug>

PageXPS::PageXPS(QWidget *parent)
    : PageBase(parent),
      m_page(nullptr)
{
    //    setFrameShape (QFrame::Box);
    //    setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);");
}

void PageXPS::setPage(XpsPage *page)
{
//    qDebug() << "----setpage";
    m_page = page;
    m_imagewidth = page->size().width();
    m_imageheight = page->size().height();
    qDebug() << "----setpage m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight;
}

bool PageXPS::showImage(double scale, RotateType_EM rotate)
{
    if (!m_page)
        return false;

    m_scale = scale;
    m_rotate = rotate;
    double w = m_imagewidth * scale;
    double h = m_imageheight * scale;
    QImage image(QSize(w, h), QImage::Format_ARGB32);
    m_page->renderToImage(&image);
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

bool PageXPS::getSlideImage(QImage &image, double &width, double &height)
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
    image = QImage(QSize(width, height), QImage::Format_ARGB32);
    m_page->renderToImage(&image);
    return true;
}

bool PageXPS::getImage(QImage &image, double width, double height)
{
    if (!m_page)
        return false;
    image = QImage(QSize(width, height), QImage::Format_ARGB32);
    m_page->renderToImage(&image);
    return true;
}

bool PageXPS::loadWords()
{
    if (!m_page)
        return false;
    m_page->loadPageWords(&m_words);
    return true;
}
bool PageXPS::loadLinks()
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


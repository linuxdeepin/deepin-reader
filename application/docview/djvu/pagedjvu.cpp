#include "pagedjvu.h"
#include "docview/publicfunc.h"
#include "docummentdjvu.h"
#include <QQueue>
#include <QDebug>
inline miniexp_t miniexp_cadddr(miniexp_t exp)
{
    return miniexp_cadr(miniexp_cddr(exp));
}

QPainterPath loadLinkBoundary(const QString &type, miniexp_t boundaryExp, QSizeF size)
{
    QPainterPath boundary;

    const int count = miniexp_length(boundaryExp);

    if (count == 4 && (type == QLatin1String("rect") || type == QLatin1String("oval"))) {
        QPoint p(miniexp_to_int(miniexp_car(boundaryExp)), miniexp_to_int(miniexp_cadr(boundaryExp)));
        QSize s(miniexp_to_int(miniexp_caddr(boundaryExp)), miniexp_to_int(miniexp_cadddr(boundaryExp)));

        p.setY(size.height() - s.height() - p.y());

        const QRectF r(p, s);

        if (type == QLatin1String("rect")) {
            boundary.addRect(r);
        } else {
            boundary.addEllipse(r);
        }
    } else if (count > 0 && count % 2 == 0 && type == QLatin1String("poly")) {
        QPolygon polygon;

        for (int index = 0; index < count; index += 2) {
            QPoint p(miniexp_to_int(miniexp_nth(index, boundaryExp)), miniexp_to_int(miniexp_nth(index + 1, boundaryExp)));

            p.setY(size.height() - p.y());

            polygon << p;
        }

        boundary.addPolygon(polygon);
    }

    return QTransform::fromScale(1.0 / size.width(), 1.0 / size.height()).map(boundary);
}

Page::Link *loadLinkTarget(const QPainterPath &boundary, miniexp_t targetExp, int index, const QHash< QString, int > &pageByName)
{
    QString target;

    if (miniexp_stringp(targetExp)) {
        target = QString::fromUtf8(miniexp_to_str(targetExp));
    } else if (miniexp_length(targetExp) == 3 && qstrcmp(miniexp_to_name(miniexp_car(targetExp)), "url") == 0) {
        target = QString::fromUtf8(miniexp_to_str(miniexp_cadr(targetExp)));
    }

    if (target.isEmpty()) {
        return 0;
    }

    if (target.at(0) == QLatin1Char('#')) {
        target.remove(0, 1);

        bool ok = false;
        int targetPage = target.toInt(&ok);

        if (!ok) {
            const int page = pageByName.value(target);

            if (page != 0) {
                targetPage = page;
            } else {
                return 0;
            }
        } else {
            if (target.at(0) == QLatin1Char('+') || target.at(0) == QLatin1Char('-')) {
                targetPage += index + 1;
            }
        }

        return new Page::Link(boundary.boundingRect(), targetPage);
    } else {
        return new Page::Link(boundary.boundingRect(), target);
    }
}

QList< Page::Link * > djvuLoadLinks(miniexp_t linkExp, QSizeF size, int index, const QHash< QString, int > &pageByName)
{
    QList< Page::Link * > links;

    for (miniexp_t linkItem = miniexp_nil; miniexp_consp(linkExp); linkExp = miniexp_cdr(linkExp)) {
        linkItem = miniexp_car(linkExp);

        if (miniexp_length(linkItem) < 4 || qstrcmp(miniexp_to_name(miniexp_car(linkItem)), "maparea") != 0) {
            continue;
        }

        miniexp_t targetExp = miniexp_cadr(linkItem);
        miniexp_t boundaryExp = miniexp_cadddr(linkItem);

        if (!miniexp_symbolp(miniexp_car(boundaryExp))) {
            continue;
        }

        const QString type = QString::fromUtf8(miniexp_to_name(miniexp_car(boundaryExp)));

        if (type == QLatin1String("rect") || type == QLatin1String("oval") || type == QLatin1String("poly")) {
            QPainterPath boundary = loadLinkBoundary(type, miniexp_cdr(boundaryExp), size);

            if (!boundary.isEmpty()) {
                Page::Link *link = loadLinkTarget(boundary, targetExp, index, pageByName);

                if (link != 0) {
                    links.append(link);
                }
            }
        }
    }

    return links;
}

static void handle_ddjvu_messages( ddjvu_context_t *ctx, int wait )
{
    const ddjvu_message_t *msg;
    if ( wait )
        ddjvu_message_wait( ctx );
    while ( ( msg = ddjvu_message_peek( ctx ) ) ) {
//        which_ddjvu_message( msg );
        ddjvu_message_pop( ctx );
    }
}

void clearMessageQueue(ddjvu_context_t *context, bool wait)
{
    if (wait) {
        ddjvu_message_wait(context);
    }

    while (true) {
        if (ddjvu_message_peek(context) != 0) {
            ddjvu_message_pop(context);
        } else {
            break;
        }
    }
}

PageDJVU::PageDJVU(QWidget *parent)
    : PageBase(parent),
      m_parent((DocummentDJVU *)parent),
      m_resolution(0),
      m_pageno(-1)
{

    //    setFrameShape (QFrame::Box);
    //    setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);");
}


void PageDJVU::removeAnnotation(Poppler::Annotation *annotation)
{
//    m_page->removeAnnotation(annotation);
}


void PageDJVU::setPage(int pageno)
{
//    qDebug() << "----setpage";
    int xres = 72.0, yres = 72.0;
    m_pageno = pageno;
    ddjvu_status_t status;
    ddjvu_pageinfo_t pageinfo;
    while (true) {
        status = ddjvu_document_get_pageinfo(m_parent->getDocument(), m_pageno, &pageinfo);

        if (status < DDJVU_JOB_OK) {
            clearMessageQueue(m_parent->getContext(), true);
        } else {
            break;
        }
    }
    if (status >= DDJVU_JOB_FAILED) {
        return ;
    }
    m_resolution = pageinfo.dpi;
    m_imagewidth = pageinfo.width * xres / m_resolution;
    m_imageheight = pageinfo.height * yres / m_resolution;
    qDebug() << "----setpage pageno:" << pageno << " m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight << " m_resolution:" << m_resolution;
}

bool PageDJVU::showImage(double scale, RotateType_EM rotate)
{
    if (!m_parent || m_pageno < 0)
        return false;
    int xres = 72.0, yres = 72.0;
    m_scale = scale;
    m_rotate = rotate;
    ddjvu_page_t *page = ddjvu_page_create_by_pageno(m_parent->getDocument(), m_pageno);

    if (page == 0) {
        return false;
    }

    ddjvu_status_t status;

    while (true) {
        status = ddjvu_page_decoding_status(page);

        if (status < DDJVU_JOB_OK) {
            clearMessageQueue(m_parent->getContext(), true);
        } else {
            break;
        }
    }

    if (status >= DDJVU_JOB_FAILED) {
        ddjvu_page_release(page);

        return false;
    }

//    switch (rotate) {
//    default:
//    case RotateType_Normal:
//        ddjvu_page_set_rotation(page, DDJVU_ROTATE_0);
//        break;
//    case RotateType_90:
//        ddjvu_page_set_rotation(page, DDJVU_ROTATE_90);
//        break;
//    case RotateType_180:
//        ddjvu_page_set_rotation(page, DDJVU_ROTATE_180);
//        break;
//    case RotateType_270:
//        ddjvu_page_set_rotation(page, DDJVU_ROTATE_270);
//        break;
//    }

    ddjvu_rect_t pagerect;

    pagerect.x = 0;
    pagerect.y = 0;
    pagerect.w = m_scale * m_imagewidth;
    pagerect.h = m_scale * m_imageheight;

//    switch (rotate) {
//    default:
//    case RotateType_Normal:
//    case RotateType_180:
////        pagerect.w = qRound(xres * scale / m_resolution * m_imagewidth);
////        pagerect.h = qRound(yres * scale / m_resolution * m_imageheight);
//        break;
//    case RotateType_90:
//        pagerect.w = m_scale * m_imageheight;
//        pagerect.h = m_scale * m_imagewidth;
//        break;
//    case RotateType_270:
//        pagerect.w = m_scale * m_imageheight;
//        pagerect.h = m_scale * m_imagewidth;
//        break;
//    }

    QImage image(pagerect.w, pagerect.h, QImage::Format_RGB32);

    if (!ddjvu_page_render(page, DDJVU_RENDER_COLOR, &pagerect, &pagerect, m_parent->getFormat(), image.bytesPerLine(), reinterpret_cast< char * >(image.bits()))) {
        image = QImage();
    }

    clearMessageQueue(m_parent->getContext(), false);

    ddjvu_page_release(page);
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

bool PageDJVU::getSlideImage(QImage &image, double &width, double &height)
{
    if (!m_parent || m_pageno < 0)
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
    ddjvu_page_t *page = ddjvu_page_create_by_pageno(m_parent->getDocument(), m_pageno);

    if (page == 0) {
        return false;
    }

    ddjvu_status_t status;

    while (true) {
        status = ddjvu_page_decoding_status(page);

        if (status < DDJVU_JOB_OK) {
            clearMessageQueue(m_parent->getContext(), true);
        } else {
            break;
        }
    }

    if (status >= DDJVU_JOB_FAILED) {
        ddjvu_page_release(page);

        return false;
    }

    ddjvu_rect_t pagerect;

    pagerect.x = 0;
    pagerect.y = 0;
    pagerect.w = width;
    pagerect.h = height;


    image = QImage(pagerect.w, pagerect.h, QImage::Format_RGB32);

    if (!ddjvu_page_render(page, DDJVU_RENDER_COLOR, &pagerect, &pagerect, m_parent->getFormat(), image.bytesPerLine(), reinterpret_cast< char * >(image.bits()))) {
        image = QImage();
    }

    clearMessageQueue(m_parent->getContext(), false);

    ddjvu_page_release(page);
    return true;
}

bool PageDJVU::getImage(QImage &image, double width, double height)
{
    if (!m_parent || m_pageno < 0)
        return false;

    ddjvu_page_t *page = ddjvu_page_create_by_pageno(m_parent->getDocument(), m_pageno);

    if (page == 0) {
        return false;
    }

    ddjvu_status_t status;

    while (true) {
        status = ddjvu_page_decoding_status(page);

        if (status < DDJVU_JOB_OK) {
            clearMessageQueue(m_parent->getContext(), true);
        } else {
            break;
        }
    }

    if (status >= DDJVU_JOB_FAILED) {
        ddjvu_page_release(page);

        return false;
    }

    ddjvu_rect_t pagerect;

    pagerect.x = 0;
    pagerect.y = 0;
    pagerect.w = width;
    pagerect.h = height;


    image = QImage(pagerect.w, pagerect.h, QImage::Format_RGB32);

    if (!ddjvu_page_render(page, DDJVU_RENDER_COLOR, &pagerect, &pagerect, m_parent->getFormat(), image.bytesPerLine(), reinterpret_cast< char * >(image.bits()))) {
        image = QImage();
    }

    clearMessageQueue(m_parent->getContext(), false);

    ddjvu_page_release(page);
    return true;
}

bool PageDJVU::loadWords()
{
    if (!m_parent || m_pageno < 0)
        return false;
    int res = 72.0;
    miniexp_t r;
    while ( ( r = ddjvu_document_get_pagetext( m_parent->getDocument(), m_pageno, nullptr ) ) == miniexp_dummy )
        handle_ddjvu_messages( m_parent->getContext(), true );

    if ( r == miniexp_nil )
        return false;
    m_words.clear();
//    QList<KDjVu::TextEntity> ret;

    int height = m_imageheight;

    QQueue<miniexp_t> queue;
    queue.enqueue( r );

    while ( !queue.isEmpty() ) {
        miniexp_t cur = queue.dequeue();

        if ( miniexp_listp( cur )
                && ( miniexp_length( cur ) > 0 )
                && miniexp_symbolp( miniexp_nth( 0, cur ) ) ) {
            int size = miniexp_length( cur );
            QString sym = QString::fromUtf8( miniexp_to_name( miniexp_nth( 0, cur ) ) );
            if ( sym == "word" ) {
                if ( size >= 6 ) {
                    int xmin = miniexp_to_int( miniexp_nth( 1, cur ) ) * res / m_resolution;
                    int ymin = miniexp_to_int( miniexp_nth( 2, cur ) ) * res / m_resolution;
                    int xmax = miniexp_to_int( miniexp_nth( 3, cur ) ) * res / m_resolution;
                    int ymax = miniexp_to_int( miniexp_nth( 4, cur ) ) * res / m_resolution;
                    QRect rect( xmin, height - ymax, xmax - xmin, ymax - ymin );
                    stWord sword;
                    sword.rect = rect;
                    sword.s = QString::fromUtf8( miniexp_to_str( miniexp_nth( 5, cur ) ) );
                    qDebug() << "djvu word:" << sword.s << " rect:" << rect;
//                    qDebug() << "xmin:" << xmin << " ymin:" << ymin << " xmax:" << xmax << " ymax:" << ymax;
                    m_words.append( sword );
                }
            } else {
                for ( int i = 5; i < size; ++i )
                    queue.enqueue( miniexp_nth( i, cur ) );
            }
        }
    }
    return true;
}
bool PageDJVU::loadLinks()
{
    if (!m_parent || m_pageno < 0)
        return false;
    for (int i = 0; i < m_links.size(); i++) {
        delete m_links.at(i);
    }
    m_links.clear();

    miniexp_t pageAnnoExp = miniexp_nil;

    {
        while (true) {
            pageAnnoExp = ddjvu_document_get_pageanno(m_parent->getDocument(), m_pageno);

            if (pageAnnoExp == miniexp_dummy) {
                clearMessageQueue(m_parent->getContext(), true);
            } else {
                break;
            }
        }
    }

    int res = 72.0;
    const QList< Page::Link * > links = djvuLoadLinks(pageAnnoExp, QSizeF(m_imagewidth * m_resolution / res, m_imageheight * m_resolution / res), m_pageno, m_parent->getPageByName());
    {

        ddjvu_miniexp_release(m_parent->getDocument(), pageAnnoExp);
    }

    m_links = links;
    for (int i = 0; i < m_links.size(); i++) {
//        qDebug() << "m_links i:" << i << " boundary:" << m_links.at(i)->boundary << " width:" << m_links.at(i)->boundary.width()* m_imagewidth << " height:" << m_links.at(i)->boundary.height()* m_imageheight;
        m_links.at(i)->boundary = QRectF(m_links.at(i)->boundary.x() * m_imagewidth,
                                         m_links.at(i)->boundary.y() * m_imageheight,
                                         m_links.at(i)->boundary.width() * m_imagewidth,
                                         m_links.at(i)->boundary.height() * m_imageheight);
        qDebug() << "m_links i:" << i << " boundary:" << m_links.at(i)->boundary;
    }
    return true;
}


QString PageDJVU::addAnnotation(QPoint screenPos)
{
    QString uniqueName;
    if (paintrects.size() > 0) {
        QRectF rectboundry;
        QList<QRectF> listrectf;
        foreach (QRect rect, paintrects) {
            rectboundry = rect;
            listrectf.append(rectboundry);
        }
        uniqueName = addHighlightAnnotation(listrectf, Qt::red);
        showImage(m_scale, m_rotate);
    }
    return  uniqueName;
}

QString PageDJVU::addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color)
{
    QString uniqueName;
//    qDebug() << "*************" << listrect.size();
//    if (listrect.size() <= 0)return uniqueName;
//    Poppler::Annotation::Style style;
//    style.setColor(color);

//    Poppler::Annotation::Popup popup;
//    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

//    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

//    Poppler::HighlightAnnotation::Quad quad;
//    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
//    QRectF rec, recboundary;
//    double curwidth = m_imagewidth * m_scale;
//    double curheight = m_imageheight * m_scale;
//    foreach (rec, listrect) {
//        recboundary.setTopLeft(QPointF(rec.left() / curwidth,
//                                       rec.top() / curheight));
//        recboundary.setTopRight(QPointF(rec.right() / curwidth,
//                                        rec.top() / curheight));
//        recboundary.setBottomLeft(QPointF(rec.left() / curwidth,
//                                          rec.bottom() / curheight));
//        recboundary.setBottomRight(QPointF(rec.right() / curwidth,
//                                           rec.bottom() / curheight));
//        qDebug() << "**" << rec << "**";
//        quad.points[0] = recboundary.topLeft();
//        quad.points[1] = recboundary.topRight();
//        quad.points[2] = recboundary.bottomRight();
//        quad.points[3] = recboundary.bottomLeft();
//        qlistquad.append(quad);
//    }
//    annotation->setHighlightQuads(qlistquad);
//    uniqueName = PublicFunc::getUuid();
//    annotation->setUniqueName(uniqueName);
//    annotation->setStyle(style);
//    annotation->setPopup(popup);
//    m_page->addAnnotation(annotation);
    return  uniqueName;
}

QString PageDJVU::removeAnnotation(const QPoint &pos)
{
    double curwidth = m_scale * m_imagewidth;
    double curheight = m_scale * m_imageheight;
    QString uniqueName;
//    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
//    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
//    QList<Poppler::Annotation *> listannote = m_page->annotations();
//    foreach (Poppler::Annotation *annote, listannote) {
//        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
//            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
//            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
//                QRectF rectbound;
//                rectbound.setTopLeft(quad.points[0]);
//                rectbound.setTopRight(quad.points[1]);
//                rectbound.setBottomLeft(quad.points[2]);
//                rectbound.setBottomRight( quad.points[3]);
//                if (rectbound.contains(ptf)) {
//                    uniqueName = annote->uniqueName();
//                    removeAnnotation(annote);
//                } else {
//                    qDebug() << "******* not contains";
//                }
//            }
//        }
//    }
//    showImage(m_scale, m_rotate);
    return uniqueName;
}

void PageDJVU::removeAnnotation(const QString &struuid)
{
//    QList<Poppler::Annotation *> listannote = m_page->annotations();
//    foreach (Poppler::Annotation *annote, listannote) {
//        /*annote->subType()==Poppler::Annotation::AHighlight&&*/
//        if (!struuid.isEmpty() && struuid.compare(annote->uniqueName()) == 0) { //必须判断
//            removeAnnotation(annote);
//            showImage(m_scale, m_rotate);
//        }
//    }
}

bool PageDJVU::annotationClicked(const QPoint &pos, QString &strtext)
{
//    const double scaleX = m_scale;
//    const double scaleY = m_scale;
//    double curwidth = m_scale * m_imagewidth;
//    double curheight = m_scale * m_imageheight;
//    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
//    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
//    QList<Poppler::Annotation *> listannote = m_page->annotations();
//    foreach (Poppler::Annotation *annote, listannote) {
//        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
//            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
//            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
//                QRectF rectbound;
//                rectbound.setTopLeft(quad.points[0]);
//                rectbound.setTopRight(quad.points[1]);
//                rectbound.setBottomLeft(quad.points[2]);
//                rectbound.setBottomRight( quad.points[3]);
//                qDebug() << "########" << quad.points[0];
//                if (rectbound.contains(ptf)) {
//                    qDebug() << "******* contaions";
//                    return true;
//                } else {
//                    qDebug() << "******* not contains";
//                }
//            }
//        }
//    }
    return  false;
}


#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QDebug>
#include <QPainter>

PagePdf::PagePdf(QWidget *parent)
    : PageBase(new PagePdfPrivate(this), parent)
{
}

PagePdf::~PagePdf()
{
}

stSearchRes PagePdf::search(const QString &text, bool matchCase, bool wholeWords)
{
    Q_D(PagePdf);
    d->m_highlights.clear();
    stSearchRes stres;
    stres.ipage = d_ptr->m_pageno;
    stres.listtext.clear();
    const Poppler::Page::SearchFlags flags((matchCase ? 0 : Poppler::Page::IgnoreCase) | (wholeWords ? Poppler::Page::WholeWords : 0));
    d->m_highlights = d->m_page->search(text, flags);
    QRectF rec;
    foreach (rec, d->m_highlights) {
        //获取搜索结果附近文字
        QRectF rctext = rec;
        rctext.setX(rctext.x() - 40);
        rctext.setWidth(rctext.width() + 80);
        if (d->m_page) {
            stres.listtext.push_back(d->m_page->text(rctext));
        } else {
            break;
        }
    }
    return stres;
}


void PagePdf::paintEvent(QPaintEvent *event)
{
    Q_D(PagePdf);
    PageBase::paintEvent(event);

    for (int i = 0; i < d_ptr->m_highlights.size(); i++) {
        if (d->m_icurhightlight == i && d->m_bcursearchshow) {
            QPainter qpainter(this);
            d_ptr->m_searchcolor.setAlpha(100);
            qpainter.setBrush( d_ptr->m_searchcolor);
            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));
        } else {
            QPainter qpainter(this);
            d_ptr->m_searchcolor.setAlpha(100);
            qpainter.setBrush(d_ptr->m_searchcolor);
            QPen qpen(d_ptr->m_pencolor);
            qpainter.setPen(qpen);
            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));
        }
    }
}

void PagePdf::removeAnnotation(Poppler::Annotation *annotation)
{
    Q_D(PagePdf);
    d->m_page->removeAnnotation(annotation);
}

void PagePdf::setPage(Poppler::Page *page, int pageno)
{
    Q_D(PagePdf);
    d->setPage(page, pageno);
}

bool PagePdf::getSlideImage(QImage &image, double &width, double &height)
{
    Q_D(PagePdf);

    return d->getSlideImage(image, width, height);
}

bool PagePdf::getImage(QImage &image, double width, double height)
{
    Q_D(PagePdf);

    //    qDebug() << "devicePixelRatioF:" << devicePixelRatioF();
    return d->getImage(image, width, height);
}

QString PagePdf::addAnnotation(const QColor &color)
{
    Q_D(PagePdf);
    QString uniqueName;
    if (d->paintrects.size() > 0) {
        uniqueName = addHighlightAnnotation(color);
    }
    QImage image;
    getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    slot_RenderFinish(image);
    return  uniqueName;
}

QString PagePdf::addAnnotation(const QColor &color, const QRect &rect)
{
     Q_D(PagePdf);
    QString uniqueName;
    Poppler::Annotation::Style style;
    style.setColor(color);
    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    Poppler::HighlightAnnotation::Quad quad;
    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
    double curwidth = d->m_imagewidth;
    double curheight = d->m_imageheight;
    QRectF boundary;
    boundary.setTopLeft(QPointF(rect.left() / curwidth,
                                   rect.top() / curheight));
    boundary.setTopRight(QPointF(rect.right() / curwidth,
                                    rect.top() / curheight));
    boundary.setBottomLeft(QPointF(rect.left() / curwidth,
                                      rect.bottom() / curheight));
    boundary.setBottomRight(QPointF(rect.right() / curwidth,
                                       rect.bottom() / curheight));
    quad.points[0] = boundary.topLeft();
    quad.points[1] = boundary.topRight();
    quad.points[2] = boundary.bottomRight();
    quad.points[3] = boundary.bottomLeft();

    annotation->setHighlightQuads(QList< Poppler::HighlightAnnotation::Quad >() << quad);
    annotation->setBoundary(boundary);
    uniqueName = PublicFunc::getUuid();
    annotation->setUniqueName(uniqueName);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    d->m_page->addAnnotation(annotation);
    QImage image;
    getImage(image,d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    slot_RenderFinish(image);
    return  uniqueName;
}

QString PagePdf::addHighlightAnnotation(const QColor &color)
{
    Q_D(PagePdf);
    QString uniqueName;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    Poppler::HighlightAnnotation::Quad quad;
    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
    QRectF rec, recboundary;
    double curwidth = d->m_imagewidth;
    double curheight = d->m_imageheight;
    if (d->m_selecttextendword < 1 || d->m_selecttextendword < 0)return "";
    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; ++i) {

        rec = d->m_words.at(i).rect;

        recboundary.setTopLeft(QPointF(rec.left() / curwidth,
                                       rec.top() / curheight));
        recboundary.setTopRight(QPointF(rec.right() / curwidth,
                                        rec.top() / curheight));
        recboundary.setBottomLeft(QPointF(rec.left() / curwidth,
                                          rec.bottom() / curheight));
        recboundary.setBottomRight(QPointF(rec.right() / curwidth,
                                           rec.bottom() / curheight));

        quad.points[0] = recboundary.topLeft();
        quad.points[1] = recboundary.topRight();
        quad.points[2] = recboundary.bottomRight();
        quad.points[3] = recboundary.bottomLeft();
        qlistquad.append(quad);
    }
    annotation->setHighlightQuads(qlistquad);
    uniqueName = PublicFunc::getUuid();
    annotation->setUniqueName(uniqueName);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    d->m_page->addAnnotation(annotation);
    return  uniqueName;
}

QString PagePdf::removeAnnotation(const QPoint &pos)
{
    Q_D(PagePdf);
    double curwidth = d->m_scale * d->m_imagewidth;
    double curheight = d->m_scale * d->m_imageheight;
    QString uniqueName;
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight( quad.points[3]);
                if (d->m_rotate == RotateType_180) {
                    rectbound.setTop(1 - rectbound.top());
                    rectbound.setBottom(1 - rectbound.bottom());
                    rectbound.setLeft(1 - rectbound.left());
                    rectbound.setRight(1 - rectbound.right());
                }
                if (rectbound.contains(ptf)) {
                    uniqueName = annote->uniqueName();
                    removeAnnotation(annote);
                } /*else {
                    qDebug() << "******* not contains";
                }*/
            }
        }
    }
    QImage image;
    getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    slot_RenderFinish(image);
//    clearImage();
//    showImage(d->m_scale, d->m_rotate);
    return uniqueName;
}

void PagePdf::removeAnnotation(const QString &struuid)
{
    Q_D(PagePdf);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    int index = 0;
    foreach (Poppler::Annotation *annote, listannote) {
        /*annote->subType()==Poppler::Annotation::AHighlight&&*/
        if (!struuid.isEmpty() && annote->uniqueName().indexOf(struuid) >= 0) { //必须判断
            removeAnnotation(annote);
            listannote.removeAt(index);
            QImage image;
            getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
            slot_RenderFinish(image);
//            clearImage();
//            showImage(d->m_scale, d->m_rotate);
            break;
        }
        ++index;
    }
    qDeleteAll(listannote);
}

bool PagePdf::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(PagePdf);
    const double scaleX = d->m_scale;
    const double scaleY = d->m_scale;
    double curwidth = d->m_scale * d->m_imagewidth;
    double curheight = d->m_scale * d->m_imageheight;

    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight( quad.points[3]);
               // qDebug() << "########" << quad.points[0];
                if (rectbound.contains(ptf)) {
                    struuid = annote->uniqueName();
                    strtext = annote->contents();
                    qDeleteAll(listannote);
                   // qDebug() << "******* contaions***" << struuid;
                    return true;
                } /*else {
                    qDebug() << "******* not contains";
                }*/
            }
        }
    }
    qDeleteAll(listannote);
    return  false;
}

Poppler::Page *PagePdf::GetPage()
{
    Q_D(PagePdf);
    return d->m_page;
}

PageInterface *PagePdf::getInterFace()
{
    Q_D(PagePdf);
    return d;
}

#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QDebug>
#include <QMutex>

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
    m_highlights.clear();
    stSearchRes stres;
    stres.ipages = d->m_pageno;
    stres.listtext.clear();
    const Poppler::Page::SearchFlags flags((matchCase ? 0 : Poppler::Page::IgnoreCase) | (wholeWords ? Poppler::Page::WholeWords : 0));
    m_highlights = d->m_page->search(text, flags);
    QRectF rec;
    foreach (rec, m_highlights) {
        //获取搜索结果附近文字
        QRectF rctext = rec;
        rctext.setX(rctext.x() - 40);
        rctext.setWidth(rctext.width() + 80);
        stres.listtext.push_back(d->m_page->text(rctext));
    }
    return stres;
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
    return d->getImage(image, width, height);
}

QString PagePdf::addAnnotation(QPoint screenPos)
{
    Q_D(PagePdf);
    QString uniqueName;
    if (d->paintrects.size() > 0) {
//        QRectF rectboundry;
        QList<QRectF> listrectf;
//        foreach (QRect rect, paintrects) {
//            rectboundry = rect;
//            listrectf.append(rectboundry);
//        }
        uniqueName = addHighlightAnnotation(listrectf, Qt::red);
        showImage(d->m_scale, d->m_rotate);
    }
    return  uniqueName;
}

QString PagePdf::addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color)
{
    Q_D(PagePdf);
    QString uniqueName;
//    if (listrect.size() <= 0)return uniqueName;
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
    /**********************************************/
//    QString uniqueName;
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

//        qDebug() << "%%%%%%%%%%%%%%%%%%%"<<m_imagewidth<<m_imageheight<< m_scale;
//        if (m_rotate == RotateType_180) {
//            qDebug() << "%%%%%%%%%%%%%%%%%%%" << m_scale;
//            rec.setTop(curheight - rec.top());
//            rec.setBottom(curheight - rec.bottom());
//            rec.setLeft(curwidth - rec.left());
//            rec.setRight(curwidth - rec.right());
//        }

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
//    return  uniqueName;
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
                } else {
                    qDebug() << "******* not contains";
                }
            }
        }
    }
    showImage(d->m_scale, d->m_rotate);
    return uniqueName;
}

void PagePdf::removeAnnotation(const QString &struuid)
{
    Q_D(PagePdf);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        /*annote->subType()==Poppler::Annotation::AHighlight&&*/
        if (!struuid.isEmpty() && struuid.compare(annote->uniqueName()) == 0) { //必须判断
            removeAnnotation(annote);
            showImage(d->m_scale, d->m_rotate);
        }
    }
    qDeleteAll(listannote);
}

bool PagePdf::annotationClicked(const QPoint &pos, QString &strtext)
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
                qDebug() << "########" << quad.points[0];
                if (rectbound.contains(ptf)) {
                    qDebug() << "******* contaions";
                    return true;
                } else {
                    qDebug() << "******* not contains";
                }
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

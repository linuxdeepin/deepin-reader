#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QDebug>
#include <QPainter>

class PagePdfPrivate: public PageBasePrivate, public  PageInterface
{
//    Q_OBJECT
public:
    PagePdfPrivate(PagePdf *parent): PageBasePrivate(parent)
    {
        m_page = nullptr;
    }

    ~PagePdfPrivate() override
    {
        //        qDebug() << "~PagePdfPrivate";
        if (m_page) {
            delete m_page;
            m_page = nullptr;
        }
    }
    bool getImage(QImage &image, double width, double height) override
    {
//        QMutexLocker locker(&m_mutexlockgetimage);
        int xres = 72.0, yres = 72.0;
        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        if (!m_page)
            return false;
        image = m_page->renderToImage(xres * scalex, yres * scaley);
        return true;
    }
    bool getSlideImage(QImage &image, double &width, double &height) override
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
//        qDebug() << "getSlideImage width:" << width << " height:" << height;
        image = m_page->renderToImage(xres * scale * pixelratiof, yres * scale * pixelratiof);
        image.setDevicePixelRatio(pixelratiof);
        return true;
    }
    bool loadData() override
    {
        loadWords();
        loadLinks();
        return true;
    }

    void setPage(Poppler::Page *page, int pageno)
    {
        m_page = page;
        m_pageno = pageno;
        QSizeF qsize = m_page->pageSizeF();
        m_imagewidth = qsize.width();
        m_imageheight = qsize.height();
    }

    Poppler::Page *m_page;
private:
    bool loadWords()
    {
        if (!m_page) {
            return false;
        }
        QList<Poppler::TextBox *> textList = m_page->textList();
        abstractTextPage(textList);
        qDeleteAll(textList);
        return true;
    }
    bool loadLinks()
    {
        qDeleteAll(m_links);
        m_links.clear();
        if (!m_page) {
            return false;
        }
        QList<Poppler::Link *> qlinks = m_page->links();
        foreach (const Poppler::Link *link, qlinks) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }
            const QRectF boundary = link->linkArea().normalized();
            //        qDebug() << "boundary:" << boundary;

            if (link->linkType() == Poppler::Link::Goto) {
                const Poppler::LinkGoto *linkGoto = static_cast< const Poppler::LinkGoto * >(link);

                int page = linkGoto->destination().pageNumber();
                qreal left = qQNaN();
                qreal top = qQNaN();

                page = page >= 1 ? page : 1;

                if (linkGoto->destination().isChangeLeft()) {
                    left = linkGoto->destination().left();

                    left = left >= 0.0 ? left : 0.0;
                    left = left <= 1.0 ? left : 1.0;
                }

                if (linkGoto->destination().isChangeTop()) {
                    top = linkGoto->destination().top();

                    top = top >= 0.0 ? top : 0.0;
                    top = top <= 1.0 ? top : 1.0;
                }

                if (linkGoto->isExternal()) {
                    //                qDebug() << "isExternal filename:" << linkGoto->fileName() << " page:" << page;
                    m_links.append(new Page::Link(boundary, linkGoto->fileName(), page));
                } else {
                    //                qDebug() << "unExternal left:" << left << " top:" << top << " page:" << page;
                    m_links.append(new Page::Link(boundary, page, left, top));
                }
            } else if (link->linkType() == Poppler::Link::Browse) {
                const Poppler::LinkBrowse *linkBrowse = static_cast< const Poppler::LinkBrowse * >(link);
                const QString url = linkBrowse->url();

                m_links.append(new Page::Link(boundary, url));
            } else if (link->linkType() == Poppler::Link::Execute) {
                const Poppler::LinkExecute *linkExecute = static_cast< const Poppler::LinkExecute * >(link);
                const QString url = linkExecute->fileName();

                m_links.append(new Page::Link(boundary, url, Page::LinkType_Execute));
            }
        }
        qDeleteAll(qlinks);
        return true;
    }
    bool abstractTextPage(const QList<Poppler::TextBox *> &text)
    {
        Poppler::TextBox *next;
        QString s;
        bool addChar;
        m_words.clear();
        foreach (Poppler::TextBox *word, text) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }
            const int qstringCharCount = word->text().length();
            next = word->nextWord();
            // if(next)
            int textBoxChar = 0;
            for (int j = 0; j < qstringCharCount; j++) {
                const QChar c = word->text().at(j);
                if (c.isHighSurrogate()) {
                    s = c;
                    addChar = false;
                } else if (c.isLowSurrogate()) {
                    s += c;
                    addChar = true;
                } else {
                    s = c;
                    addChar = true;
                }

                if (addChar) {
                    QRectF charBBox = word->charBoundingBox(textBoxChar);
                    //                qDebug() << "addChar s:" << s << " charBBox:" << charBBox;
                    stWord sword;
                    sword.s = s;
                    sword.rect = charBBox;
                    m_words.append(sword);
                    textBoxChar++;
                }
            }

            if (word->hasSpaceAfter() && next) {
                QRectF wordBBox = word->boundingBox();
                QRectF nextWordBBox = next->boundingBox();
                //            qDebug() << "hasSpaceAfter wordBBox:" << wordBBox << " nextWordBBox:" << nextWordBBox;
                stWord sword;
                sword.s = QStringLiteral(" ");
                QRectF qrect;
                qrect.setLeft(wordBBox.right());
                qrect.setBottom(wordBBox.bottom());
                qrect.setRight(nextWordBBox.left());
                qrect.setTop(wordBBox.top());
                sword.rect = qrect;
                m_words.append(sword);
            }
        }
        return true;
    }
};

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

void PagePdf::changeAnnotationColor(const QString uuid, const QColor &color)
{
    Q_D(PagePdf);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) {
            if (annote->uniqueName().indexOf(uuid) >= 0 && !uuid.isEmpty()) {
                Poppler::Annotation::Style style = annote->style();
                style.setColor(color);
                annote->setStyle(style);
                QImage image;
                getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
                slot_RenderFinish(image);
                break;
            }
        }
    }
    d->paintrects.clear();
    clearSelectText();
    qDeleteAll(listannote);
}

void PagePdf::paintEvent(QPaintEvent *event)
{
    Q_D(PagePdf);
    PageBase::paintEvent(event);
    QPainter qpainter(this);
    QPen qpen(d_ptr->m_pencolor);
    qpainter.setPen(qpen);
    for (int i = 0; i < d_ptr->m_highlights.size(); i++) {
        if (d->m_icurhightlight == i && d->m_bcursearchshow) {

            d_ptr->m_cursearchfocuscolor.setAlpha(100);
            qpainter.setBrush(d_ptr->m_cursearchfocuscolor);
            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));

//            qpainter.setBrush(d_ptr->m_searchcolor);
//            d_ptr->m_searchcolor.setAlpha(100);
//<<<<<<< HEAD
//            qpainter.setBrush(d_ptr->m_searchcolor);
//=======
//>>>>>>> chore:change search current select text color
//            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));
        } else {
            d_ptr->m_searchcolor.setAlpha(100);
            qpainter.setBrush(d_ptr->m_searchcolor);

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
        d->paintrects.clear();
    }
    clearSelectText();
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
    getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
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
    if (d->m_selecttextendword < 1 || d->m_selecttextendword < 0 || d->m_selecttextendword > d->m_words.size() ||
            d->m_selecttextstartword > d->m_words.size())return "";

    qreal averangeheight = d->m_words.at(d->m_selecttextstartword).rect.height();
    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; ++i) {
        if (d->m_words.at(i).rect.height() < averangeheight && d->m_words.at(i).rect.height() > 0) {
            averangeheight = d->m_words.at(i).rect.height();
        }
    }

    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; ++i) {

        rec = d->m_words.at(i).rect;
        //处理高亮文字矩形大小不一致问题，统一采用最小矩形

        if (rec.height() > averangeheight) {
            rec.setY(rec.y() + (rec.height() - averangeheight) / 2.0);
            rec.setHeight(averangeheight);
        }
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
    int index = 0;
    bool bok = false;
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight(quad.points[3]);
                if (d->m_rotate == RotateType_180) {
                    rectbound.setTop(1 - rectbound.top());
                    rectbound.setBottom(1 - rectbound.bottom());
                    rectbound.setLeft(1 - rectbound.left());
                    rectbound.setRight(1 - rectbound.right());
                }
                if (rectbound.contains(ptf)) {
                    uniqueName = annote->uniqueName();
                    removeAnnotation(annote);
                    listannote.removeAt(index);
                    bok = true;
                    break;
                } /*else {
                    qDebug() << "******* not contains";
                }*/
            }
        }
        ++index;
        if (bok)
            break;
    }
    qDeleteAll(listannote);
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
                rectbound.setBottomRight(quad.points[3]);
                // qDebug() << "########" << quad.points[0];
                if (rectbound.contains(ptf)) {
                    struuid = annote->uniqueName();
                    strtext = annote->contents();
                    qDeleteAll(listannote);
                    //  qDebug() << "******* contaions***" << struuid;
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

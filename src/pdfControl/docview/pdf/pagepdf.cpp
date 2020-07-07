#include "pagepdf.h"
#include "pdfControl/docview/publicfunc.h"
#include "docummentpdf.h"

#include <QDebug>
#include <QPainter>

#include <exception>

class PagePdfPrivate: public PageBasePrivate, public  PageInterface
{
public:
    Q_DECLARE_PUBLIC(PagePdf);
    PagePdfPrivate(PagePdf *parent): PageBasePrivate(parent)
    {
        m_page = nullptr;
    }

    ~PagePdfPrivate() override
    {
        QMutexLocker mutext(&m_imageMutex);
        m_documentpdf = nullptr;
        m_bClosed = true;
        if (m_page) {
            delete m_page;
            m_page = nullptr;
        }
    }
    bool getImage(QImage &image, double width, double height) override
    {
        QMutexLocker mutext(&m_imageMutex);
        if (m_documentpdf) m_documentpdf->imageSemapphore->acquire();
        int xres = 72.0, yres = 72.0;
        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        if (!m_page) {
            if (m_documentpdf) m_documentpdf->imageSemapphore->release();
            return false;
        }

        image = m_page->renderToImage(xres * scalex, yres * scaley);
        if (m_documentpdf) m_documentpdf->imageSemapphore->release();
        return true;
    }
    bool loadData() override
    {
        if (m_bClosed) {
            return false;
        }
        loadWords();
        loadLinks();
        return true;
    }

    void setPage(Poppler::Page *page, int pageno)
    {
        Q_Q(PagePdf);
        if (m_documentpdf == nullptr)
            m_documentpdf = dynamic_cast<DocummentPDF *>(q->parent());
        m_page = page;
        m_pageno = pageno;
        QSizeF qsize = m_page->pageSizeF();
        m_imagewidth = qsize.width();
        m_imageheight = qsize.height();
    }

    DocummentPDF *m_documentpdf = nullptr;
    Poppler::Page *m_page;
    QMutex m_imageMutex;
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
            if (QThread::currentThread()->isInterruptionRequested() || m_bquit) {
                break;
            }
            const QRectF boundary = link->linkArea().normalized();

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
                    m_links.append(new Page::Link(boundary, linkGoto->fileName(), page));
                } else {
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
            if (QThread::currentThread()->isInterruptionRequested() || m_bquit) {
                break;
            }
            const int qstringCharCount = word->text().length();
            next = word->nextWord();

            int textBoxChar = 0;
            for (int j = 0; j < qstringCharCount && !m_bquit; j++) {
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
//                if (m_bquit)
//                    int a = 0;
                if (addChar) {
                    QRectF charBBox = word->charBoundingBox(textBoxChar);
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
//        if (m_bquit)
//            int a = 0;
        return true;
    }
};

QSet<PageBase *> PagePdf::m_pageItems{nullptr};
PagePdf::PagePdf(QWidget *parent)
    : PageBase(new PagePdfPrivate(this), parent)
{
    m_pageItems.insert(this);
    connect(this, SIGNAL(sigRenderFinish(QImage)), this, SLOT(slotRenderFinish(QImage)));
}

PagePdf::~PagePdf()
{
    m_pageItems.remove(this);
}

stSearchRes PagePdf::search(const QString &text, bool matchCase, bool wholeWords)
{
    Q_D(PagePdf);
    d->m_highlights.clear();
    stSearchRes stres;
    stres.ipage = static_cast<unsigned int>(d->m_pageno);
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
            qpainter.setBrush(d_ptr->m_cursearchfocuscolor);
            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));
        } else {
            qpainter.setBrush(d_ptr->m_searchcolor);

            qpainter.drawRect(translateRect(d_ptr->m_highlights[i], d_ptr->m_scale, d_ptr->m_rotate));
        }
    }
}

void PagePdf::setPage(Poppler::Page *page, int pageno)
{
    Q_D(PagePdf);
    d->setPage(page, pageno);
}

bool PagePdf::getImage(QImage &image, double width, double height)
{
    Q_D(PagePdf);
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
    //处理高亮文字矩形大小不一致问题，统一采用最小矩形     之前讨论的结果
    //现在改成按照实际范围添加高亮       2020-5-21    add by  dxh
//    qreal averangeheight = d->m_words.at(d->m_selecttextstartword).rect.height();
//    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; ++i) {
//        if (d->m_words.at(i).rect.height() < averangeheight && d->m_words.at(i).rect.height() > 0) {
//            averangeheight = d->m_words.at(i).rect.height();
//        }
//    }
    double smallestrx = 1.0, largestx = 0.0;
    double smallestry = 1.0, largestry = 0.0;
    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; ++i) {
        rec = d->m_words.at(i).rect;
        //处理高亮文字矩形大小不一致问题，统一采用最小矩形     之前讨论的结果
        //现在改成按照实际范围添加高亮       2020-5-21    add by  dxh
//        if (rec.height() > averangeheight) {
//            rec.setY(rec.y() + (rec.height() - averangeheight) / 2.0);
//            rec.setHeight(averangeheight);
//        }
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
        smallestrx = smallestrx > recboundary.x() ? recboundary.x() : smallestrx;
        smallestry = smallestry > recboundary.y() ? recboundary.y() : smallestry;
        largestx = largestx < recboundary.x() ? recboundary.x() : largestx;
        largestry = largestry < recboundary.y() ? recboundary.y() : largestry;
    }
    //如果需要在其它文档中查看注释必须调用setBoundary(recboundary)设置点击范围
    recboundary.setX(smallestrx);
    recboundary.setY(smallestry);
    recboundary.setWidth(largestx - smallestrx);
    recboundary.setHeight(largestry - smallestrx);
    annotation->setBoundary(recboundary);
    annotation->setHighlightQuads(qlistquad);
    uniqueName = PublicFunc::getUuid();
    annotation->setUniqueName(uniqueName);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    d->m_page->addAnnotation(annotation);
    return  uniqueName;
}

QPointF PagePdf::globalPoint2Iner(const QPoint point)
{
    Q_D(PagePdf);
    double curwidth = d->m_scale * d->m_imagewidth;
    double curheight = d->m_scale * d->m_imageheight;
    QPointF tpoint(0, 0);
    switch (d->m_rotate) {
    case RotateType_90: {
        QPointF tmpP(static_cast<double>(point.y() - y()) - static_cast<double>(this->height() - curwidth) / 2.0,
                     static_cast<double>(point.x() - x()) - static_cast<double>(this->width() - curheight) / 2.0);
        tpoint = QPointF(abs(static_cast<double>(tmpP.x())) / curwidth,
                         abs(curheight - tmpP.y()) / curheight);
        break;
    }
    case RotateType_180: {
        QPointF tmpP(static_cast<double>(point.x() - x()) - static_cast<double>(this->width() - curwidth) / 2.0,
                     static_cast<double>(point.y() - y()) - static_cast<double>(this->height() - curheight) / 2.0);
        tpoint = QPointF((abs(curwidth - tmpP.x())) / curwidth, abs(curheight - tmpP.y()) / curheight);
        break;
    }
    case RotateType_270: {
        QPointF tmpP(static_cast<double>(point.y() - y()) - static_cast<double>(this->height() - curwidth) / 2.0,
                     static_cast<double>(point.x() - x()) - static_cast<double>(this->width() - curheight) / 2.0);
        tpoint = QPointF(abs(curwidth - tmpP.x()) / curwidth, tmpP.y() / curheight);
        break;
    }
    default:
        tpoint = QPointF(abs(static_cast<double>(point.x() - x()) - static_cast<double>(this->width() - curwidth) / 2.0) / curwidth,
                         abs(static_cast<double>(point.y() - y()) - static_cast<double>(this->height() - curheight)) / static_cast<double>(curheight));
        break;
    }

    return  tpoint;
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
            QRectF rectbound;
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[3]);
                rectbound.setBottomRight(quad.points[2]);
                if (d->m_rotate == RotateType_180) {
                    rectbound.setTop(1 - rectbound.top());
                    rectbound.setBottom(1 - rectbound.bottom());
                    rectbound.setLeft(1 - rectbound.left());
                    rectbound.setRight(1 - rectbound.right());
                }
                if (rectbound.contains(ptf)) {
                    uniqueName = annote->uniqueName();
                    d->m_page->removeAnnotation(annote);
                    listannote.removeOne(annote);
                    break;
                }
            }
        } else if (annote->subType() == Poppler::Annotation::AText) {
            QRectF rectbound;
            rectbound = annote->boundary();
            if (rectbound.contains(ptf)) {
                uniqueName = annote->uniqueName();
                listannote.removeOne(annote);
                d->m_page->removeAnnotation(annote);
                break;
            }
        }
    }
    qDeleteAll(listannote);

    QImage image;
    getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    slot_RenderFinish(image);
    return uniqueName;
}

void PagePdf::removeAnnotation(const QString &struuid)
{
    Q_D(PagePdf);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    int index = 0;
    try {
        foreach (Poppler::Annotation *annote, listannote) {
            if (!struuid.isEmpty() && annote->uniqueName().indexOf(struuid) >= 0) { //必须判断
                d->m_page->removeAnnotation(annote);
                listannote.removeAt(index);
                QImage image;
                getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
                slot_RenderFinish(image);
                break;
            }
            ++index;
        }
        qDeleteAll(listannote);
    } catch (QString exception) {
        qInfo() << "        error:" << exception;
        qDeleteAll(listannote);
    }
}

bool PagePdf::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(PagePdf);

    int ret = false;
    QPointF tpos = globalPoint2Iner(pos);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[3]);
                rectbound.setBottomRight(quad.points[2]);
                if (rectbound.contains(tpos)) {
                    struuid = annote->uniqueName();
                    strtext = annote->contents();
                    ret = true;
                }
            }
        }
    }
    qDeleteAll(listannote);
    return  ret;
}

bool PagePdf::iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(PagePdf);
    bool bclicked = false;
    QPointF tpos = globalPoint2Iner(pos);
    QList<Poppler::Annotation *> listannote = d->m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AText) { //必须判断
            if (annote->boundary().contains(tpos)) {
                strtext = annote->contents();
                struuid = annote->uniqueName();
                bclicked = true;
                break;
            }
        }
    }
    qDeleteAll(listannote);
    return  bclicked;
}

QString PagePdf::addTextAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type)
{
    Q_D(PagePdf);
    QString strtype;
    switch (type) {
    case TextAnnoteType_Note:
        strtype = QString("Note");
        break;
    case TextAnnoteType_Comment:
        strtype = QString("Comment");
        break;
    case TextAnnoteType_Help:
        strtype = QString("Help");
        break;
    case TextAnnoteType_Insert:
        strtype = QString("Insert");
        break;
    case TextAnnoteType_Key:
        strtype = QString("Key");
        break;
    case TextAnnoteType_NewParagraph:
        strtype = QString("NewParagraph");
        break;
    case TextAnnoteType_Paragraph:
        strtype = QString("Paragraph");
        break;
    }
    QString uuid;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::Linked);

    double x1 = pos.x() / (d->m_imagewidth * d->m_scale);
    double y1 = pos.y() / (d->m_imageheight * d->m_scale);
    double width = ICONANNOTE_WIDTH / d->m_imagewidth;
    double height = ICONANNOTE_WIDTH / d->m_imageheight;
    QRectF boundary;
    boundary.setX(x1 - width / 2.0);
    boundary.setY(y1 - height / 2.0);
    boundary.setWidth(width);
    boundary.setHeight(height);

    uuid = PublicFunc::getUuid();
    annotation->setBoundary(boundary);
    annotation->setTextIcon(strtype);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    annotation->setUniqueName(uuid);
    annotation->setFlags(annotation->flags() | Poppler::Annotation::FixedRotation);
    d->m_page->addAnnotation(annotation);

    delete annotation;

    QImage image;
    getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    slot_RenderFinish(image);
    return uuid;
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

void PagePdf::deletePage()
{
    Q_D(PagePdf);
    QMutexLocker mutext(&d->m_imageMutex);
    if (nullptr != d->m_page) {
        delete d->m_page;
        d->m_page = nullptr;
    }
}

void PagePdf::freshPage(Poppler::Page *page)
{
    Q_D(PagePdf);
    d->m_page = page;
}

bool PagePdf::getrectimage(QImage &image, double destwidth, double scalebase, double magnifierscale, QPoint &pt)
{
    Q_D(PagePdf);
    QPointF ptimage = pt;
    getImagePoint(ptimage);
    ptimage.setX(ptimage.x()*magnifierscale * d->m_scale * devicePixelRatioF());
    ptimage.setY(ptimage.y()*magnifierscale * d->m_scale * devicePixelRatioF());
    int xres = 72.0, yres = 72.0;
    if (!d->m_page)
        return false;
    image = d->m_page->renderToImage(xres * scalebase, yres * scalebase,
                                     static_cast<int>(ptimage.x() - destwidth / 2),
                                     static_cast<int>(ptimage.y() - destwidth / 2),
                                     static_cast<int>(destwidth), static_cast<int>(destwidth));
    return true;
}

bool PagePdf::renderImage(double scale, RotateType_EM rotate)
{
    Q_D(PagePdf);
    d->m_scale = scale;
    d->m_rotate = rotate;
    QImage image;

    if (!d->m_bActive)
        return false;

    if (d->getImage(image, d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof)) {
        emit sigRenderFinish(image);
        return true;
    }

    return false;
}

QImage PagePdf::thumbnail()
{
    Q_D(PagePdf);
    return d->m_page->thumbnail();
}

bool PagePdf::existIns(PageBase *item)
{
    return m_pageItems.contains(item);
}

#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QDebug>
#include <QMutex>
#include <QPainter>
#include <QRectF>
ThreadRenderImage::ThreadRenderImage()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_page = nullptr;
    restart = false;
    m_width = 0;
    m_height = 0;
}

void ThreadRenderImage::setRestart()
{
    restart = true;
}

void ThreadRenderImage::setPage(Poppler::Page *page, double xres, double yres, double width, double height)
{
    m_page = page;
    m_width = width;
    m_height = height;
    m_xres = xres;
    m_yres = yres;
}

void ThreadRenderImage::run()
{
    if (!m_page)
        return;
    restart = true;
    while (restart) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        restart = false;
        if (m_width > 0 && m_height > 0) {
            QImage image = m_page->renderToImage(m_xres, m_yres, -1, -1, m_width, m_height);
            emit signal_RenderFinish(image);
        }
    }
}

PagePdf::PagePdf(QWidget *parent)
    : PageBase(parent),
      m_page(nullptr)
{
    connect(&threadreander, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));
}
PagePdf::~PagePdf()
{
    //    for (int i = 0; i < m_links.size(); i++) {
    //        delete m_links.at(i);
    //    }
    if (threadreander.isRunning()) {
        threadreander.requestInterruption();
        threadreander.quit();
        threadreander.wait();
    }
    delete m_page;
    m_page = nullptr;
}

void PagePdf::clearThread()
{
    if (threadreander.isRunning()) {
        threadreander.requestInterruption();
        //        threadreander.quit();
        threadreander.wait();
    }
}

stSearchRes PagePdf::search(const QString &text, bool matchCase, bool wholeWords)const
{
    m_highlights.clear();
    stSearchRes stres;
    stres.ipage=m_pageno;
    stres.listtext.clear();
    const Poppler::Page::SearchFlags flags((matchCase ? 0 : Poppler::Page::IgnoreCase) | (wholeWords ? Poppler::Page::WholeWords : 0));
    m_highlights = m_page->search(text, flags);
    QRectF rec;
    foreach (rec, m_highlights) {
        //获取搜索结果附近文字
        QRectF rctext = rec;
        rctext.setX(rctext.x() - 40);
        rctext.setWidth(rctext.width() + 80);
        stres.listtext.push_back(m_page->text(rctext));
    }
    return stres;
}

void PagePdf::paintEvent(QPaintEvent *event)
{
    PageBase::paintEvent(event);

    for (int i = 0; i < m_highlights.size(); i++) {     
        if(m_icurhightlight==i&&m_bcursearchshow)
        {
            QPainter qpainter(this);
            QColor color(Qt::yellow);
            color.setAlpha(100);
            qpainter.setBrush(/*m_searchcolor*/color);
            qpainter.drawRect(translateRect(m_highlights[i],m_scale,m_rotate));
        }
        else {
            QPainter qpainter(this);
            qpainter.setBrush(m_searchcolor);
            QPen qpen(m_pencolor);
            qpainter.setPen(qpen);
            qpainter.drawRect(translateRect(m_highlights[i],m_scale,m_rotate));      
        }
    }
}

void PagePdf::removeAnnotation(Poppler::Annotation *annotation)
{
    m_page->removeAnnotation(annotation);
}

void PagePdf::setPage(Poppler::Page *page, int pageno)
{
    //    qDebug() << "----setpage";
    m_page = page;
    m_pageno = pageno;
    QSizeF qsize = m_page->pageSizeF();
    m_imagewidth = qsize.width();
    m_imageheight = qsize.height();
    //    qDebug() << "----setpage m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight;
}

void PagePdf::slot_RenderFinish(QImage image)
{
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
}

bool PagePdf::showImage(double scale, RotateType_EM rotate)
{
    if (!m_page)
        return false;
    int xres = 72.0, yres = 72.0;
    m_scale = scale;
    m_rotate = rotate;
    threadreander.setPage(m_page, xres * m_scale, yres * m_scale, m_imagewidth * m_scale, m_imageheight * m_scale);
    if (!threadreander.isRunning()) {
        threadreander.start();
    } else {
        threadreander.setRestart();
    }
    //    QImage image = m_page->renderToImage(xres * m_scale, yres * m_scale, -1, -1, m_imagewidth * m_scale, m_imageheight * m_scale);
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
    return true;
}

bool PagePdf::getSlideImage(QImage &image, double &width, double &height)
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
    image = m_page->renderToImage(xres * scale, yres * scale, -1, -1, width, height);
    return true;
}

bool PagePdf::getImage(QImage &image, double width, double height)
{
    int xres = 72.0, yres = 72.0;
    double scalex = width / m_imagewidth;
    double scaley = height / m_imageheight;
    if (!m_page)
        return false;
    image = m_page->renderToImage(xres * scalex, yres * scaley, -1, -1, width, height);
    return true;
}

QString PagePdf::addAnnotation(QPoint screenPos)
{
    QString uniqueName;
    if (paintrects.size() > 0) {
        //        QRectF rectboundry;
        QList<QRectF> listrectf;
        //        foreach (QRect rect, paintrects) {
        //            rectboundry = rect;
        //            listrectf.append(rectboundry);
        //        }
        uniqueName = addHighlightAnnotation(listrectf, Qt::red);
        showImage(m_scale, m_rotate);
    }
    return  uniqueName;
}

QString PagePdf::addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color)
{
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
    double curwidth = m_imagewidth;
    double curheight = m_imageheight;
    if (m_selecttextendword < 1 || m_selecttextendword < 0)return "";
    for (int i = m_selecttextstartword; i <= m_selecttextendword; ++i) {

        rec = m_words.at(i).rect;

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
    m_page->addAnnotation(annotation);
    return  uniqueName;
}

QString PagePdf::removeAnnotation(const QPoint &pos)
{
    double curwidth = m_scale * m_imagewidth;
    double curheight = m_scale * m_imageheight;
    QString uniqueName;
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
    QList<Poppler::Annotation *> listannote = m_page->annotations();
    QList<Poppler::Annotation *> listdel;
    foreach (Poppler::Annotation *annote, listannote) {
        if (annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
            QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
            bool bdel = true;
            foreach (Poppler::HighlightAnnotation::Quad quad, listquad) {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight( quad.points[3]);
                if (m_rotate == RotateType_180) {
                    rectbound.setTop(1 - rectbound.top());
                    rectbound.setBottom(1 - rectbound.bottom());
                    rectbound.setLeft(1 - rectbound.left());
                    rectbound.setRight(1 - rectbound.right());
                }
                if (rectbound.contains(ptf)) {
                    uniqueName = annote->uniqueName();
                    removeAnnotation(annote);
                    bdel = false;
                } else {
                    qDebug() << "******* not contains";
                }
            }
            if (bdel) {
                listdel.append(annote);
            }
        } else {
            if (annote) {
                listdel.append(annote);
            }
        }
    }
    qDeleteAll(listdel);
    showImage(m_scale, m_rotate);
    return uniqueName;
}

void PagePdf::removeAnnotation(const QString &struuid)
{
    QList<Poppler::Annotation *> listannote = m_page->annotations();
    foreach (Poppler::Annotation *annote, listannote) {
        /*annote->subType()==Poppler::Annotation::AHighlight&&*/
        if (!struuid.isEmpty() && struuid.compare(annote->uniqueName()) == 0) { //必须判断
            removeAnnotation(annote);
            showImage(m_scale, m_rotate);
        } else {
            delete  annote;
        }
    }
}

bool PagePdf::annotationClicked(const QPoint &pos, QString &strtext)
{
    if (!m_page) {
        qDebug() << ")))))))))))))))))))";
        return false;
    }
    const double scaleX = m_scale;
    const double scaleY = m_scale;
    double curwidth = m_scale * m_imagewidth;
    double curheight = m_scale * m_imageheight;

    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth, (pos.y() - y() - (height() - curheight)) / curheight);
    QList<Poppler::Annotation *> listannote = m_page->annotations();
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
                    qDeleteAll(listannote);
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
    return m_page;
}
bool PagePdf::abstractTextPage(const QList<Poppler::TextBox *> &text)
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
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }
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

bool PagePdf::loadWords()
{
    if (!m_page) {
        return false;
    }
    QList<Poppler::TextBox *> textList = m_page->textList();
    abstractTextPage(textList);
    qDeleteAll(textList);
    return true;
}

bool PagePdf::loadLinks()
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

        delete link;
    }
    return true;
}

#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QPainter>
#include <QDebug>

ThreadLoadMagnifierCache::ThreadLoadMagnifierCache()
{
//    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_page = nullptr;
    restart = false;
    m_width = 0;
    m_height = 0;
}

void ThreadLoadMagnifierCache::setRestart()
{
    restart = true;
}

void ThreadLoadMagnifierCache::setPage(PagePdf *page, double width, double height)
{
    m_page = page;
    m_width = width;
    m_height = height;
}

void ThreadLoadMagnifierCache::run()
{
    if (!m_page)
        return;
    restart = true;
    while (restart) {
        restart = false;
        if (m_width > 0 && m_height > 0) {
            m_page->loadMagnifierPixmapCache(m_width, m_height);
        }
    }
}


PagePdf::PagePdf(QWidget *parent)
    : PageBase(parent),
      m_imagewidth(0.01),
      m_imageheight(0.01),
      m_page(nullptr),
      m_paintercolor(QColor(72, 118, 255, 100)),
      m_pencolor (QColor(72, 118, 255, 0)),
      m_penwidth(0)
{

    //    setFrameShape (QFrame::Box);
    //    setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);");
    m_links.clear();
}

void PagePdf::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter qpainter(this);
    qpainter.setBrush(m_paintercolor);
    QPen qpen(m_pencolor, m_penwidth);
    qpainter.setPen(qpen);
    for (int i = 0; i < paintrects.size(); i++) {
        qpainter.drawRect(paintrects[i]);
    }
}

void PagePdf::removeAnnotation(Poppler::Annotation *annotation)
{
    m_page->removeAnnotation(annotation);
}

void PagePdf::clearPageTextSelections()
{
    if (paintrects.size() > 0) {
        paintrects.clear();
        update();
    }
}

void PagePdf::setPage(Poppler::Page *page)
{
    m_page = page;
    m_imagewidth = m_page->pageSizeF().width();
    m_imageheight = m_page->pageSizeF().height();
}

void PagePdf::setScaleAndRotate(double scale, RotateType_EM rotate)
{
    m_scale = scale;
    m_rotate = rotate;
    switch (rotate) {
    case RotateType_90:
        setMaximumSize(QSize(m_imageheight * scale, m_imagewidth * scale));
        setMinimumSize(QSize(m_imageheight * scale, m_imagewidth * scale));
        break;
    case RotateType_180:
        setMaximumSize(QSize(m_imagewidth * scale, m_imageheight * scale));
        setMinimumSize(QSize(m_imagewidth * scale, m_imageheight * scale));
        break;
    case RotateType_270:
        setMaximumSize(QSize(m_imageheight * scale, m_imagewidth * scale));
        setMinimumSize(QSize(m_imageheight * scale, m_imagewidth * scale));
        break;
    default:
        setMaximumSize(QSize(m_imagewidth * scale, m_imageheight * scale));
        setMinimumSize(QSize(m_imagewidth * scale, m_imageheight * scale));
        break;
    }
    update();
}

bool PagePdf::showImage(double scale, RotateType_EM rotate)
{
    if (!m_page)
        return false;
    int xres = 72.0, yres = 72.0;
    m_scale = scale;
    m_rotate = rotate;
    QImage image = m_page->renderToImage(xres * m_scale, yres * m_scale, m_imagewidth * m_scale, m_imageheight * m_scale);
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

bool PagePdf::getImage(QImage &image, double width, double height)
{
    if (!m_page)
        return false;
    int xres = 72.0, yres = 72.0;
    double scalex = width / m_imagewidth;
    double scaley = height / m_imageheight;
    image = m_page->renderToImage(xres * scalex, yres * scaley, width, height);
    return true;
}

bool PagePdf::clearMagnifierPixmap()
{
    m_magnifierpixmap = QPixmap();
    return true;
}

QString PagePdf::addAnnotation(QPoint screenPos)
{
    QString uniqueName;
    if(paintrects.size()>0)
    {
        QRectF rectboundry;
        QList<QRectF> listrectf;
        foreach(QRect rect,paintrects)
        {
            rectboundry=rect;
            listrectf.append(rectboundry);
        }
        uniqueName=addHighlightAnnotation(listrectf,Qt::red);
        showImage(m_scale,m_rotate);
    }
    return  uniqueName;
}

void PagePdf::loadMagnifierCacheThreadStart(double width, double height)
{
    if (m_magnifierpixmap.isNull() && !loadmagnifiercachethread.isRunning()) {
        loadmagnifiercachethread.setPage(this, width, height);
        loadmagnifiercachethread.start();
    }
}

void PagePdf::loadMagnifierPixmapCache(double width, double height)
{
    QImage image;
    QPixmap qpixmap;
    if (getImage(image, width, height)) {
        qpixmap = QPixmap::fromImage(image);
        m_magnifierpixmap = qpixmap;
    }
}

bool PagePdf::getMagnifierPixmap(QPixmap &pixmap, QPoint point, int radius, double width, double height)
{
    qDebug() << "getMagnifierPixmap";
//    QImage image;
    QPixmap qpixmap;
    if (!m_magnifierpixmap.isNull()) {
        qpixmap = m_magnifierpixmap;
    } else {
//        if (getImage(image, width, height)) {
//            qpixmap = QPixmap::fromImage(image);
//            m_magnifierpixmap = qpixmap;
//        } else {
//            return false;
//        }
        loadMagnifierCacheThreadStart(width, height);
        return false;
    }
    QPoint qp = point;
    getImagePoint(qp);
    double scalex = width / m_imagewidth;
    double scaley = height / m_imageheight;
    double relx = qp.x() * scalex, rely = qp.y() * scaley;
    if (qp.x() * scalex - radius < 0) {
        relx = radius;
    } else if (qp.x() * scalex > width - radius) {
        relx = width - radius;
    }
    if (qp.y() * scaley - radius < 0) {
        rely = radius;
    } else if (qp.y() * scaley > height - radius) {
        rely = height - radius;
    }
    //    qDebug() << "getMagnifierPixmap scalex:" << scalex << " scaley: " << scaley << " radius: " << radius << " qp: " << qp;
    QPixmap qpixmap1 = qpixmap.copy(relx - radius, rely - radius, radius * 2, radius * 2);
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
        pixmap = qpixmap1;
        return true;
        break;
    }
    pixmap = qpixmap1.transformed(leftmatrix, Qt::SmoothTransformation);
    return true;
}

bool PagePdf::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    m_paintercolor = paintercolor;
    m_pencolor = pencolor;
    m_penwidth = penwidth;
    update();
    return true;
}

bool PagePdf::pageTextSelections(const QPoint start, const QPoint end)
{
    qDebug() << "pageTextSelections start:" << start << " end:" << end;
    //    qDebug() << "pageTextSelections x():" << x() << " y()" << y();
    QPoint startC = QPoint(start.x() - x() - (width() - m_scale * m_imagewidth) / 2, start.y() - y() - (height() - m_scale * m_imageheight) / 2);
    QPoint endC = QPoint(end.x() - x() - (width() - m_scale * m_imagewidth) / 2, end.y() - y() - (height() - m_scale * m_imageheight) / 2);
    //    qDebug() << "startC1:" << startC << " endC1:" << endC;
    switch (m_rotate) {
    case RotateType_90:
        startC = QPoint((start.x() - x() - (width() - m_scale * m_imageheight) / 2), (start.y() - y() - (height() - m_scale * m_imagewidth) / 2));
        startC = QPoint(startC.y(), m_scale * m_imageheight - startC.x());
        endC = QPoint((end.x() - x() - (width() - m_scale * m_imageheight) / 2), (end.y() - y() - (height() - m_scale * m_imagewidth) / 2));
        endC = QPoint(endC.y(), m_scale * m_imageheight - endC.x());
        break;
    case RotateType_180:
        startC = QPoint(m_scale * m_imagewidth - startC.x(), m_scale * m_imageheight - startC.y());
        endC = QPoint(m_scale * m_imagewidth - endC.x(), m_scale * m_imageheight - endC.y());
        break;
    case RotateType_270:
        startC = QPoint((start.x() - x() - (width() - m_scale * m_imageheight) / 2), (start.y() - y() - (height() - m_scale * m_imagewidth) / 2));
        startC = QPoint(m_scale * m_imagewidth - startC.y(), startC.x());
        endC = QPoint((end.x() - x() - (width() - m_scale * m_imageheight) / 2), (end.y() - y() - (height() - m_scale * m_imagewidth) / 2));
        endC = QPoint(m_scale * m_imagewidth - endC.y(), endC.x());
        break;
    default:
        break;
    }
    //    qDebug() << "startC:" << startC << " endC:" << endC;
    QPoint temp;
    if (startC.x() > endC.x()) {
        temp = startC;
        startC = endC;
        endC = temp;
    }

    const QRect start_end = (startC.y() < endC.y())
            ? QRect(startC.x(), startC.y(), endC.x(), endC.y())
            : QRect(startC.x(), endC.y(), endC.x(), startC.y());

    QRectF tmp;
    int startword = 0, stopword = -1;
    //    qDebug() << "page width:" << width() << " height:" << height() << " m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight;
    //    const double scaleX = width() / m_imagewidth;
    //    const double scaleY = height() / m_imageheight;
    const double scaleX = m_scale;
    const double scaleY = m_scale;
    qDebug() << "m_words size:" << m_words.size();
    for (int i = 0; i < m_words.size(); i++) {
        //        qDebug() << "m_words i:" << i << " rect:" << m_words.at(i).rect;
        tmp = m_words.at(i).rect;
        if (startC.x() > (tmp.x() * m_scale) &&
                startC.x() < (tmp.x() * scaleX + tmp.width() * scaleX) &&
                startC.y() > (tmp.y() * scaleY) &&
                startC.y() < (tmp.y() * scaleY + tmp.height() * scaleY)) {
            startword = i;
        }
        if (endC.x() > (tmp.x() * scaleX ) &&
                endC.x() < (tmp.x() * scaleX + tmp.width() * scaleX ) &&
                endC.y() > (tmp.y() * scaleY ) &&
                endC.y() < (tmp.y() * scaleY + tmp.height() * scaleY)) {
            stopword = i;
        }
    }
    qDebug() << " startword:" << startword << " stopword:" << stopword;
    if (-1 == startword && stopword == -1) {
        int i;
        for (i = 0; i < m_words.size(); i++) {
            tmp = m_words.at(i).rect;
            if (start_end.intersects(QRect(tmp.x() * scaleX,
                                           tmp.y() * scaleY, tmp.width() * scaleX,
                                           tmp.height() * scaleY))) {
                //                qDebug() << "break i:" << i;
                break;
            }
        }

        if (i == m_words.size()) {
            return false;
        }
    }
    bool selection_two_start = false;
    if (startword == 0) {
        QRectF rect;
        if (startC.y() <= endC.y()) {
            for (int i = 0; i < m_words.size(); i++) {
                tmp = m_words.at(i).rect;
                rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                             tmp.width() * scaleX, tmp.height() * scaleY);
                if (rect.y() > startC.y() && rect.x() > startC.x()) {
                    startword = i;
                    break;
                }
            }
        } else {
            selection_two_start = true;
            int distance = scaleX + scaleY + 100;
            int count = 0;

            for (int i = 0; i < m_words.size(); i++) {
                tmp = m_words.at(i).rect;
                rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                             tmp.width() * scaleX, tmp.height() * scaleY);

                if ((rect.y() + rect.height()) < startC.y() &&
                        (rect.x() + rect.height()) < startC.x()) {
                    count++;
                    int xdist, ydist;
                    xdist = rect.center().x() - startC.x();
                    ydist = rect.center().y() - startC.y();

                    if (xdist < 0)
                        xdist = -xdist;
                    if (ydist < 0)
                        ydist = -ydist;

                    if ((xdist + ydist) < distance) {
                        distance = xdist + ydist;
                        startword = i;
                    }
                }
            }
        }
    }
    if (stopword == -1) {
        QRectF rect;

        if (startC.y() <= endC.y()) {
            for (int i = m_words.size() - 1; i >= 0; i--) {
                tmp = m_words.at(i).rect;
                rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                             tmp.width() * scaleX, tmp.height() * scaleY);

                if ((rect.y() + rect.height()) < endC.y() && (rect.x() + rect.width()) < endC.x()) {
                    stopword = i;
                    break;
                }
            }
        }

        else {
            int distance = scaleX + scaleY + 100;
            for (int i = m_words.size() - 1; i >= 0; i--) {
                tmp = m_words.at(i).rect;
                rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                             tmp.width() * scaleX, tmp.height() * scaleY);
                if (rect.y() > endC.y() && rect.x() > endC.x()) {
                    int xdist, ydist;
                    xdist = rect.center().x() - endC.x();
                    ydist = rect.center().y() - endC.y();

                    if (xdist < 0)
                        xdist = -xdist;
                    if (ydist < 0)
                        ydist = -ydist;

                    if ((xdist + ydist) < distance) {
                        distance = xdist + ydist;
                        stopword = i;
                    }
                }
            }
        }
    }
    if (-1 == stopword)
        return false;
    if (selection_two_start) {
        if (startword > stopword) {
            startword = startword - 1;
        }
    }
    if (startword > stopword) {
        int im = startword;
        startword = stopword;
        stopword = im;
    }
    paintrects.clear();
    tmp = m_words.at(startword).rect;
    for (int i = startword + 1; i <= stopword; i++) {
        QRectF tmpafter;
        tmpafter = m_words.at(i).rect;
        if ((abs(tmp.y() - tmpafter.y()) < tmp.height() / 5 ||
             abs(tmp.y() + tmp.height() / 2 - tmpafter.y() + tmpafter.height() / 2) <
             tmp.height() / 5) &&
                abs(tmp.x() + tmp.width() - tmpafter.x()) < tmp.width() / 5) {
            if (tmpafter.y() < tmp.y()) {
                tmp.setY(tmpafter.y());
            }
            if (tmpafter.height() > tmp.height()) {
                tmp.setHeight(tmpafter.height());
            }
            tmp.setWidth(tmpafter.x() + tmpafter.width() - tmp.x());
        } else {
            QRect paintrect = QRect(tmp.x() * scaleX + (width() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (height() - m_scale * m_imageheight) / 2, tmp.width() * scaleX,
                                    tmp.height() * scaleY);
            switch (m_rotate) {
            case RotateType_90:
                paintrect = QRect(tmp.x() * scaleX + (height() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (width() - m_scale * m_imageheight) / 2, tmp.width() * scaleX,
                                  tmp.height() * scaleY);
                paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
                break;
            case RotateType_180:
                paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
                break;
            case RotateType_270:
                paintrect = QRect(tmp.x() * scaleX + (height() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (width() - m_scale * m_imageheight) / 2, tmp.width() * scaleX,
                                  tmp.height() * scaleY);
                paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
                break;
            default:
                break;
            }
            paintrects.append(paintrect);
            tmp = tmpafter;
        }
    }
    QRect paintrect = QRect(tmp.x() * scaleX + (width() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (height() - m_scale * m_imageheight) / 2, tmp.width() * scaleX, tmp.height() * scaleY);
    switch (m_rotate) {
    case RotateType_90:
        paintrect = QRect(tmp.x() * scaleX + (height() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (width() - m_scale * m_imageheight) / 2, tmp.width() * scaleX,
                          tmp.height() * scaleY);
        paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
        break;
    case RotateType_180:
        paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
        break;
    case RotateType_270:
        paintrect = QRect(tmp.x() * scaleX + (height() - m_scale * m_imagewidth) / 2, tmp.y() * scaleY + (width() - m_scale * m_imageheight) / 2, tmp.width() * scaleX,
                          tmp.height() * scaleY);
        paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
        break;
    default:
        break;
    }
    paintrects.append(paintrect);
    update();
    return true;
}

void PagePdf::getImagePoint(QPoint &point)
{
    const double scaleX = m_scale;
    const double scaleY = m_scale;
    QPoint qp = QPoint((point.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (point.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
    switch (m_rotate) {
    case RotateType_90:
        qp = QPoint((point.x() - x() - (width() - m_scale * m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - m_scale * m_imagewidth) / 2) / scaleY);
        qp = QPoint(qp.y(), m_imageheight - qp.x());
        break;
    case RotateType_180:
        qp = QPoint(m_imagewidth - qp.x(),  m_imageheight - qp.y());
        break;
    case RotateType_270:
        qp = QPoint((point.x() - x() - (width() - m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - m_scale * m_imagewidth) / 2) / scaleY);
        qp = QPoint(m_imagewidth - qp.y(), qp.x());
        break;
    default:
        break;
    }
    point = qp;
}

bool PagePdf::ifMouseMoveOverText(const QPoint point)
{
    QPoint qp = point;
    getImagePoint(qp);
    for (int i = 0; i < m_words.size(); i++) {
        if (qp.x() > m_words.at(i).rect.x() &&
                qp.x() < m_words.at(i).rect.x() + m_words.at(i).rect.width() &&
                qp.y() > m_words.at(i).rect.y() &&
                qp.y() < m_words.at(i).rect.y() + m_words.at(i).rect.height()) {
            qDebug() << "rect:" << m_words.at(i).rect;
            return true;
        }
    }
    return false;
}

void PagePdf::appendWord(stWord word)
{
    m_words.append(word);
}

void PagePdf::setImageWidth(double width)
{
    m_imagewidth = width;
}
void PagePdf::setImageHeight(double height)
{
    m_imageheight = height;
}

QString PagePdf::addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color)
{
    QString uniqueName;
    qDebug() << "*************" << listrect.size();
    if (listrect.size() <= 0)return uniqueName;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    Poppler::HighlightAnnotation::Quad quad;
    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
    QRectF rec, recboundary;
    double curwidth=m_imagewidth*m_scale;
    double curheight=m_imageheight*m_scale;
    foreach (rec, listrect) {
        recboundary.setTopLeft(QPointF(rec.left() / curwidth,
                                       rec.top() / curheight));
        recboundary.setTopRight(QPointF(rec.right() / curwidth,
                                        rec.top() / curheight));
        recboundary.setBottomLeft(QPointF(rec.left() / curwidth,
                                          rec.bottom() / curheight));
        recboundary.setBottomRight(QPointF(rec.right() /curwidth,
                                           rec.bottom() / curheight));
        qDebug() << "**" << rec << "**";
        quad.points[0] = recboundary.topLeft();
        quad.points[1] = recboundary.topRight();
        quad.points[2] = recboundary.bottomRight();
        quad.points[3] = recboundary.bottomLeft();
        qlistquad.append(quad);
    }
    annotation->setHighlightQuads(qlistquad);
    uniqueName=PublicFunc::getUuid();
    annotation->setUniqueName(uniqueName);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    m_page->addAnnotation(annotation);
    return  uniqueName;
}

QString PagePdf::removeAnnotation(const QPoint &pos)
{  
    double curwidth=m_scale * m_imagewidth;
    double curheight=m_scale * m_imageheight;
    QString uniqueName;
    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth,(pos.y() - y() - (height() - curheight))/curheight);
    QList<Poppler::Annotation*> listannote=m_page->annotations();
    foreach(Poppler::Annotation* annote,listannote)
    {
        if(annote->subType()==Poppler::Annotation::AHighlight)//必须判断
        {
            QList<Poppler::HighlightAnnotation::Quad> listquad=static_cast<Poppler::HighlightAnnotation*>(annote)->highlightQuads();
            foreach(Poppler::HighlightAnnotation::Quad quad,listquad)
            {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight( quad.points[3]);
                if(rectbound.contains(ptf))
                {
                    uniqueName=annote->uniqueName();
                    removeAnnotation(annote);
                }
                else {
                    qDebug()<<"******* not contains";
                }
            }
        }
    }
    showImage(m_scale,m_rotate);
    return uniqueName;
}

void PagePdf::removeAnnotation(const QString &struuid)
{
    QList<Poppler::Annotation*> listannote=m_page->annotations();
    foreach(Poppler::Annotation* annote,listannote)
    {
        /*annote->subType()==Poppler::Annotation::AHighlight&&*/
        if(!struuid.isEmpty()&&struuid.compare(annote->uniqueName())==0)//必须判断
        {
            removeAnnotation(annote);
            showImage(m_scale,m_rotate);
        }
    }
}

bool PagePdf::annotationClicked(const QPoint &pos, QString &strtext)
{
    const double scaleX = m_scale;
    const double scaleY = m_scale;
    double curwidth=m_scale * m_imagewidth;
    double curheight=m_scale * m_imageheight;
    // QPoint qp = QPoint((pos.x() - x() - (width() - m_scale * m_imagewidth) / 2) / scaleX, (pos.y() - y() - (height() - m_scale * m_imageheight) / 2) / scaleY);
    QPointF ptf((pos.x() - x() - (width() - curwidth) / 2) / curwidth,(pos.y() - y() - (height() - curheight))/curheight);
    QList<Poppler::Annotation*> listannote=m_page->annotations();
    foreach(Poppler::Annotation* annote,listannote)
    {
        if(annote->subType()==Poppler::Annotation::AHighlight)//必须判断
        {
            QList<Poppler::HighlightAnnotation::Quad> listquad=static_cast<Poppler::HighlightAnnotation*>(annote)->highlightQuads();
            foreach(Poppler::HighlightAnnotation::Quad quad,listquad)
            {
                QRectF rectbound;
                rectbound.setTopLeft(quad.points[0]);
                rectbound.setTopRight(quad.points[1]);
                rectbound.setBottomLeft(quad.points[2]);
                rectbound.setBottomRight( quad.points[3]);
                qDebug()<<"########"<<quad.points[0];
                if(rectbound.contains(ptf))
                {
                    qDebug()<<"******* contaions";
                    return true;
                }
                else {
                    qDebug()<<"******* not contains";
                }
            }
        }
    }
    return  false;
}

bool PagePdf::loadLinks()
{

    if (!m_page) {
        return false;
    }
    for (int i = 0; i < m_links.size(); i++) {
        delete m_links.at(i);
    }
    m_links.clear();

    foreach (const Poppler::Link *link, m_page->links()) {
        const QRectF boundary = link->linkArea().normalized();
        qDebug() << "boundary:" << boundary;

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
                qDebug() << "isExternal filename:" << linkGoto->fileName() << " page:" << page;
                m_links.append(new Page::Link(boundary, linkGoto->fileName(), page));
            } else {
                qDebug() << "unExternal left:" << left << " top:" << top << " page:" << page;
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

Page::Link *PagePdf::ifMouseMoveOverLink(const QPoint point)
{
    QPoint qp = point;
    getImagePoint(qp);
    //    qDebug() << "ifMouseMoveOverLink qp:" << qp;
    for (int i = 0; i < m_links.size(); i++) {
        if (qp.x() > m_links.at(i)->boundary.x()*m_imagewidth &&
                qp.x() < m_links.at(i)->boundary.x()*m_imagewidth + m_links.at(i)->boundary.width()*m_imagewidth &&
                qp.y() > m_links.at(i)->boundary.y()*m_imageheight &&
                qp.y() < m_links.at(i)->boundary.y()*m_imageheight + m_links.at(i)->boundary.height()*m_imageheight) {
            //            qDebug() << "boundary:" << m_links.at(i)->boundary;
            return m_links.at(i);
        }
    }
    return nullptr;
}

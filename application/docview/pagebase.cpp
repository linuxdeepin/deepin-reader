#include "pagebase.h"
#include <QPainter>
#include <QDebug>
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

void ThreadRenderImage::setPage(PageInterface *page, double width, double height)
{
    m_page = page;
    m_width = width;
    m_height = height;
//    m_xres = xres;
//    m_yres = yres;
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
            QImage image;
            if (m_page->getImage(image, m_width, m_height)) {
                emit signal_RenderFinish(image);
            }
        }
    }
}
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

void ThreadLoadMagnifierCache::setPage(PageInterface *page, double width, double height)
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
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        restart = false;
        if (m_width > 0 && m_height > 0) {
            QImage image;
            if (m_page->getImage(image, m_width, m_height)) {
                emit signal_loadMagnifierPixmapCache(image, m_width, m_height);
            }
//            m_page->loadMagnifierPixmapCache(m_width, m_height);
        }
    }
}

PageBase::PageBase(PageBasePrivate *ptr, DWidget *parent)
    : DLabel(parent),
      d_ptr(ptr ? ptr : new PageBasePrivate(this))
{
    Q_D(PageBase);
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
    connect(d, SIGNAL(signal_loadMagnifierPixmapCache(QImage, double, double)), this, SLOT(slot_loadMagnifierPixmapCache(QImage, double, double)));
}

PageBase::~PageBase()
{
//    Q_D(PageBase);
//    qDeleteAll(d->m_links);
//    d->m_links.clear();
}

void PageBase::paintEvent(QPaintEvent *event)
{
    Q_D(PageBase);
    DLabel::paintEvent(event);
    QPainter qpainter(this);
    qpainter.setBrush(d->m_paintercolor);
    QPen qpen(d->m_pencolor, d->m_penwidth);
    qpainter.setPen(qpen);
    for (int i = 0; i < d->paintrects.size(); i++) {
        qpainter.drawRect(d->paintrects[i]);
    }
}

bool PageBase::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    Q_D(PageBase);
    d->m_paintercolor = paintercolor;
    d->m_pencolor = pencolor;
    d->m_penwidth = penwidth;
    update();
    return true;
}

void PageBase::clearPageTextSelections()
{
    Q_D(PageBase);
    if (d->paintrects.size() > 0) {
        d->paintrects.clear();
        d->m_selecttextstartword = -1;
        d->m_selecttextendword = -1;
        update();
    }
}

bool PageBase::pageTextSelections(const QPoint start, const QPoint end)
{
    Q_D(PageBase);
    qDebug() << "pageTextSelections start:" << start << " end:" << end;
    //    qDebug() << "pageTextSelections x():" << x() << " y()" << y();
    QPoint startC = QPoint(start.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2, start.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2);
    QPoint endC = QPoint(end.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2, end.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2);
    //    qDebug() << "startC1:" << startC << " endC1:" << endC;
    switch (d->m_rotate) {
    case RotateType_90:
        startC = QPoint((start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(startC.y(), d->m_scale * d->m_imageheight - startC.x());
        endC = QPoint((end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(endC.y(), d->m_scale * d->m_imageheight - endC.x());
        break;
    case RotateType_180:
        startC = QPoint(d->m_scale * d->m_imagewidth - startC.x(), d->m_scale * d->m_imageheight - startC.y());
        endC = QPoint(d->m_scale * d->m_imagewidth - endC.x(), d->m_scale * d->m_imageheight - endC.y());
        break;
    case RotateType_270:
        startC = QPoint((start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(d->m_scale * d->m_imagewidth - startC.y(), startC.x());
        endC = QPoint((end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(d->m_scale * d->m_imagewidth - endC.y(), endC.x());
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
    const double scaleX = d->m_scale;
    const double scaleY = d->m_scale;
    qDebug() << "m_words size:" << d->m_words.size();
    for (int i = 0; i < d->m_words.size(); i++) {
        //        qDebug() << "m_words i:" << i << " rect:" << m_words.at(i).rect;
        tmp = d->m_words.at(i).rect;
        if (startC.x() > (tmp.x() * d->m_scale) &&
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
        for (i = 0; i < d->m_words.size(); i++) {
            tmp = d->m_words.at(i).rect;
            if (start_end.intersects(QRect(tmp.x() * scaleX,
                                           tmp.y() * scaleY, tmp.width() * scaleX,
                                           tmp.height() * scaleY))) {
                //                qDebug() << "break i:" << i;
                break;
            }
        }

        if (i == d->m_words.size()) {
            return false;
        }
    }
    bool selection_two_start = false;
    if (startword == 0) {
        QRectF rect;
        if (startC.y() <= endC.y()) {
            for (int i = 0; i < d->m_words.size(); i++) {
                tmp = d->m_words.at(i).rect;
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

            for (int i = 0; i < d->m_words.size(); i++) {
                tmp = d->m_words.at(i).rect;
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
            for (int i = d->m_words.size() - 1; i >= 0; i--) {
                tmp = d->m_words.at(i).rect;
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
            for (int i = d->m_words.size() - 1; i >= 0; i--) {
                tmp = d->m_words.at(i).rect;
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
    d->paintrects.clear();
    d->m_selecttextstartword = startword;
    d->m_selecttextendword = stopword;
    tmp = d->m_words.at(startword).rect;
    for (int i = startword + 1; i <= stopword; i++) {
        QRectF tmpafter;
        tmpafter = d->m_words.at(i).rect;
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
            QRect paintrect = QRect(tmp.x() * scaleX + (width() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (height() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX,
                                    tmp.height() * scaleY);
            switch (d->m_rotate) {
            case RotateType_90:
                paintrect = QRect(tmp.x() * scaleX + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX,
                                  tmp.height() * scaleY);
                paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
                break;
            case RotateType_180:
                paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
                break;
            case RotateType_270:
                paintrect = QRect(tmp.x() * scaleX + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX,
                                  tmp.height() * scaleY);
                paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
                break;
            default:
                break;
            }
            d->paintrects.append(paintrect);
            tmp = tmpafter;
        }
    }
    QRect paintrect = QRect(tmp.x() * scaleX + (width() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (height() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX, tmp.height() * scaleY);
    switch (d->m_rotate) {
    case RotateType_90:
        paintrect = QRect(tmp.x() * scaleX + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX,
                          tmp.height() * scaleY);
        paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
        break;
    case RotateType_180:
        paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
        break;
    case RotateType_270:
        paintrect = QRect(tmp.x() * scaleX + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * scaleY + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * scaleX,
                          tmp.height() * scaleY);
        paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
        break;
    default:
        break;
    }
    d->paintrects.append(paintrect);
    update();
    return true;
}

bool PageBase::ifMouseMoveOverText(const QPoint point)
{
    Q_D(PageBase);
    QPoint qp = point;
    getImagePoint(qp);
    for (int i = 0; i < d->m_words.size(); i++) {
        if (qp.x() > d->m_words.at(i).rect.x() &&
                qp.x() < d->m_words.at(i).rect.x() + d->m_words.at(i).rect.width() &&
                qp.y() > d->m_words.at(i).rect.y() &&
                qp.y() < d->m_words.at(i).rect.y() + d->m_words.at(i).rect.height()) {
            return true;
        }
    }
    return false;
}

void PageBase::getImagePoint(QPoint &point)
{
    Q_D(PageBase);
    const double scaleX = d->m_scale;
    const double scaleY = d->m_scale;
    QPoint qp = QPoint((point.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2) / scaleY);
    switch (d->m_rotate) {
    case RotateType_90:
        qp = QPoint((point.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2) / scaleY);
        qp = QPoint(qp.y(), d->m_imageheight - qp.x());
        break;
    case RotateType_180:
        qp = QPoint(d->m_imagewidth - qp.x(),  d->m_imageheight - qp.y());
        break;
    case RotateType_270:
        qp = QPoint((point.x() - x() - (width() - d->m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2) / scaleY);
        qp = QPoint(d->m_imagewidth - qp.y(), qp.x());
        break;
    default:
        break;
    }
    point = qp;
}

bool PageBase::clearMagnifierPixmap()
{
    Q_D(PageBase);
    d->m_magnifierpixmap = QPixmap();
    return true;
}

bool PageBase::getMagnifierPixmap(QPixmap &pixmap, QPoint point, int radius, double width, double height)
{
    Q_D(PageBase);
    qDebug() << "getMagnifierPixmap";
//    QImage image;
    QPixmap qpixmap;
    if (!d->m_magnifierpixmap.isNull()) {
        qpixmap = d->m_magnifierpixmap;
    } else {
        loadMagnifierCacheThreadStart(width, height);
        return false;
    }
    QPoint qp = point;
    getImagePoint(qp);
    double scalex = width / d->m_imagewidth;
    double scaley = height / d->m_imageheight;
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
    switch (d->m_rotate) {
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

void PageBase::loadMagnifierCacheThreadStart(double width, double height)
{
    Q_D(PageBase);
    if ((d->m_magnifierwidth != width || d->m_magnifierheight != height || d->m_magnifierpixmap.isNull()) &&
            !d->loadmagnifiercachethread.isRunning()) {
        d->loadmagnifiercachethread.setPage(getInterFace(), width, height);
        d->loadmagnifiercachethread.start();
    }
}

void PageBase::slot_RenderFinish(QImage image)
{
    Q_D(PageBase);
    QPixmap map = QPixmap::fromImage(image);
    QMatrix leftmatrix;
    switch (d->m_rotate) {
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

void PageBase::slot_loadMagnifierPixmapCache(QImage image, double width, double height)
{
    Q_D(PageBase);
//    QImage image;
    QPixmap qpixmap;
//    if (getImage(image, width, height)) {
    qpixmap = QPixmap::fromImage(image);
    d->m_magnifierpixmap = qpixmap;
    d->m_magnifierwidth = width;
    d->m_magnifierheight = height;
    emit signal_MagnifierPixmapCacheLoaded(d->m_pageno);
//}
}

void PageBase::setScaleAndRotate(double scale, RotateType_EM rotate)
{
    Q_D(PageBase);
    d->m_scale = scale;
    d->m_rotate = rotate;
    switch (rotate) {
    case RotateType_90:
//        resize(m_imageheight * scale, m_imagewidth * scale);
//        setFixedSize(m_imageheight * scale, m_imagewidth * scale);
        setMaximumSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        setMinimumSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        break;
    case RotateType_180:
//        resize(m_imagewidth * scale, m_imageheight * scale);
//        setFixedSize(m_imagewidth * scale, m_imageheight * scale);
        setMaximumSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        setMinimumSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        break;
    case RotateType_270:
//        resize(m_imageheight * scale, m_imagewidth * scale);
//        setFixedSize(m_imageheight * scale, m_imagewidth * scale);
        setMaximumSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        setMinimumSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        break;
    default:
//        resize(m_imagewidth * scale, m_imageheight * scale);
//        setFixedSize(m_imagewidth * scale, m_imageheight * scale);
        setMaximumSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        setMinimumSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        break;
    }
    update();
}

Page::Link *PageBase::ifMouseMoveOverLink(const QPoint point)
{
    Q_D(PageBase);
    QPoint qp = point;
    getImagePoint(qp);
    //    qDebug() << "ifMouseMoveOverLink qp:" << qp;
    for (int i = 0; i < d->m_links.size(); i++) {
        if (qp.x() > d->m_links.at(i)->boundary.x()*d->m_imagewidth &&
                qp.x() < d->m_links.at(i)->boundary.x()*d->m_imagewidth + d->m_links.at(i)->boundary.width()*d->m_imagewidth &&
                qp.y() > d->m_links.at(i)->boundary.y()*d->m_imageheight &&
                qp.y() < d->m_links.at(i)->boundary.y()*d->m_imageheight + d->m_links.at(i)->boundary.height()*d->m_imageheight) {
            //            qDebug() << "boundary:" << m_links.at(i)->boundary;
            return d->m_links.at(i);
        }
    }
    return nullptr;
}

bool PageBase::getSelectTextString(QString &st)
{
    Q_D(PageBase);
    if (d->m_selecttextstartword < 0 || d->m_selecttextendword < 0 || d->m_words.size() < 1 ||
            d->m_words.size() <= d->m_selecttextendword || d->m_words.size() <= d->m_selecttextstartword) {
        return false;
    }
    st = "";
    for (int i = d->m_selecttextstartword; i <= d->m_selecttextendword; i++) {
        st += d->m_words.at(i).s;
    }
    return true;
}

bool PageBase::showImage(double scale, RotateType_EM rotate)
{
    Q_D(PageBase);
    d->m_scale = scale;
    d->m_rotate = rotate;
    d->threadreander.setPage(getInterFace(), d->m_imagewidth * d->m_scale, d->m_imageheight * d->m_scale);
    if (!d->threadreander.isRunning()) {
        d->threadreander.start();
    } else {
        d->threadreander.setRestart();
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

void PageBase::clearThread()
{
    Q_D(PageBase);
    if (d->threadreander.isRunning()) {
        d->threadreander.requestInterruption();
//        threadreander.quit();
        d->threadreander.wait();
    }
    if (d->loadmagnifiercachethread.isRunning()) {
        d->loadmagnifiercachethread.requestInterruption();
//        loadmagnifiercachethread.quit();
        d->loadmagnifiercachethread.wait();
    }
}

#include "pagebase.h"
#include "publicfunc.h"
#include "utils/utils.h"
#include <QPainter>
#include <QThreadPool>
#include <QDebug>
//ThreadRenderImage::ThreadRenderImage()
//{
//    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
//    m_page = nullptr;
//    restart = false;
//    m_width = 0;
//    m_height = 0;
//}

//void ThreadRenderImage::setRestart()
//{
//    restart = true;
//}

//void ThreadRenderImage::setPage(PageInterface *page, double width, double height)
//{
//    qDebug() << "ThreadRenderImage::setPage" << width << height;
//    m_page = page;
//    m_width = width;
//    m_height = height;
//}

//void ThreadRenderImage::run()
//{
//    if (!m_page)
//        return;
//    restart = true;
//    while (restart) {
//        if (QThread::currentThread()->isInterruptionRequested()) {
//            return;
//        }
//        restart = false;
//        if (m_width > 0 && m_height > 0) {
//            QImage image;
//            qDebug() << "ThreadRenderImage getImage ID:" << currentThreadId();
//            if (m_page->getImage(image, m_width, m_height)) {
//                if (QThread::currentThread()->isInterruptionRequested()) {
//                    return;
//                }
////                m_page->loadData();
//                emit signal_RenderFinish(image);
//            } else {
//                qDebug() << "ThreadRenderImage getImage ID:" << currentThreadId() << " fail!";
//            }
//        }
//    }
//}

ThreadRenderImage::ThreadRenderImage()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_page = nullptr;
    restart = false;
    m_width = 0;
    m_height = 0;
    b_running = false;
    setAutoDelete(false);
}

void ThreadRenderImage::setRestart()
{
    restart = true;
}

void ThreadRenderImage::setPage(PageInterface *page, double width, double height)
{
//    qDebug() << "ThreadRenderImage::setPage" << width << height;
    m_page = page;
    m_width = width;
    m_height = height;
}

bool ThreadRenderImage::isRunning()
{
    return b_running;
}

void ThreadRenderImage::setRunningTrue()
{
    b_running = true;
}

void ThreadRenderImage::run()
{
    if (!m_page)
        return;
    restart = true;
    b_running = true;
    while (restart) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            b_running = false;
            return;
        }
        restart = false;
        if (m_width > 0 && m_height > 0) {
            QImage image;
            qDebug() << "ThreadRenderImage getImage width:" << m_width << " height:" << m_height;
            QTime timecost;
            timecost.start();
            if (m_page->getImage(image, m_width, m_height)) {
                qDebug() << "ThreadRenderImage getImage ID:" << QThread::currentThreadId() << " get suc!" << timecost.elapsed();
                if (QThread::currentThread()->isInterruptionRequested()) {
                    b_running = false;
                    return;
                }
                if (restart)
                    continue;
//                m_page->loadData();
                qDebug() << "ThreadRenderImage getImage ID:" << QThread::currentThreadId() << " emit render!";
                emit signal_RenderFinish(image);
            } else {
                qDebug() << "ThreadRenderImage getImage ID:" << QThread::currentThreadId() << " get fail!";
            }
        }
    }
    b_running = false;
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
        }
    }
}

PageBase::PageBase(PageBasePrivate *ptr, DWidget *parent)
    : DLabel(parent),
      d_ptr(ptr ? ptr : new PageBasePrivate(this))
{
    Q_D(PageBase);
    setMouseTracking(true);
    d->pixelratiof = devicePixelRatioF();
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->bookmarkbtn = new BookMarkButton(this);
    d->bookmarkbtn->raise();
    d->m_spinner = new DSpinner(this);
    d->m_spinner->setFixedSize(40, 40);
    d->m_spinner->hide();
    setAlignment(Qt::AlignCenter);
    connect(d, SIGNAL(signal_loadMagnifierPixmapCache(QImage, double, double)), this, SLOT(slot_loadMagnifierPixmapCache(QImage, double, double)));
    connect(d, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));
    connect(this, &PageBase::signal_update, this, [ = ]() {
        this->update();
    });
    connect(d->bookmarkbtn, &BookMarkButton::signal_bookMarkStateChange, this, [ = ](bool state) {
        emit signal_bookMarkStateChange(d->m_pageno, state);
    });
}

PageBase::~PageBase()
{
    clear();
    delete d_ptr;
}

void PageBase::paintEvent(QPaintEvent *event)
{
    Q_D(PageBase);
    DLabel::paintEvent(event);
    d->bookmarkbtn->move(this->width() - d->bookmarkbtn->width() - 20, 0);
    d->m_spinner->move(this->width() / 2, this->height() / 2);
    QPainter qpainter(this);
    // qpainter.drawPixmap(this->rect(), d->m_pixmapshow);
    qpainter.setBrush(d->m_paintercolor);
    QPen qpen(d->m_pencolor, d->m_penwidth);
    qpainter.setPen(qpen);
    for (int i = 0; i < d->paintrects.size(); i++) {
        qpainter.drawRect(d->paintrects[i]);
    }

    foreach (ICONANNOTATION annote, d->m_iconannotationlist) {
        double rx = annote.position.x() * d->m_scale * d->m_imagewidth - ICONANNOTE_WIDTH / 2;
        double ry = annote.position.y() * d->m_scale * d->m_imageheight - ICONANNOTE_HEIGHT / 2;
        QPointF pt(rx, ry);

        QPixmap pixtag(Utils::renderSVG(":/icons/deepin/builtin/ok.svg", QSize(24, 24)));
        qpainter.drawPixmap(pt, pixtag);
    }
}

bool PageBase::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    Q_D(PageBase);
    d->m_paintercolor = paintercolor;
    d->m_pencolor = pencolor;
    d->m_penwidth = penwidth;
    emit signal_update();
    return true;
}

void PageBase::clearPageTextSelections()
{
    Q_D(PageBase);
    if (d->paintrects.size() > 0) {
        d->paintrects.clear();
        d->m_selecttextstartword = -1;
        d->m_selecttextendword = -1;
        emit signal_update();
    }
}

bool PageBase::pageTextSelections(const QPoint start, const QPoint end)
{
    Q_D(PageBase);
    // qDebug() << "pageTextSelections start:" << start << " end:" << end;
    //    qDebug() << "pageTextSelections x():" << x() << " y()" << y();
    QPoint startC = QPoint(start.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2, start.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2);
    QPoint endC = QPoint(end.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2, end.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2);
    //    qDebug() << "startC1:" << startC << " endC1:" << endC;
    switch (d->m_rotate) {
    case RotateType_90:
        startC = QPoint((start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(startC.y(), d->m_scale * d->m_imageheight  - startC.x());
        endC = QPoint((end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(endC.y(), d->m_scale * d->m_imageheight  - endC.x());
        break;
    case RotateType_180:
        startC = QPoint(d->m_scale * d->m_imagewidth  - startC.x(), d->m_scale * d->m_imageheight  - startC.y());
        endC = QPoint(d->m_scale * d->m_imagewidth  - endC.x(), d->m_scale * d->m_imageheight  - endC.y());
        break;
    case RotateType_270:
        startC = QPoint((start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(d->m_scale * d->m_imagewidth  - startC.y(), startC.x());
        endC = QPoint((end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2), (end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(d->m_scale * d->m_imagewidth  - endC.y(), endC.x());
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
                            ? QRect(startC.x(), startC.y(), endC.x() - startC.x(), endC.y() - startC.y())
                            : QRect(startC.x(), endC.y(), endC.x() - startC.x(), startC.y() - endC.y());

    QPoint startD = QPoint(start_end.x(), start_end.y());
    QPoint endD = QPoint(start_end.x() + start_end.width(), start_end.y() + start_end.height());
    QRectF tmp;
    int startword = -1, stopword = -1;
    //    qDebug() << "page width:" << width() << " height:" << height() << " m_imagewidth:" << m_imagewidth << " m_imageheight:" << m_imageheight;
    const double scaleX = d->m_scale ;
    const double scaleY = d->m_scale ;
    // qDebug() << "m_words size:" << d->m_words.size();
    for (int i = 0; i < d->m_words.size(); i++) {
        //        qDebug() << "m_words i:" << i << " rect:" << m_words.at(i).rect;
        tmp = d->m_words.at(i).rect;
        if (startD.x() > (tmp.x() * d->m_scale) &&
                startD.x() < (tmp.x() * scaleX + tmp.width() * scaleX) &&
                startD.y() > (tmp.y() * scaleY) &&
                startD.y() < (tmp.y() * scaleY + tmp.height() * scaleY)) {
            startword = i;
        }
        if (endD.x() > (tmp.x() * scaleX) &&
                endD.x() < (tmp.x() * scaleX + tmp.width() * scaleX) &&
                endD.y() > (tmp.y() * scaleY) &&
                endD.y() < (tmp.y() * scaleY + tmp.height() * scaleY)) {
            stopword = i;
        }
    }
    // qDebug() << " startword:" << startword << " stopword:" << stopword;
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
    if (startword == -1) {
        QRectF rect;
        for (int i = 0; i < d->m_words.size(); i++) {
            tmp = d->m_words.at(i).rect;
            rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                         tmp.width() * scaleX, tmp.height() * scaleY);
            if (rect.y() + rect.height() > startD.y() && rect.x() + rect.width() > startD.x()) {
                startword = i;
                break;
            }
        }
    }
    if (stopword == -1) {
        QRectF rect;

        for (int i = d->m_words.size() - 1; i >= 0; i--) {
            tmp = d->m_words.at(i).rect;
            rect = QRect(tmp.x() * scaleX, tmp.y() * scaleY,
                         tmp.width() * scaleX, tmp.height() * scaleY);

            if (rect.y() < endD.y() && rect.x() < endD.x()) {
                stopword = i;
                break;
            }
        }
    }
    if (stopword < 0 || startword < 0)
        return false;
    if (startword > stopword) {
        int im = startword;
        startword = stopword;
        stopword = im;
    }
//    d->paintrects.clear();
    d->m_selecttextstartword = startword;
    d->m_selecttextendword = stopword;
    setSelectTextRects();
    emit signal_update();
    return true;
}

void PageBase::setSelectTextRects()
{
    Q_D(PageBase);
    d->paintrects.clear();
    int startword = d->m_selecttextstartword;
    int stopword = d->m_selecttextendword;
    if (startword < 0 || stopword < 0 || startword >= d->m_words.size() || stopword >= d->m_words.size()) {
        return;
    }
    QRectF tmp;
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
            QRect paintrect = QRect(tmp.x() * d->m_scale + (width() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (height() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale,
                                    tmp.height() * d->m_scale);
            switch (d->m_rotate) {
            case RotateType_90:
                paintrect = QRect(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale,
                                  tmp.height() * d->m_scale);
                paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
                break;
            case RotateType_180:
                paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
                break;
            case RotateType_270:
                paintrect = QRect(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale,
                                  tmp.height() * d->m_scale);
                paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
                break;
            default:
                break;
            }
            d->paintrects.append(paintrect);
            tmp = tmpafter;
        }
    }
    QRect paintrect = QRect(tmp.x() * d->m_scale + (width() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (height() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale, tmp.height() * d->m_scale);
    switch (d->m_rotate) {
    case RotateType_90:
        paintrect = QRect(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale,
                          tmp.height() * d->m_scale);
        paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
        break;
    case RotateType_180:
        paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
        break;
    case RotateType_270:
        paintrect = QRect(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2, tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2, tmp.width() * d->m_scale,
                          tmp.height() * d->m_scale);
        paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
        break;
    default:
        break;
    }
    d->paintrects.append(paintrect);
}

bool PageBase::ifMouseMoveOverText(const QPoint point)
{
    Q_D(PageBase);
    QPointF qp = point;
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

void PageBase::getImagePoint(QPointF &point)
{
    Q_D(PageBase);
    const double scaleX = d->m_scale ;
    const double scaleY = d->m_scale ;
    QPointF qp = QPointF((point.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2) / scaleY);
    switch (d->m_rotate) {
    case RotateType_90:
        qp = QPointF((point.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2) / scaleY);
        qp = QPointF(qp.y(), d->m_imageheight - qp.x());
        break;
    case RotateType_180:
        qp = QPointF(d->m_imagewidth - qp.x(),  d->m_imageheight - qp.y());
        break;
    case RotateType_270:
        qp = QPointF((point.x() - x() - (width() - d->m_imageheight) / 2) / scaleX, (point.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2) / scaleY);
        qp = QPointF(d->m_imagewidth - qp.y(), qp.x());
        break;
    default:
        break;
    }
    point = qp;
}

void PageBase::clearHighlightRects()
{
    Q_D(PageBase);
    d->m_highlights.clear();
}

bool PageBase::clearMagnifierPixmap()
{
    Q_D(PageBase);
    d->m_magnifierpixmap = QPixmap();
    return true;
}

bool PageBase::getMagnifierPixmap(QPixmap &pixmap, QPointF point, int radius, double width, double height)
{
    Q_D(PageBase);
    QPixmap qpixmap;
    if (!d->m_magnifierpixmap.isNull()) {
        qpixmap = d->m_magnifierpixmap;
    } else {
        loadMagnifierCacheThreadStart(width * d->pixelratiof, height * d->pixelratiof);
        return false;
    }
    QPointF qp = point;
    getImagePoint(qp);
    double scalex = width / d->m_imagewidth;
    double scaley = height / d->m_imageheight;

    double relx = qp.x() * scalex, rely = qp.y() * scaley;
    // qDebug() << __FUNCTION__ << "=========" << qp << relx << rely;
    if (qp.x() * scalex <= 0) {
        relx = radius;
    } else if (relx < 2 * radius && relx > 0) {
        relx = relx + radius;
    } else if (qp.x() * scalex >= width) {
        relx = width;
    }
    if (qp.y() * scaley - radius < 0) {
        rely = radius;
    } else if (qp.y() * scaley > height - radius) {
        rely = height - radius;
    }

    relx *= devicePixelRatioF();
    rely *= devicePixelRatioF();
    QPixmap qpixmap1;
    if (relx - radius <= 0) {
        qpixmap1 = qpixmap.copy(relx - radius, rely - radius, radius, radius * 2);
    } else if (relx - radius > 0 && relx - radius < radius) {
        qpixmap1 = qpixmap.copy(relx - radius, rely - radius, relx, radius * 2);
        // qDebug() << __FUNCTION__ << "&&&&&&&&&&&&" << qpixmap1.size();
    } else {
        qpixmap1 = qpixmap.copy(relx - radius, rely - radius, radius * 2, radius * 2);
    }

//    if (qp.x() * scalex <= 0) {
//        relx = radius;
//    } else if (qp.x() * scalex >= width) {
//        relx = width;
//    }
//    if (qp.y() * scaley - radius < 0) {
//        rely = radius;
//    } else if (qp.y() * scaley > height - radius) {
//        rely = height - radius;
//    }

//    relx *= devicePixelRatioF();
//    rely *= devicePixelRatioF();
//    qpixmap1 = qpixmap.copy(relx - radius, rely - radius, radius * 2, radius * 2);

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
    d->m_spinner->stop();
    d->m_spinner->hide();
    QTime  time;
    time.start();
    qDebug() << "page RenderFinish pagenum:" << d->m_pageno;
    d->havereander = true;
//    double originwidth = image.width(), originheight = image.height();
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
    QPixmap map = QPixmap::fromImage(image);
    map = map.transformed(leftmatrix, Qt::SmoothTransformation);
    map.setDevicePixelRatio(devicePixelRatioF());
    qDebug() << "PageBase::slot_RenderFinish to setPixmap" << map.rect();
    setPixmap(map);

//    d->m_pixmapshow = QPixmap::fromImage(image);
//    d->m_pixmapshow = d->m_pixmapshow.transformed(leftmatrix, Qt::SmoothTransformation);
//    d->m_pixmapshow.setDevicePixelRatio(devicePixelRatioF());
//    qDebug() << "PageBase::slot_RenderFinish to setPixmap" << d->m_pixmapshow.rect();
//    update();
    setSelectTextRects();

    qDebug() << "PageBase::slot_RenderFinish over time elapsed=" << time.elapsed();
}

void PageBase::clearImage()
{
    Q_D(PageBase);
//    qDebug() << "page clearImage pagenum:" << d->m_pageno;
    disconnect(d, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));//防止页面频繁切换，页面有概率会不刷新的问题
    stopThread();
    waitThread();
//    QThreadPool::globalInstance()->waitForDone();
    clear();
//    qDeleteAll(d->m_links);
//    d->m_links.clear();
    d->paintrects.clear();
//    d->m_words.clear();
    d->havereander = false;
    clearMagnifierPixmap();
}

void PageBase::slot_loadMagnifierPixmapCache(QImage image, double width, double height)
{
    Q_D(PageBase);
    QPixmap qpixmap;
    qpixmap = QPixmap::fromImage(image);
    d->m_magnifierpixmap = qpixmap;
    d->m_magnifierpixmap.setDevicePixelRatio(devicePixelRatioF());
    d->m_magnifierwidth = width;
    d->m_magnifierheight = height;
    emit signal_MagnifierPixmapCacheLoaded(d->m_pageno);

}

void PageBase::setScaleAndRotate(double scale, RotateType_EM rotate)
{
    Q_D(PageBase);
    clearImage();
    d->m_scale = scale;
    d->m_rotate = rotate;
    switch (rotate) {
    case RotateType_90:
        setFixedSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        break;
    case RotateType_180:
        setFixedSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        break;
    case RotateType_270:
        setFixedSize(QSize(d->m_imageheight * scale, d->m_imagewidth * scale));
        break;
    default:
        setFixedSize(QSize(d->m_imagewidth * scale, d->m_imageheight * scale));
        break;
    }
    //  emit signal_update();
}

Page::Link *PageBase::ifMouseMoveOverLink(const QPoint point)
{
    Q_D(PageBase);
    QPointF qp = point;
    getImagePoint(qp);

    for (int i = 0; i < d->m_links.size(); i++) {
        if (qp.x() > d->m_links.at(i)->boundary.x()*d->m_imagewidth &&
                qp.x() < d->m_links.at(i)->boundary.x()*d->m_imagewidth + d->m_links.at(i)->boundary.width()*d->m_imagewidth &&
                qp.y() > d->m_links.at(i)->boundary.y()*d->m_imageheight &&
                qp.y() < d->m_links.at(i)->boundary.y()*d->m_imageheight + d->m_links.at(i)->boundary.height()*d->m_imageheight) {
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

void PageBase::clearSelectText()
{
    Q_D(PageBase);
    d->m_selecttextstartword = -1;
    d->m_selecttextendword = -1;
}

void PageBase::selectAllText()
{
    Q_D(PageBase);
    if (d->m_words.size() > 0) {
        d->m_selecttextstartword = 0;
        d->m_selecttextendword = d->m_words.size() - 1;
        setSelectTextRects();
        update();
    }
}

QRectF PageBase::translateRect(QRectF &rect, double scale, RotateType_EM rotate)
{
    Q_D(PageBase);
    //旋转角度逆时针增加
    QRectF newrect;
    switch (rotate) {
    case RotateType_Normal:
    case RotateType_0: {
        newrect.setX(rect.x()*scale);
        newrect.setY(rect.y()*scale);
        newrect.setWidth(rect.width()*scale);
        newrect.setHeight(rect.height()*scale);
        break;
    }
    case RotateType_90: {
        newrect.setX((d_ptr->m_imageheight - rect.y() - rect.height())*scale);
        newrect.setY(rect.x()*scale);
        newrect.setWidth(rect.height()*scale);
        newrect.setHeight(rect.width()*scale);
        break;
    }
    case RotateType_180: {
        newrect.setX((d_ptr->m_imagewidth - rect.x() - rect.width())*scale);
        newrect.setY((d_ptr->m_imageheight - rect.y() - rect.height())*scale);
        newrect.setWidth(rect.width()*scale);
        newrect.setHeight(rect.height()*scale);
        break;
    }
    case RotateType_270: {
        newrect.setX(rect.y()*scale);
        newrect.setY((d_ptr->m_imagewidth - rect.x() - rect.width())*scale);
        newrect.setWidth(rect.height()*scale);
        newrect.setHeight(rect.width()*scale);
        break;
    }
    default:
        break;
    }
    return  newrect;
}

bool PageBase::showImage(double scale, RotateType_EM rotate)
{
    Q_D(PageBase);
    // qDebug() << "PageBase in showImage" << scale << d->m_scale;
    if (((d->m_scale - scale) < EPSINON || (scale - d->m_scale) < EPSINON) && d->m_rotate == rotate && d->havereander) {
        return false;
    }
    d->m_scale = scale;
    d->m_rotate = rotate;
    qDebug() << "PageBase::showImage*****" << d->m_pageno;
    d->threadreander.setPage(getInterFace(), d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
    connect(d, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));
    if (!d->threadreander.isRunning()) {
//        d->threadreander.start();
        d->threadreander.setRunningTrue();
        QThreadPool::globalInstance()->start(&d->threadreander);
    } else {
        d->threadreander.setRestart();
    }
    d->havereander = true;
    d->m_spinner->show();
    d->m_spinner->start();
    return true;
}

void PageBase::stopThread()
{
    Q_D(PageBase);
//    d->threadreander.requestInterruption();
    if (d->loadmagnifiercachethread.isRunning()) {
        d->loadmagnifiercachethread.requestInterruption();
    }
}

void PageBase::waitThread()
{
    Q_D(PageBase);
//    if (d->threadreander.isRunning()) {
//        //        threadreander.quit();
//        d->threadreander.wait();
//    }
    if (d->loadmagnifiercachethread.isRunning()) {
        //        loadmagnifiercachethread.quit();
        d->loadmagnifiercachethread.wait();
    }
    d->m_spinner->stop();
    d->m_spinner->hide();
}

bool PageBase::setBookMarkState(bool state)
{
    Q_D(PageBase);
    d->bookmarkbtn->setClickState(state);
    return true;
}

QString PageBase::addIconAnnotation(const QPoint &pos)
{
    Q_D(PageBase);
    double curwidth = d->m_imagewidth * d->m_scale;
    double curheight = d->m_imageheight * d->m_scale;
    QString uuid("");
    uuid = PublicFunc::getUuid();
    ICONANNOTATION annote;
    annote.position.setX(pos.x() / curwidth);
    annote.position.setY(pos.y() / curheight);
    annote.uuid = uuid;
    d->m_iconannotationlist.push_back(annote);
    update(QRect(pos.x() - 100, pos.y() - 100, 200, 200));
    return  uuid;
}

bool PageBase::iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(PageBase);
    bool bsuccess = false;
    foreach (ICONANNOTATION annote, d->m_iconannotationlist) {
        double rx = annote.position.x() * d->m_scale * d->m_imagewidth;
        double ry = annote.position.y() * d->m_scale * d->m_imageheight;
        QRectF rect(rx - ICONANNOTE_WIDTH / 2, ry - ICONANNOTE_HEIGHT / 2, ICONANNOTE_WIDTH, ICONANNOTE_HEIGHT);
        if (rect.contains(pos.x(), pos.y())) {
            qDebug() << "PageBase::iconAnnotationClicked success ^^^^^^^start" << annote.position ;
            strtext = annote.strnote;
            struuid = annote.uuid;
            bsuccess = true;
            break;
        }
    }
    return bsuccess;
}

void PageBase::moveIconAnnotation(const QString &uuid, const QPoint &pos)
{
    Q_D(PageBase);
    for (int i = 0; i < d->m_iconannotationlist.size(); i++) {
        if (d->m_iconannotationlist.at(i).uuid == uuid) {
            ICONANNOTATION annote = d->m_iconannotationlist.takeAt(i);
            qDebug() << "PageBase::moveIconAnnotation success ^^^^^^^start" << annote.position ;
            annote.position.setX(pos.x() / (d->m_scale * d->m_imagewidth));
            annote.position.setY(pos.y() / (d->m_scale * d->m_imageheight));
            d->m_iconannotationlist.push_back(annote);
            update();
            qDebug() << "PageBase::moveIconAnnotation success ^^^^^^^end" << annote.position ;
            break;
        }
    }
}

bool PageBase::removeIconAnnotation(const QString &uuid)
{
    Q_D(PageBase);
    bool bsuccess = false;
    int index = 0;
    foreach (ICONANNOTATION annote, d->m_iconannotationlist) {
        if (annote.uuid == uuid) {
            bsuccess = true;
            d->m_iconannotationlist.removeAt(index);
            break;
        }
        index++;
    }
    return  bsuccess;
}

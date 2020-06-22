#include "pagebase.h"
#include "publicfunc.h"
#include "utils/utils.h"
#include <QPainter>
#include <QThreadPool>
#include <QDebug>

const double D_MINUS_DIFF = 0.000001;

ThreadRenderImage::ThreadRenderImage()
{
    m_page = nullptr;
    restart = false;
    m_width = 0;
    m_height = 0;
    b_running = false;
    m_bquit = false;
    setAutoDelete(false);
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
}

bool ThreadRenderImage::isRunning()
{
    return b_running;
}

void ThreadRenderImage::setRunningTrue()
{
    b_running = true;
}

void ThreadRenderImage::setQuit()
{
    m_bquit = true;
}

void ThreadRenderImage::run()
{
    if (!m_page)
        return;
    restart = true;
    b_running = true;
    while (restart) {
        if (QThread::currentThread()->isInterruptionRequested() || m_bquit) {
            b_running = false;
            return;
        }
        restart = false;
        if (m_width > 0 && m_height > 0) {
            QImage image;
            QTime timecost;
            timecost.start();
            if (m_page->getImage(image, m_width, m_height)) {
                if (QThread::currentThread()->isInterruptionRequested() || m_bquit) {
//                    qDebug() << "ThreadRenderImage getImage ID:" << QThread::currentThreadId() << " get suc!" << timecost.elapsed() << m_bquit;
                    b_running = false;
                    return;
                }
                if (restart)
                    continue;
                emit signal_RenderFinish(image);
            }
        }
    }
    b_running = false;
}

ThreadLoadMagnifierCache::ThreadLoadMagnifierCache()
{
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
    connect(d->bookmarkbtn, &BookMarkButton::sigClicked, this, [ = ](bool state) {
        emit sigBookMarkButtonClicked(d->m_pageno, state);
    });

//    connect(this, SIGNAL(sigRenderFinish(QImage &)), this, SLOT(slotRenderFinish(QImage &)));
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

    QPoint startC = QPoint(static_cast<int>(start.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2),
                           static_cast<int>(start.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2));
    QPoint endC = QPoint(static_cast<int>(end.x() - x() - (width() - d->m_scale * d->m_imagewidth) / 2),
                         static_cast<int>(end.y() - y() - (height() - d->m_scale * d->m_imageheight) / 2));

    switch (d->m_rotate) {
    case RotateType_90:
        startC = QPoint(static_cast<int>(start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2),
                        static_cast<int>(start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(static_cast<int>(startC.y()), static_cast<int>(d->m_scale * d->m_imageheight  - startC.x()));
        endC = QPoint(static_cast<int>(end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2),
                      static_cast<int>(end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(endC.y(), static_cast<int>(d->m_scale * d->m_imageheight  - endC.x()));
        break;
    case RotateType_180:
        startC = QPoint(static_cast<int>(d->m_scale * d->m_imagewidth  - startC.x()),
                        static_cast<int>(d->m_scale * d->m_imageheight  - startC.y()));
        endC = QPoint(static_cast<int>(d->m_scale * d->m_imagewidth  - endC.x()),
                      static_cast<int>(d->m_scale * d->m_imageheight  - endC.y()));
        break;
    case RotateType_270:
        startC = QPoint(static_cast<int>(start.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2),
                        static_cast<int>(start.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        startC = QPoint(static_cast<int>(d->m_scale * d->m_imagewidth  - startC.y()), startC.x());
        endC = QPoint(static_cast<int>(end.x() - x() - (width() - d->m_scale * d->m_imageheight) / 2),
                      static_cast<int>(end.y() - y() - (height() - d->m_scale * d->m_imagewidth) / 2));
        endC = QPoint(static_cast<int>(d->m_scale * d->m_imagewidth  - endC.y()), endC.x());
        break;
    default:
        break;
    }
    QPoint temp;
    //增加判断条件 （startC.y() > endC.y()）  add by dxh   2020-5-22
    if (startC.y() > endC.y() && startC.x() > endC.x()) {
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
    const double scaleX = d->m_scale ;
    const double scaleY = d->m_scale ;
    for (int i = 0; i < d->m_words.size(); i++) {
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
    if (-1 == startword && stopword == -1) {
        int i;
        for (i = 0; i < d->m_words.size(); i++) {
            tmp = d->m_words.at(i).rect;
            if (start_end.intersects(QRect(static_cast<int>(tmp.x() * scaleX),
                                           static_cast<int>(tmp.y() * scaleY), static_cast<int>(tmp.width() * scaleX),
                                           static_cast<int>(tmp.height() * scaleY)))) {
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
            rect = QRect(static_cast<int>(tmp.x() * scaleX), static_cast<int>(tmp.y() * scaleY),
                         static_cast<int>(tmp.width() * scaleX), static_cast<int>(tmp.height() * scaleY));
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
            rect = QRect(static_cast<int>(tmp.x() * scaleX), static_cast<int>(tmp.y() * scaleY),
                         static_cast<int>(tmp.width() * scaleX), static_cast<int>(tmp.height() * scaleY));

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
            QRect paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (width() - d->m_scale * d->m_imagewidth) / 2),
                                    static_cast<int>(tmp.y() * d->m_scale + (height() - d->m_scale * d->m_imageheight) / 2),
                                    static_cast<int>(tmp.width() * d->m_scale),
                                    static_cast<int>(tmp.height() * d->m_scale));
            switch (d->m_rotate) {
            case RotateType_90:
                paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2),
                                  static_cast<int>(tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2),
                                  static_cast<int>(tmp.width() * d->m_scale),
                                  static_cast<int>(tmp.height() * d->m_scale));
                paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
                break;
            case RotateType_180:
                paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
                break;
            case RotateType_270:
                paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2),
                                  static_cast<int>(tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2),
                                  static_cast<int>(tmp.width() * d->m_scale),
                                  static_cast<int>(tmp.height() * d->m_scale));
                paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
                break;
            default:
                break;
            }
            d->paintrects.append(paintrect);
            tmp = tmpafter;
        }
    }
    QRect paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (width() - d->m_scale * d->m_imagewidth) / 2),
                            static_cast<int>(tmp.y() * d->m_scale + (height() - d->m_scale * d->m_imageheight) / 2),
                            static_cast<int>(tmp.width() * d->m_scale),
                            static_cast<int>(tmp.height() * d->m_scale));
    switch (d->m_rotate) {
    case RotateType_90:
        paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2),
                          static_cast<int>(tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2),
                          static_cast<int>(tmp.width() * d->m_scale),
                          static_cast<int>(tmp.height() * d->m_scale));
        paintrect = QRect(width() - paintrect.y() - paintrect.height(), paintrect.x(), paintrect.height(), paintrect.width());
        break;
    case RotateType_180:
        paintrect = QRect(width() - paintrect.x() - paintrect.width(), height() - paintrect.y() - paintrect.height(), paintrect.width(), paintrect.height());
        break;
    case RotateType_270:
        paintrect = QRect(static_cast<int>(tmp.x() * d->m_scale + (height() - d->m_scale * d->m_imagewidth) / 2),
                          static_cast<int>(tmp.y() * d->m_scale + (width() - d->m_scale * d->m_imageheight) / 2),
                          static_cast<int>(tmp.width() * d->m_scale),
                          static_cast<int>(tmp.height() * d->m_scale));
        paintrect = QRect(paintrect.y(), height() - paintrect.x() - paintrect.width(), paintrect.height(), paintrect.width());
        break;
    default:
        break;
    }
    d->paintrects.append(paintrect);
}

QPointF PageBase::globalPoint2Iner(const QPoint)
{
    return QPointF(0, 0);
}

bool PageBase::ifMouseMoveOverText(const QPoint point)
{
    Q_D(PageBase);
    QPointF qp(point);
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
        qp = QPointF((static_cast<double>(point.x() - x()) - static_cast<double>(width() - d->m_scale * d->m_imageheight) / 2.0) / scaleX,
                     (static_cast<double>(point.y() - y()) - static_cast<double>(height() - d->m_scale * d->m_imagewidth) / 2.0) / scaleY);
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
        qpixmap1 = qpixmap.copy(static_cast<int>(relx - radius), static_cast<int>(rely - radius), radius, radius * 2);
    } else if (relx - radius > 0 && relx - radius < radius) {
        qpixmap1 = qpixmap.copy(static_cast<int>(relx - radius), static_cast<int>(rely - radius), static_cast<int>(relx), radius * 2);
    } else {
        qpixmap1 = qpixmap.copy(static_cast<int>(relx - radius), static_cast<int>(rely - radius), radius * 2, radius * 2);
    }
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
//        break;
    }
    pixmap = qpixmap1.transformed(leftmatrix, Qt::SmoothTransformation);
    return true;
}

void PageBase::loadMagnifierCacheThreadStart(double width, double height)
{
    Q_D(PageBase);
//    if ((d->m_magnifierwidth != width || d->m_magnifierheight != height || d->m_magnifierpixmap.isNull()) &&
    if ((abs(d->m_magnifierwidth - width) > D_MINUS_DIFF || abs(d->m_magnifierheight - height) > D_MINUS_DIFF || d->m_magnifierpixmap.isNull()) &&
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
    d->havereander = true;
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
    setPixmap(map);
    setSelectTextRects();
}

void PageBase::slotRenderFinish(QImage image)
{
    Q_D(PageBase);
    d->m_spinner->stop();
    d->m_spinner->hide();
    d->havereander = true;
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
    setPixmap(map);
    setSelectTextRects();
}

void PageBase::clearImage()
{
    Q_D(PageBase);
    disconnect(d, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));//防止页面频繁切换，页面有概率会不刷新的问题
    stopThread();
    waitThread();
    clear();
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
        setFixedSize(QSize(static_cast<int>(d->m_imageheight * scale), static_cast<int>(d->m_imagewidth * scale)));
        break;
    case RotateType_180:
        setFixedSize(QSize(static_cast<int>(d->m_imagewidth * scale), static_cast<int>(d->m_imageheight * scale)));
        break;
    case RotateType_270:
        setFixedSize(QSize(static_cast<int>(d->m_imageheight * scale), static_cast<int>(d->m_imagewidth * scale)));
        break;
    default:
        setFixedSize(QSize(static_cast<int>(d->m_imagewidth * scale), static_cast<int>(d->m_imageheight * scale)));
        break;
    }
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

QPointF PageBase::translatepoint(QPointF pt, double, RotateType_EM rotate)
{
    QPointF respt;
    switch (rotate) {
    case RotateType_Normal:
    case RotateType_0: {
        respt = pt;
        break;
    }
    case RotateType_90: {
        respt.setX(1 - pt.y());
        respt.setY(pt.x());
        break;
    }
    case RotateType_180: {
        respt.setX(1 - pt.y());
        respt.setY(1 - pt.x());
        break;
    }
    case RotateType_270: {
        respt.setX(pt.y());
        respt.setY(1 - pt.x());
        break;
    }
    default:
        break;
    }
    return  respt;
}

//bool PageBase::renderImage(double scale, RotateType_EM rotate)
//{
//    Q_D(PageBase);
//    if (getInterFace() && ((d->m_scale - scale) < EPSINON || (scale - d->m_scale) < EPSINON) && d->m_rotate == rotate) {
//        return false;
//    }

//    d->m_scale = scale;
//    d->m_rotate = rotate;
//    int imageW = d->m_imagewidth * scale * d->pixelratiof;
//    int imageH = d->m_imageheight * scale * d->pixelratiof;

//    if (imageW > 0 && imageH > 0) {
//        QImage image;
//        if (getInterFace()->getImage(image, imageW, imageH)) {
//            emit sigRenderFinish(image);
//            return true;
//        }
//    }

//    return false;
//}

bool PageBase::showImage(double scale, RotateType_EM rotate)
{
//    Q_D(PageBase);
//    if (((d->m_scale - scale) < EPSINON || (scale - d->m_scale) < EPSINON) && d->m_rotate == rotate && d->havereander) {
//        return false;
//    }
//    d->m_scale = scale;
//    d->m_rotate = rotate;
//    d->threadreander.setPage(getInterFace(), d->m_imagewidth * d->m_scale * d->pixelratiof, d->m_imageheight * d->m_scale * d->pixelratiof);
//    connect(d, SIGNAL(signal_RenderFinish(QImage)), this, SLOT(slot_RenderFinish(QImage)));
//    if (!d->threadreander.isRunning()) {
//        d->threadreander.setRunningTrue();
//        QThreadPool::globalInstance()->start(&d->threadreander);
//    } else {
//        d->threadreander.setRestart();
//    }
//    d->havereander = true;
//    d->m_spinner->show();
//    d->m_spinner->start();
    return true;
}

void PageBase::stopThread()
{
    Q_D(PageBase);
    if (d->loadmagnifiercachethread.isRunning()) {
        d->loadmagnifiercachethread.requestInterruption();
    }
}

void PageBase::quitThread()
{
    Q_D(PageBase);
    if (d->loadmagnifiercachethread.isRunning()) {
        d->loadmagnifiercachethread.quit();
    }
//    if (d->threadreander.isRunning())
//        d->threadreander.setQuit();
    d->m_bquit = true;
}

void PageBase::waitThread()
{
    Q_D(PageBase);
    if (d->loadmagnifiercachethread.isRunning()) {
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







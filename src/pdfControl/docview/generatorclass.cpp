#include "generatorclass.h"
#include "pdfControl/docview/pagebase.h"
#include "docummentbase.h"

MagnifierImageRender::MagnifierImageRender()
{

}

void MagnifierImageRender::getmagnigierimage(PageBase *page, double width, double scalebase, double magnifierscale, QPoint &pt)
{
    brender = true;
    pagebase = page;
    renderwidth = width;
    basescale = scalebase;
    scalemagnifier = magnifierscale;
    mtex.lock();
    pold = pos;
    pos = pt;
    condition.wakeAll();
    if (!this->isRunning())
        start();
    mtex.unlock();
}

void MagnifierImageRender::run()
{
    if (nullptr != pagebase) {
        while (brender) {
            QImage image;
            if (pold != pos) {
                pagebase->getrectimage(image, renderwidth, basescale, scalemagnifier, pos);
                if (!image.isNull())
                    emit signal_imagerenderover(image);
            } else {
                mtex.lock();
                condition.wait(&mtex);
                mtex.unlock();
            }
        }
    }
}

/*
 *
*/

MagnifierWidget::MagnifierWidget(DWidget *parent): DWidget(parent)
{
    m_magnifierringwidth = 10;
    m_magnifierringmapwidth = 14;
    m_magnifierradius = 100;
    m_magnifiermapradius = 109;
    m_magnifierscale = 2;//3;
    m_magnifierpixmap = QPixmap();
    bStartShow = false;
    setMouseTracking(true);
    connect(&renderthread, SIGNAL(signal_imagerenderover(QImage)), this, SLOT(slotupadteimage(QImage)));
}

void MagnifierWidget::setShowState(bool bshow)
{
    bStartShow = bshow;
    if (!bshow)
        renderthread.stoprender();
}

bool MagnifierWidget::showState()
{
    return bStartShow;
}

void MagnifierWidget::showrectimage(PageBase *page, double pagescale, RotateType_EM type, QPoint &pt)
{
    bStartShow = true;
    int imagewidth = m_magnifiermapradius * 2;
    rotatetype = type;
    double basescale = pagescale * devicePixelRatioF() * m_magnifierscale;
    renderthread.getmagnigierimage(page, imagewidth, basescale, m_magnifierscale, pt);
}

void MagnifierWidget::paintEvent(QPaintEvent *event)
{
    //贴图方式实现
    DWidget::paintEvent(event);
    QPainter qpainter(this);
    if (!bStartShow || m_magnifierpixmap.isNull())
        return;
    int radius = m_magnifiermapradius + m_magnifierringmapwidth;
    int bigcirclex = m_magnifierpoint.x() - radius;
    int bigcircley = m_magnifierpoint.y() - radius;
    if (bigcirclex < 0) {
        bigcirclex = 0;
    } else if (bigcirclex > width() - radius * 2) {
        bigcirclex = width() - radius * 2;
    }
    if (bigcircley < 0) {
        bigcircley = 0;
    } else if (bigcircley > height() - radius * 2) {
        bigcircley = height() - radius * 2;
    }
    radius = m_magnifiermapradius;
    int smallcirclex = bigcirclex + m_magnifierringmapwidth;
    int smallcircley = bigcircley + m_magnifierringmapwidth;
    const qreal ratio = qApp->devicePixelRatio();
    QTransform tr;
    tr.translate(smallcirclex * ratio, smallcircley * ratio);
    tr.scale(1.0, 1.0);
    QBrush brush(m_magnifierpixmap);
    brush.setTransform(tr);
    qpainter.setPen(QPen(QColor(255, 255, 255), 0));
    qpainter.setBrush(brush);
    qpainter.drawEllipse(smallcirclex, smallcircley, m_magnifiermapradius * 2, m_magnifiermapradius * 2);
    QPixmap pix(QIcon::fromTheme("dr_maganifier").pixmap(QSize(244, 244)));
    pix.setDevicePixelRatio(ratio);
    pix = pix.scaled(234, 234, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    qpainter.setRenderHints(QPainter::SmoothPixmapTransform);
    qpainter.drawPixmap(bigcirclex, bigcircley, m_magnifiermapradius * 2 + m_magnifierringmapwidth * 2, m_magnifiermapradius * 2 + m_magnifierringmapwidth * 2, pix);
}

void MagnifierWidget::slotupadteimage(QImage image)
{
    QPixmap pix;
    pix = pix.fromImage(image);
    QMatrix leftmatrix;
    switch (rotatetype) {
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
        break;
    }
    m_magnifierpixmap = pix.transformed(leftmatrix, Qt::SmoothTransformation);
    m_magnifierpixmap.setDevicePixelRatio(devicePixelRatioF());
    update();
}

void MagnifierWidget::setPixmap(QPixmap pixmap)
{
    m_magnifierpixmap = pixmap;
}


void MagnifierWidget::setPoint(QPoint point)
{
    m_magnifierpoint = point;
}

int MagnifierWidget::getMagnifierRadius()
{
    return m_magnifiermapradius;
}

double MagnifierWidget::getMagnifierScale()
{
    return m_magnifierscale;
}

int MagnifierWidget::getMagnifierRingWidth()
{
    return m_magnifierringmapwidth;
}

void MagnifierWidget::setMagnifierRadius(int radius)
{
    m_magnifierradius = radius;
}
void MagnifierWidget::setMagnifierScale(double scale)
{
    m_magnifierscale = scale;
}
void MagnifierWidget::setMagnifierRingWidth(int ringWidth)
{
    m_magnifierringwidth = ringWidth;
}

/*
 *
*/

ThreadLoadData::ThreadLoadData()
{
    m_doc = nullptr;
    restart = false;
}

ThreadLoadData::~ThreadLoadData()
{
    restart = false;
//    quit();
    wait();
}

void ThreadLoadData::setDoc(DocummentBase *doc)
{
    m_doc = doc;
}

void ThreadLoadData::setRestart()
{
    restart = true;
}

void ThreadLoadData::run()
{
    if (!m_doc) {
        emit signal_dataLoaded(false);
        return;
    }
    restart = true;
    while (restart) {
        msleep(1);
        restart = false;
        m_doc->loadData();
    }
}






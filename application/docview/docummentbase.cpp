#include "docummentbase.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QScrollBar>

MagnifierWidget::MagnifierWidget(QWidget *parent): QWidget(parent)
{

    m_magnifiercolor = Qt::white;
    m_magnifierringwidth = 10;
    m_magnifierradius = 100;
    m_magnifierscale = 3;
    setMouseTracking(true);
}

void MagnifierWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter qpainter(this);
//    qpainter.save();
    if (m_magnifierpixmap.isNull())
        return;
    qpainter.setBrush(m_magnifiercolor);
    qpainter.setPen(m_magnifiercolor);
    int radius = m_magnifierradius + m_magnifierringwidth;
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
    QPainterPath bigCircle;
    bigCircle.addEllipse(bigcirclex, bigcircley, radius * 2, radius * 2);

    radius = m_magnifierradius;
    int smallcirclex = bigcirclex + m_magnifierringwidth;
    int smallcircley = bigcircley + m_magnifierringwidth;
    QPainterPath smallCircle;
    smallCircle.addEllipse(smallcirclex, smallcircley, radius * 2, radius * 2);

    QPainterPath path = bigCircle - smallCircle;
    qpainter.drawPath(path);

    qpainter.restore();
    QTransform tr;
    tr.translate(smallcirclex, smallcircley);
    tr.scale(1.0, 1.0);
    QBrush brush(m_magnifierpixmap);
    brush.setTransform(tr);
    qpainter.setBrush(brush);
    qpainter.drawEllipse(smallcirclex, smallcircley, m_magnifierradius * 2, m_magnifierradius * 2);


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
    return m_magnifierradius;
}

double MagnifierWidget::getMagnifierScale()
{
    return m_magnifierscale;
}

int MagnifierWidget::getMagnifierRingWidth()
{
    return m_magnifierringwidth;
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
void MagnifierWidget::setMagnifierColor(QColor color)
{
    m_magnifiercolor = color;
}

DocummentBase::DocummentBase(QWidget *parent): QScrollArea(parent)
    , m_bModified(false), m_bslidemodel(false)
{
    m_magnifierwidget = new MagnifierWidget(parent);
    m_slidewidget = new QWidget(parent);
    pslidelabel = new QLabel(m_slidewidget);
    QGridLayout *gridlyout = new QGridLayout(parent);
    parent->setLayout(gridlyout);
    gridlyout->addWidget(this, 0, 0);
    gridlyout->addWidget(m_magnifierwidget, 0, 0);
    gridlyout->addWidget(m_slidewidget, 0, 0);
    m_slidewidget->raise();
    m_slidewidget->hide();
    m_magnifierwidget->raise();
    m_magnifierwidget->hide();
    m_widget.setLayout(&m_vboxLayout);
    m_widget.setMouseTracking(true);
    setMouseTracking(true);

    m_viewmode = ViewMode_SinglePage;
    m_lastmagnifierpagenum = -1;
    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slot_vScrollBarValueChanged(int)));
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slot_hScrollBarValueChanged(int)));
}

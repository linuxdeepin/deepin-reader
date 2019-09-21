#include "docummentbase.h"
#include "publicfunc.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

ThreadLoadDoc::ThreadLoadDoc()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_doc = nullptr;
    restart = false;
}

void ThreadLoadDoc::setRestart()
{
    restart = true;
}

void ThreadLoadDoc::setDoc(DocummentBase *doc)
{
    m_doc = doc;
}

void ThreadLoadDoc::run()
{
    if (!m_doc)
        return;
    restart = true;
    while (restart) {
        restart = false;
        m_doc->loadPages();
    }
}


ThreadLoadWords::ThreadLoadWords()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_doc = nullptr;
    restart = false;
}

void ThreadLoadWords::setDoc(DocummentBase *doc)
{
    m_doc = doc;
}

void ThreadLoadWords::setRestart()
{
    restart = true;
}

void ThreadLoadWords::run()
{
    if (!m_doc)
        return;
    restart = true;
    while (restart) {
        restart = false;
        m_doc->loadWords();
    }
}

MagnifierWidget::MagnifierWidget(DWidget *parent): DWidget(parent)
{

    m_magnifiercolor = Qt::white;
    m_magnifierringwidth = 10;
    m_magnifierradius = 100;
    m_magnifierscale = 3;
    setMouseTracking(true);
}

void MagnifierWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
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

DocummentBase::DocummentBase(DWidget *parent): DScrollArea(parent)
    , m_bModified(false), m_bslidemodel(false), m_slidepageno(-1),
    m_currentpageno(-1),
    m_scale(1),
    m_rotate(RotateType_Normal),
    donotneedreloaddoc(false)
{
    m_currentpageno = 0;
    m_threadloaddoc.setDoc(this);
    m_threadloadwords.setDoc(this);
    setWidgetResizable(true);
    setWidget(&m_widget);
    pblankwidget = new DWidget(this);
    pblankwidget->setMouseTracking(true);
    pblankwidget->hide();
    m_magnifierwidget = new MagnifierWidget(parent);
    m_slidewidget = new DWidget(parent);
    pslidelabel = new DLabel(m_slidewidget);
    pslideanimationlabel = new DLabel(m_slidewidget);
    pslideanimationlabel->setGeometry(-200, -200, 100, 100);
    delete parent->layout();
    QGridLayout *gridlyout = new QGridLayout(parent);
    parent->setLayout(gridlyout);
    gridlyout->addWidget(this, 0, 0);
    gridlyout->addWidget(m_magnifierwidget, 0, 0);
    gridlyout->addWidget(m_slidewidget, 0, 0);
    gridlyout->setMargin(0);
    pslidelabel->lower();
    pslideanimationlabel->lower();
    m_slidewidget->lower();
//    m_slidewidget->raise();
    m_slidewidget->hide();
//    m_magnifierwidget->raise();
    m_magnifierwidget->hide();
    m_widget.setLayout(&m_vboxLayout);
    m_widget.setMouseTracking(true);
    pslidelabel->setMouseTracking(true);
    pslideanimationlabel->setMouseTracking(true);
    setMouseTracking(true);

    m_viewmode = ViewMode_SinglePage;
    m_lastmagnifierpagenum = -1;

    m_slidewidget->setAttribute(Qt::WA_StyledBackground, true);
    m_slidewidget->setStyleSheet("background-color: rgb(0,0,0)");

    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slot_vScrollBarValueChanged(int)));
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slot_hScrollBarValueChanged(int)));
}

DocummentBase::~DocummentBase()
{
    if (m_threadloaddoc.isRunning()) {
        m_threadloaddoc.requestInterruption();
        m_threadloaddoc.quit();
        m_threadloaddoc.wait();
    }
    if (m_threadloadwords.isRunning()) {
        m_threadloadwords.requestInterruption();
        m_threadloadwords.quit();
        m_threadloadwords.wait();
    }
    if (m_magnifierwidget) {
        delete m_magnifierwidget;
        m_magnifierwidget = nullptr;
    }
    if (m_slidewidget) {
        delete m_slidewidget;
        m_slidewidget = nullptr;
    }
}

QPoint DocummentBase::global2RelativePoint(QPoint globalpoint)
{
    int x_offset = 0;
    int y_offset = 0;
    DScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    DScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    QPoint qpoint = QPoint(mapFromGlobal(globalpoint).x() + x_offset,
                           mapFromGlobal(globalpoint).y() + y_offset);
    //    qDebug() << "globalpoint:" << globalpoint << " relativepoint:" << qpoint;
    return qpoint;
}

bool DocummentBase::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!bDocummentExist()) {
        return false;
    }
    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->setSelectTextStyle(paintercolor, pencolor, penwidth);
    }
    return true;
}

bool DocummentBase::mouseSelectText(QPoint start, QPoint stop)
{
    if (!bDocummentExist()) {
        return false;
    }
    QPoint qstart = start;
    QPoint qstop = stop;
    qDebug() << "startpoint:" << start << " stoppoint:" << stop;
    int startpagenum = -1, endpagenum = -1;

    for (int i = 0; i < m_widgets.size(); i++) {
        if (qstop.x() > m_widgets.at(i)->x() &&
                qstop.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qstop.y() > m_widgets.at(i)->y() &&
                qstop.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qstop = QPoint(qstop.x() - m_widgets.at(i)->x(), qstop.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                endpagenum = i;
                break;
            case ViewMode_FacingPage:
                if (qstop.x() > m_pages.at(2 * i)->x() &&
                        qstop.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qstop.y() > m_pages.at(2 * i)->y() &&
                        qstop.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    endpagenum = 2 * i;
                } else {
                    endpagenum = 2 * i + 1;
                    if (endpagenum >= m_pages.size()) {
                        endpagenum = 2 * i;
                        qstop = QPoint(m_pages.at(endpagenum)->width() + m_pages.at(endpagenum)->x(),
                                       m_pages.at(endpagenum)->height() + m_pages.at(endpagenum)->y());
                    }
                }
                break;
            default:
                break;
            }
            break;
        }
    }


    for (int i = 0; i < m_widgets.size(); i++) {
        if (qstart.x() > m_widgets.at(i)->x() &&
                qstart.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qstart.y() > m_widgets.at(i)->y() &&
                qstart.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qstart = QPoint(qstart.x() - m_widgets.at(i)->x(), qstart.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                startpagenum = i;
                break;
            case ViewMode_FacingPage:
                if (qstart.x() > m_pages.at(2 * i)->x() &&
                        qstart.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qstart.y() > m_pages.at(2 * i)->y() &&
                        qstart.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    startpagenum = 2 * i;
                } else {
                    startpagenum = 2 * i + 1;
                    if (startpagenum >= m_pages.size()) {
                        startpagenum = 2 * i;
                        qstart = QPoint(m_pages.at(startpagenum)->width() + m_pages.at(startpagenum)->x(),
                                        m_pages.at(startpagenum)->height() + m_pages.at(startpagenum)->y());
                    }
                }
                break;
            default:
                break;
            }
            break;
        }
    }

    //    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
    if (-1 == startpagenum || -1 == endpagenum)
        return false;
    if (startpagenum > endpagenum) {
        int mi = startpagenum;
        startpagenum = endpagenum;
        endpagenum = mi;
        QPoint mp = qstart;
        qstart = qstop;
        qstop = mp;
    }
    //    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
    bool re = false;
    for (int i = startpagenum; i < endpagenum + 1; i++) {
//        PagePdf *ppdf = (PagePdf *)m_pages.at(i);
        QPoint pfirst = QPoint(m_pages.at(i)->x(), m_pages.at(i)->y());
        QPoint plast = QPoint(m_pages.at(i)->width() + m_pages.at(i)->x(),
                              m_pages.at(i)->height() + m_pages.at(i)->y());
        switch (m_rotate) {
        case RotateType_90:
            pfirst = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(), m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x(),
                           m_pages.at(i)->height() + m_pages.at(i)->y());
            break;
        case RotateType_180:
            pfirst = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(), m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x(), m_pages.at(i)->y());
            break;
        case RotateType_270:
            pfirst = QPoint(m_pages.at(i)->x(), m_pages.at(i)->height() + m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(),
                           m_pages.at(i)->y());
            break;
        default:
            break;
        }
        if (i == startpagenum) {
            if (startpagenum == endpagenum) {
                re = m_pages.at(i)->pageTextSelections(qstart, qstop);
            } else {
                re = m_pages.at(i)->pageTextSelections(qstart,
                                                       plast);
            }
        } else if (i == endpagenum) {
            re = m_pages.at(i)->pageTextSelections(
                     pfirst,
                     qstop);
        } else {
            re = m_pages.at(i)->pageTextSelections(pfirst,
                                                   plast);
        }
    }
    return re;
}

void DocummentBase::mouseSelectTextClear()
{
    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->clearPageTextSelections();
    }
}

bool DocummentBase::mouseBeOverText(QPoint point)
{
    if (!bDocummentExist()) {
        return false;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "mouseBeOverText pagenum:" << pagenum;
    if (-1 != pagenum) {
        return m_pages.at(pagenum)->ifMouseMoveOverText(qpoint);
    }
    return false;
}

int DocummentBase::pointInWhichPage(QPoint &qpoint)
{
    int pagenum = -1;
    for (int i = 0; i < m_widgets.size(); i++) {
        if (qpoint.x() > m_widgets.at(i)->x() &&
                qpoint.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qpoint.y() > m_widgets.at(i)->y() &&
                qpoint.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                pagenum = i;
                //                qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
                break;
            case ViewMode_FacingPage:
                //                qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
                if (qpoint.x() > m_pages.at(2 * i)->x() &&
                        qpoint.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qpoint.y() > m_pages.at(2 * i)->y() &&
                        qpoint.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    pagenum = 2 * i;
                } else {
                    pagenum = 2 * i + 1;
                    if (pagenum >= m_pages.size())
                        return -1;
                }
                break;
            default:
                break;
            }
            break;
        }
    }
    return pagenum;
}

bool DocummentBase::showMagnifier(QPoint point)
{
    if (!bDocummentExist() || !m_magnifierwidget) {
        return false;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    int x_offset = 0;
    int y_offset = 0;
    DScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    DScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    QPoint gpoint = m_magnifierwidget->mapFromGlobal(mapToGlobal(QPoint(point.x() - x_offset, point.y() - y_offset)));
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "showMagnifier pagenum:" << pagenum;
    if (-1 != pagenum) {
        if (pagenum != m_lastmagnifierpagenum && -1 != m_lastmagnifierpagenum) {
            if (pagenum > m_lastmagnifierpagenum && m_lastmagnifierpagenum - 3 > 0) {
                PageBase *ppage = m_pages.at(m_lastmagnifierpagenum - 3);
                ppage->clearMagnifierPixmap();
                if (pagenum - m_lastmagnifierpagenum > 1) {
                    ppage = m_pages.at(m_lastmagnifierpagenum - 2);
                    ppage->clearMagnifierPixmap();
                }
            } else if (pagenum < m_lastmagnifierpagenum && m_lastmagnifierpagenum + 3 < m_pages.size()) {
                PageBase *ppage = m_pages.at(m_lastmagnifierpagenum + 3);
                ppage->clearMagnifierPixmap();
                if ( m_lastmagnifierpagenum - pagenum > 1) {
                    ppage = m_pages.at(m_lastmagnifierpagenum + 2);
                    ppage->clearMagnifierPixmap();
                }
            }
            for (int i = pagenum - 3; i < pagenum + 4; i++) {
                if (i > 0 && i < m_pages.size()) {
                    PageBase *ppage = m_pages.at(i);
                    ppage->loadMagnifierCacheThreadStart(ppage->width() *m_magnifierwidget->getMagnifierScale(), ppage->height() *m_magnifierwidget->getMagnifierScale());
                }
            }
        }
        m_lastmagnifierpagenum = pagenum;
        PageBase *ppage = m_pages.at(pagenum);
        QPixmap pixmap;
        if (ppage ->getMagnifierPixmap(pixmap, qpoint, m_magnifierwidget->getMagnifierRadius(), ppage->width() *m_magnifierwidget->getMagnifierScale(), ppage->height() *m_magnifierwidget->getMagnifierScale())) {
            m_magnifierwidget->setPixmap(pixmap);

            m_magnifierwidget->setPoint(gpoint);
            m_magnifierwidget->show();
            m_magnifierwidget->update();
        }
    } else {
        QPixmap pix(m_magnifierwidget->getMagnifierRadius() * 2, m_magnifierwidget->getMagnifierRadius() * 2);
        pix.fill(Qt::transparent);
        m_magnifierwidget->setPixmap(pix);
        m_magnifierwidget->setPoint(gpoint);
        m_magnifierwidget->show();
        m_magnifierwidget->update();
    }
    int radius = m_magnifierwidget->getMagnifierRadius() - m_magnifierwidget->getMagnifierRingWidth();
    int bigcirclex = gpoint.x() - radius;
    int bigcircley = gpoint.y() - radius;
    if (bigcircley < 0) {

        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + bigcircley);
    } else if (bigcircley > m_magnifierwidget->height() - radius * 2) {
        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + bigcircley - (m_magnifierwidget->height() - radius * 2));
    }
    if (bigcirclex < 0) {
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + bigcirclex);
    } else if (bigcirclex > m_magnifierwidget->width() - radius * 2) {
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + bigcirclex - (m_magnifierwidget->width() - radius * 2));
    }
    return false;
}

bool DocummentBase::pageJump(int pagenum)
{
    if (pagenum < 0 || pagenum > m_pages.size())
        return false;
    if (m_bslidemodel) {
        QImage image;
        double width = m_slidewidget->width(), height = m_slidewidget->height();
        if (!m_pages.at(pagenum)->getSlideImage(image, width, height)) {
            return false;
        }
        if (-1 != m_slidepageno) {
            DLabel *plabel = pslideanimationlabel;
            pslideanimationlabel = pslidelabel;
            pslidelabel = plabel;
        }
        pslidelabel->setGeometry((m_slidewidget->width() - width) / 2, (m_slidewidget->height() - height) / 2, width, height);
        QPixmap map = QPixmap::fromImage(image);
        pslidelabel->setPixmap(map);
        pslidelabel->show();

        if (-1 != m_slidepageno) {
            QPropertyAnimation *animation = new QPropertyAnimation(pslideanimationlabel, "geometry");
            animation->setDuration(500);

            QPropertyAnimation *animation1 = new QPropertyAnimation(pslidelabel, "geometry");
            animation1->setDuration(500);

            if (m_slidepageno > pagenum) {
                qDebug() << "pageJump previous pagenum:" << pagenum;
                animation->setStartValue(pslideanimationlabel->geometry());
                animation->setEndValue(QRect(m_slidewidget->width(), 10, pslideanimationlabel->width(), pslideanimationlabel->height()));

                animation1->setStartValue(QRect(-m_slidewidget->width(), 10, pslidelabel->width(), pslidelabel->height()));
                animation1->setEndValue(pslidelabel->geometry());
            } else {
                qDebug() << "pageJump next pagenum:" << pagenum;
                animation->setStartValue(pslideanimationlabel->geometry());
                animation->setEndValue(QRect(-m_slidewidget->width(), 10, pslideanimationlabel->width(), pslideanimationlabel->height()));

                animation1->setStartValue(QRect(m_slidewidget->width(), 10, pslidelabel->width(), pslidelabel->height()));
                animation1->setEndValue(pslidelabel->geometry());
            }
            QParallelAnimationGroup *group = new QParallelAnimationGroup;
            group->addAnimation(animation);
            group->addAnimation(animation1);

            group->start(QAbstractAnimation::DeleteWhenStopped);
        }

        m_slidepageno = pagenum;
    } else {
        DScrollBar *scrollBar_X = horizontalScrollBar();
        DScrollBar *scrollBar_Y = verticalScrollBar();
        switch (m_viewmode) {
        case ViewMode_SinglePage:
            qDebug() << "-------pagenum:" << pagenum << " x():" << m_widgets.at(pagenum)->x() << " y():" << m_widgets.at(pagenum)->y();
            if (scrollBar_X)
                scrollBar_X->setValue(m_widgets.at(pagenum)->x());
            if (scrollBar_Y)
                scrollBar_Y->setValue(m_widgets.at(pagenum)->y());
            break;
        case ViewMode_FacingPage:
            if (scrollBar_X)
                scrollBar_X->setValue(m_widgets.at(pagenum / 2)->x() + m_pages.at(pagenum)->x());
            if (scrollBar_Y)
                scrollBar_Y->setValue(m_widgets.at(pagenum / 2)->y());
            break;
        default:
            break;
        }
    }
    return true;
}

void DocummentBase::scaleAndShow(double scale, RotateType_EM rotate)
{
    int currpageno = m_currentpageno;
    if (m_pages.size() < 1) {
        return;
    }

    m_scale = scale;
    if (rotate != RotateType_Normal)
        m_rotate = rotate;
    donotneedreloaddoc = true;
    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->setScaleAndRotate(m_scale, m_rotate);
    }

    int rex = m_vboxLayout.margin(), rey = m_vboxLayout.margin();

    switch (m_viewmode) {
    case ViewMode_SinglePage:
        for (int i = 0; i < m_widgets.size(); i++) {
            m_widgets.at(i)->setGeometry(rex, rey, m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->width(), m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->height());
            rey += m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->height() + m_vboxLayout.spacing();
        }
        break;
    case ViewMode_FacingPage:
        for (int i = 0; i < m_widgets.size() / 2; i++) {
            int reheight = 0;
            if (m_pages.at(i * 2)->height() < m_pages.at(i * 2 + 1)->height()) {
                reheight = m_pages.at(i * 2 + 1)->height();
            } else {
                reheight = m_pages.at(i * 2)->height();
            }
            m_widgets.at(i)->setGeometry(rex, rey, m_widgets.at(i)->layout()->margin() * 2 + m_widgets.at(i)->layout()->spacing() + m_pages.at(i * 2)->width() + m_pages.at(i * 2 + 1)->width(), m_widgets.at(i)->layout()->margin() * 2 + reheight);
            rey += m_widgets.at(i)->layout()->margin() * 2 + reheight + m_vboxLayout.spacing();
        }
        if (m_widgets.size() % 2) {
            int reheight = m_pages.at(m_pages.size() - 1)->height();
            m_widgets.at(m_widgets.size() / 2)->setGeometry(rex, rey, m_widgets.at(m_widgets.size() / 2)->layout()->margin() * 2 + m_widgets.at(m_widgets.size() / 2)->layout()->spacing() + m_pages.at(m_pages.size() - 1)->width() * 2, m_widgets.at(m_widgets.size() / 2)->layout()->margin() * 2 + reheight);
        }
        break;
    default:
        break;
    }
    pageJump(currpageno);
    donotneedreloaddoc = false;
    m_currentpageno = currpageno;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
}

int DocummentBase::currentPageNo()
{
    if (m_bslidemodel) {
        return m_slidepageno;
    }
    int pagenum = -1;
    int x_offset = 0;
    int y_offset = 0;
    DScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    DScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    switch (m_viewmode) {
    case ViewMode_SinglePage:
        for (int i = 0; i < m_pages.size(); i++) {
            if (y_offset < m_widgets.at(i)->y() + m_widgets.at(i)->height()) {
                pagenum = i;
                break;
            }
        }
        break;
    case ViewMode_FacingPage:
        for (int i = 0; i < m_pages.size() / 2; i++) {
            if (y_offset < m_widgets.at(i)->y() + m_widgets.at(i)->height()) {
                if (x_offset < m_widgets.at(i)->x() + m_pages.at(i * 2)->x() + m_pages.at(i * 2)->width()) {
                    pagenum = i * 2;
                } else {
                    pagenum = i * 2 + 1;
                }
                break;
            }
        }
        if (-1 == pagenum && m_pages.size() % 2) {
            if (y_offset < m_widgets.at(m_pages.size() / 2)->y() + m_widgets.at(m_pages.size() / 2)->height()) {
                if (x_offset < m_widgets.at(m_pages.size() / 2)->x() + m_pages.at(m_pages.size() - 1)->x() + m_pages.at(m_pages.size() - 1)->width()) {
                    pagenum = m_pages.size() - 1;
                } else {
                    pagenum = m_pages.size();
                }
                break;
            }
        }
        break;
    default:
        break;
    }
    return pagenum;
}

void DocummentBase::showSinglePage()
{
    pblankwidget->hide();
    for (int i = 0; i < m_pages.size(); i++) {
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i));
        m_widgets.at(i)->show();
    }
    int rex = m_vboxLayout.margin(), rey = m_vboxLayout.margin();
    for (int i = 0; i < m_widgets.size(); i++) {
        m_widgets.at(i)->setGeometry(rex, rey, m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->width(), m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->height());
        rey += m_widgets.at(i)->layout()->margin() * 2 + m_pages.at(i)->height() + m_vboxLayout.spacing();
    }
}
void DocummentBase::showFacingPage()
{
    for (int i = 0; i < m_widgets.size(); i++) {
        m_widgets.at(i)->hide();
    }
    pblankwidget->hide();
    for (int i = 0; i < m_pages.size() / 2; i++) {
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i * 2));
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i * 2 + 1));
        m_widgets.at(i)->show();
    }
    if (m_pages.size() % 2) {
        pblankwidget->show();
        m_widgets.at(m_pages.size() / 2)->layout()->addWidget(m_pages.at(m_pages.size() - 1));
        m_widgets.at(m_pages.size() / 2)->layout()->addWidget(pblankwidget);
        m_widgets.at(m_pages.size() / 2)->show();
    }
    int rex = m_vboxLayout.margin(), rey = m_vboxLayout.margin();
    for (int i = 0; i < m_widgets.size() / 2; i++) {
        int reheight = 0;
        if (m_pages.at(i * 2)->height() < m_pages.at(i * 2 + 1)->height()) {
            reheight = m_pages.at(i * 2 + 1)->height();
        } else {
            reheight = m_pages.at(i * 2)->height();
        }
        m_widgets.at(i)->setGeometry(rex, rey, m_widgets.at(i)->layout()->margin() * 2 + m_widgets.at(i)->layout()->spacing() + m_pages.at(i * 2)->width() + m_pages.at(i * 2 + 1)->width(), m_widgets.at(i)->layout()->margin() * 2 + reheight);
        rey += m_widgets.at(i)->layout()->margin() * 2 + reheight + m_vboxLayout.spacing();
    }
    if (m_pages.size() % 2) {
        int reheight = m_pages.at(m_pages.size() - 1)->height();
        m_widgets.at(m_widgets.size() / 2)->setGeometry(rex, rey, m_widgets.at(m_widgets.size() / 2)->layout()->margin() * 2 + m_widgets.at(m_widgets.size() / 2)->layout()->spacing() + m_pages.at(m_pages.size() - 1)->width() * 2, m_widgets.at(m_widgets.size() / 2)->layout()->margin() * 2 + reheight);
    }
}

Page::Link *DocummentBase::mouseBeOverLink(QPoint point)
{
    if (!bDocummentExist()) {
        return nullptr;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "mouseBeOverLink pagenum:" << pagenum;
    if (-1 != pagenum) {
        return m_pages.at(pagenum)->ifMouseMoveOverLink(qpoint);
    }
    return nullptr;
}

void DocummentBase::slot_vScrollBarValueChanged(int value)
{
//    qDebug() << "slot_vScrollBarValueChanged" << value;
    if (!donotneedreloaddoc) {
        int pageno = currentPageNo();
        if (m_currentpageno != pageno) {
            m_currentpageno = pageno;
            emit signal_pageChange(m_currentpageno);
        }
        if (m_threadloaddoc.isRunning())
            m_threadloaddoc.setRestart();
        else
            m_threadloaddoc.start();
    }
}

void DocummentBase::slot_hScrollBarValueChanged(int value)
{
//    qDebug() << "slot_hScrollBarValueChanged" << value;
    if (!donotneedreloaddoc) {
        int pageno = currentPageNo();
        if (m_currentpageno != pageno) {
            m_currentpageno = pageno;
            emit signal_pageChange(m_currentpageno);
        }
        if (m_threadloaddoc.isRunning())
            m_threadloaddoc.setRestart();
        else
            m_threadloaddoc.start();
    }
}

bool DocummentBase::getSelectTextString(QString &st)
{
    if (!bDocummentExist()) {
        return false;
    }
    st = "";
    bool bselectexit = false;
    for (int i = 0; i < m_pages.size(); i++) {
        QString stpage = "";
        if (m_pages.at(i)->getSelectTextString(stpage)) {
            bselectexit = true;
            st += stpage;
        }
    }
    return bselectexit;
}

bool DocummentBase::showSlideModel()
{
    if (!bDocummentExist()) {
        return false;
    }
    int curpageno = currentPageNo();
    if (curpageno < 0) {
        curpageno = 0;
    }
    m_bslidemodel = true;
    this->hide();
    m_slidewidget->show();
    if (pageJump(curpageno)) {
        return true;
    }
    m_slidepageno = -1;
    m_bslidemodel = false;
    this->show();
    m_slidewidget->hide();
    return false;
}

bool DocummentBase::setViewModeAndShow(ViewMode_EM viewmode)
{
    int currpageno = m_currentpageno;
    m_viewmode = viewmode;
    donotneedreloaddoc = true;
    switch (m_viewmode) {
    case ViewMode_SinglePage:
        showSinglePage();
        break;
    case ViewMode_FacingPage:
        showFacingPage();
        break;
    default:
        return false;
        break;
    }
    pageJump(currpageno);
    donotneedreloaddoc = false;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
    return true;;
}

#include "pagepdf.h"
#include <QPainter>
#include <QDebug>

PagePdf::PagePdf(QWidget *parent)
    : PageBase(parent),
      m_imagewidth(0.01),
      m_imageheight(0.01)
{
}

void PagePdf::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter qpainter(this);
    qpainter.setBrush(QColor(72, 118, 255, 100));
    QPen qpen(QColor(72, 118, 255, 0), 0);
    qpainter.setPen(qpen);
    for (int i = 0; i < paintrects.size(); i++) {
        qpainter.drawRect(paintrects[i]);
    }
}

void PagePdf::clearPageTextSelections()
{
    if (paintrects.size() > 0) {
        paintrects.clear();
        update();
    }
}

bool PagePdf::pageTextSelections(const QPoint start, const QPoint end)
{
    QPoint startC = start;
    QPoint endC = end;
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
    const double scaleX = width() / m_imagewidth;
    const double scaleY = height() / m_imageheight;
    for (int i = 0; i < m_words.size(); i++) {
        tmp = m_words.at(i).rect;
        if (startC.x() > (tmp.x() * scaleX + x()) &&
                startC.x() < (tmp.x() * scaleX + tmp.width() * scaleX + x()) &&
                startC.y() > (tmp.y() * scaleY + y()) &&
                startC.y() < (tmp.y() * scaleY + tmp.height() * scaleY + y())) {
            startword = i;
        }
        if (endC.x() > (tmp.x() * scaleX + x()) &&
                endC.x() < (tmp.x() * scaleX + tmp.width() * scaleX + x()) &&
                endC.y() > (tmp.y() * scaleY + y()) &&
                endC.y() < (tmp.y() * scaleY + tmp.height() * scaleY + y())) {
            stopword = i;
        }
    }
    qDebug() << " startword:" << startword << " stopword:" << stopword;
    if (-1 == startword && stopword == -1) {
        int i;
        for (i = 0; i < m_words.size(); i++) {
            tmp = m_words.at(i).rect;
            if (start_end.intersects(QRect(tmp.x() * scaleX + x(),
                                           tmp.y() * scaleY + y(), tmp.width() * scaleX,
                                           tmp.height() * scaleY))) {
                qDebug() << "break i:" << i;
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
                rect = QRect(tmp.x() * scaleX + x(), tmp.y() * scaleY + y(),
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
                rect = QRect(tmp.x() * scaleX + x(), tmp.y() * scaleY + y(),
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
                rect = QRect(tmp.x() * scaleX + x(), tmp.y() * scaleY + y(),
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
                rect = QRect(tmp.x() * scaleX + x(), tmp.y() * scaleY + y(),
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
            paintrects.append(QRect(tmp.x() * scaleX, tmp.y() * scaleY, tmp.width() * scaleX,
                                    tmp.height() * scaleY));
            tmp = tmpafter;
        }
    }

    paintrects.append(
        QRect(tmp.x() * scaleX, tmp.y() * scaleY, tmp.width() * scaleX, tmp.height() * scaleY));
    update();
    return true;
}

bool PagePdf::ifMouseMoveOverText(const QPoint point)
{
    const double scaleX = width() / m_imagewidth;
    const double scaleY = height() / m_imageheight;
    QPoint qp = QPoint((point.x() - x()) / scaleX, (point.y() - y()) / scaleY);
    for (int i = 0; i < m_words.size(); i++) {
        if (qp.x() > m_words.at(i).rect.x() &&
                qp.x() < m_words.at(i).rect.x() + m_words.at(i).rect.width() &&
                qp.y() > m_words.at(i).rect.y() &&
                qp.y() < m_words.at(i).rect.y() + m_words.at(i).rect.height()) {
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

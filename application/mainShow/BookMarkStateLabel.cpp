#include "BookMarkStateLabel.h"
#include "subjectObserver/ImageHeader.h"

#include "controller/NotifySubject.h"

BookMarkStateLabel::BookMarkStateLabel(DWidget *parent)
    : QLabel ( parent)
{
    setFixedSize(QSize(39, 39));

    setMouseTracking(true);
}

void BookMarkStateLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bChecked) {
        setPixmapState(g_checked_state);
    } else {
        setPixmapState(g_hover_state);
    }
}

void BookMarkStateLabel::mousePressEvent(QMouseEvent *event)
{
    m_bChecked = !m_bChecked;

    if (!m_bChecked) {
        setPixmapState(g_press_state);
    } else {
        setPixmapState(g_checked_state);
    }
}

void BookMarkStateLabel::leaveEvent(QEvent *event)
{
    if (m_bChecked) {
        setPixmapState(g_checked_state);
    } else {
        this->clear();
    }
}

void BookMarkStateLabel::setPixmapState(const QString &state)
{
    QString ssPath = QString(":/resources/image/%1/bookmark.svg").arg(state);
    QPixmap pixmap(ssPath);
    this->setPixmap(pixmap);
}

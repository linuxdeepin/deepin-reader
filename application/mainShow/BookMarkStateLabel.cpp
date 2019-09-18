#include "BookMarkStateLabel.h"
#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"
#include "controller/MsgSubject.h"

BookMarkStateLabel::BookMarkStateLabel(DWidget *parent)
    : QLabel (parent)
{
    setFixedSize(QSize(39, 39));

    setMouseTracking(true);
}

void BookMarkStateLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bChecked) {
        setToolTip(tr("delete bookmark"));
        setPixmapState(ImageModule::g_checked_state);
    } else {
        setToolTip(tr("add bookmark"));
        setPixmapState(ImageModule::g_hover_state);
    }
    DLabel::mouseMoveEvent(event);
}

void BookMarkStateLabel::mousePressEvent(QMouseEvent *event)
{
    m_bChecked = !m_bChecked;

    if (!m_bChecked) {
        setPixmapState(ImageModule::g_press_state);
        MsgSubject::getInstance()->sendMsg(nullptr, MSG_BOOKMARK_DLTITEM, "");
    } else {
        setPixmapState(ImageModule::g_checked_state);
        MsgSubject::getInstance()->sendMsg(nullptr, MSG_BOOKMARK_ADDITEM, "");
    }

    DLabel::mousePressEvent(event);
}

void BookMarkStateLabel::leaveEvent(QEvent *event)
{
    if (m_bChecked) {
        setPixmapState(ImageModule::g_checked_state);
    } else {
        this->clear();
    }

    DLabel::leaveEvent(event);
}

void BookMarkStateLabel::setPixmapState(const QString &state)
{
    QString ssPath = QString(":/resources/image/%1/bookmark.svg").arg(state);
    QPixmap pixmap(ssPath);
    this->setPixmap(pixmap);
}

void BookMarkStateLabel::SlotSetMarkState(const bool &bCheck)
{
    m_bChecked = bCheck;

    if (!m_bChecked) {
        this->clear();
    } else {
        setPixmapState(ImageModule::g_checked_state);
    }
}

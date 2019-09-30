#include "BookMarkStateLabel.h"
#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"
#include "controller/MsgSubject.h"
#include "translator/PdfControl.h"
#include "docview/docummentproxy.h"

BookMarkStateLabel::BookMarkStateLabel(DWidget *parent)
    : QLabel (parent)
{
    setFixedSize(QSize(39, 39));

    setMouseTracking(true);
    setObserverName();

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }
    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

void BookMarkStateLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bChecked) {
        setToolTip(PdfControl::DLT_BK);
        setPixmapState(ImageModule::g_checked_state);
    } else {
        setToolTip(PdfControl::ADD_BK);
        setPixmapState(ImageModule::g_hover_state);
    }
    DLabel::mouseMoveEvent(event);
}

void BookMarkStateLabel::mousePressEvent(QMouseEvent *event)
{
    m_bChecked = !m_bChecked;
    //  将当前 添加\删除 到书签
    int nCurPage = DocummentProxy::instance()->currentPageNo();

    if (!m_bChecked) {
        setPixmapState(ImageModule::g_press_state);
        sendMsg(MSG_BOOKMARK_DLTITEM, QString("%1").arg(nCurPage));
    } else {
        setPixmapState(ImageModule::g_checked_state);
        sendMsg(MSG_BOOKMARK_ADDITEM, QString("%1").arg(nCurPage));
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

void BookMarkStateLabel::setMarkState(const QString &sData)
{
    m_bChecked = sData.toInt();

    if (!m_bChecked) {
        this->clear();
    } else {
        setPixmapState(ImageModule::g_checked_state);
    }
}

bool BookMarkStateLabel::bChecked() const
{
    return m_bChecked;
}

int BookMarkStateLabel::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_BOOKMARK_STATE) {     //  当前页的书签状态
        setMarkState(msgContent);
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}

void BookMarkStateLabel::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(this, msgType, msgContent);
}

void BookMarkStateLabel::setObserverName()
{
    m_strObserverName = "BookMarkStateLabel";
}

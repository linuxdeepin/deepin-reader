#ifndef BOOKMARKSTATELABEL_H
#define BOOKMARKSTATELABEL_H

#include <DLabel>
#include <DWidget>

#include "subjectObserver/IObserver.h"
#include "controller/NotifySubject.h"
#include "controller/MsgSubject.h"

/**
 * @brief The BookMarkStateWidget class
 * @brief   主面板， 当前页的 书签状态
 *
 *
 */

DWIDGET_USE_NAMESPACE

class BookMarkStateLabel : public DLabel, public IObserver
{
    Q_OBJECT
public:
    BookMarkStateLabel(DWidget *parent = nullptr);

public:
    bool bChecked() const;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setPixmapState(const QString &);
    void setMarkState(const bool &);

private:
    bool            m_bChecked = false;
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &, const QString &msgContent = "") override;

private:
    void setObserverName(const QString &name) override;
};

#endif // BOOKMARKSTATEWIDGT_H
